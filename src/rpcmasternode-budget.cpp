// Copyright (c) 2014-2015 The Dash Developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "activemasternode.h"
#include "db.h"
#include "init.h"
#include "main.h"
#include "masternode-budget.h"
#include "masternode-payments.h"
#include "masternodeconfig.h"
#include "masternodeman.h"
#include "rpcserver.h"
#include "utilmoneystr.h"

#include <fstream>
using namespace json_spirit;
using namespace std;

Value mnbudget(const Array& params, bool fHelp)
{
    string strCommand;
    if (params.size() >= 1)
        strCommand = params[0].get_str();

    if (fHelp ||
        (strCommand != "vote-alias" && strCommand != "vote-many" && strCommand != "prepare" && strCommand != "submit" && strCommand != "vote" && strCommand != "getvotes" && strCommand != "getinfo" && strCommand != "show" && strCommand != "projection" && strCommand != "check" && strCommand != "nextblock"))
        throw runtime_error(
            "mnbudget \"command\"... ( \"passphrase\" )\n"
            "Vote or show current budgets\n"
            "\nAvailable commands:\n"
            "  prepare            - Prepare proposal for network by signing and creating tx\n"
            "  submit             - Submit proposal for network\n"
            "  vote-many          - Vote on a Pivx initiative\n"
            "  vote-alias         - Vote on a Pivx initiative\n"
            "  vote               - Vote on a Pivx initiative/budget\n"
            "  getvotes           - Show current masternode budgets\n"
            "  getinfo            - Show current masternode budgets\n"
            "  show               - Show all budgets\n"
            "  projection         - Show the projection of which proposals will be paid the next cycle\n"
            "  check              - Scan proposals and remove invalid\n"
            "  nextblock          - Get next superblock for budget system\n");

    if (strCommand == "nextblock") {
        CBlockIndex* pindexPrev = chainActive.Tip();
        if (!pindexPrev) return "unknown";

        int nNext = pindexPrev->nHeight - pindexPrev->nHeight % GetBudgetPaymentCycleBlocks() + GetBudgetPaymentCycleBlocks();
        return nNext;
    }

    if (strCommand == "prepare") {
        int nBlockMin = 0;
        CBlockIndex* pindexPrev = chainActive.Tip();

        if (params.size() != 7)
            throw runtime_error("Correct usage is 'mnbudget prepare proposal-name url payment_count block_start pivx_address monthly_payment_pivx'");

        std::string strProposalName = params[1].get_str();
        if (strProposalName.size() > 20)
            return "Invalid proposal name, limit of 20 characters.";

        std::string strURL = params[2].get_str();
        if (strURL.size() > 64)
            return "Invalid url, limit of 64 characters.";

        int nPaymentCount = params[3].get_int();
        if (nPaymentCount < 1)
            return "Invalid payment count, must be more than zero.";

        //set block min
        if (pindexPrev != NULL) nBlockMin = pindexPrev->nHeight - GetBudgetPaymentCycleBlocks() * (nPaymentCount + 1);

        int nBlockStart = params[4].get_int();
        if (nBlockStart % GetBudgetPaymentCycleBlocks() != 0) {
            int nNext = pindexPrev->nHeight - pindexPrev->nHeight % GetBudgetPaymentCycleBlocks() + GetBudgetPaymentCycleBlocks();
            return strprintf("Invalid block start - must be a budget cycle block. Next valid block: %d", nNext);
        }

        int nBlockEnd = nBlockStart + GetBudgetPaymentCycleBlocks() * nPaymentCount;

        if (nBlockStart < nBlockMin)
            return "Invalid block start, must be more than current height.";

        if (nBlockEnd < pindexPrev->nHeight)
            return "Invalid ending block, starting block + (payment_cycle*payments) must be more than current height.";

        CBitcoinAddress address(params[5].get_str());
        if (!address.IsValid())
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid Pivx address");

        // Parse Pivx address
        CScript scriptPubKey = GetScriptForDestination(address.Get());
        CAmount nAmount = AmountFromValue(params[6]);

        //*************************************************************************

        // create transaction 15 minutes into the future, to allow for confirmation time
        CBudgetProposalBroadcast budgetProposalBroadcast(strProposalName, strURL, nPaymentCount, scriptPubKey, nAmount, nBlockStart, 0);

        std::string strError = "";
        if (!budgetProposalBroadcast.IsValid(strError, false))
            return "Proposal is not valid - " + budgetProposalBroadcast.GetHash().ToString() + " - " + strError;

        bool useIX = false; //true;
        // if (params.size() > 7) {
        //     if(params[7].get_str() != "false" && params[7].get_str() != "true")
        //         return "Invalid use_ix, must be true or false";
        //     useIX = params[7].get_str() == "true" ? true : false;
        // }

        CWalletTx wtx;
        if (!pwalletMain->GetBudgetSystemCollateralTX(wtx, budgetProposalBroadcast.GetHash(), useIX)) {
            return "Error making collateral transaction for proposal. Please check your wallet balance and make sure your wallet is unlocked.";
        }

        // make our change address
        CReserveKey reservekey(pwalletMain);
        //send the tx to the network
        pwalletMain->CommitTransaction(wtx, reservekey, useIX ? "ix" : "tx");

        return wtx.GetHash().ToString();
    }

    if (strCommand == "submit") {
        int nBlockMin = 0;
        CBlockIndex* pindexPrev = chainActive.Tip();

        if (params.size() != 8)
            throw runtime_error("Correct usage is 'mnbudget submit proposal-name url payment_count block_start pivx_address monthly_payment_pivx fee_tx'");

        // Check these inputs the same way we check the vote commands:
        // **********************************************************

        std::string strProposalName = params[1].get_str();
        if (strProposalName.size() > 20)
            return "Invalid proposal name, limit of 20 characters.";

        std::string strURL = params[2].get_str();
        if (strURL.size() > 64)
            return "Invalid url, limit of 64 characters.";

        int nPaymentCount = params[3].get_int();
        if (nPaymentCount < 1)
            return "Invalid payment count, must be more than zero.";

        //set block min
        if (pindexPrev != NULL) nBlockMin = pindexPrev->nHeight - GetBudgetPaymentCycleBlocks() * (nPaymentCount + 1);

        int nBlockStart = params[4].get_int();
        if (nBlockStart % GetBudgetPaymentCycleBlocks() != 0) {
            int nNext = pindexPrev->nHeight - pindexPrev->nHeight % GetBudgetPaymentCycleBlocks() + GetBudgetPaymentCycleBlocks();
            return strprintf("Invalid block start - must be a budget cycle block. Next valid block: %d", nNext);
        }

        int nBlockEnd = nBlockStart + (GetBudgetPaymentCycleBlocks() * nPaymentCount);

        if (nBlockStart < nBlockMin)
            return "Invalid payment count, must be more than current height.";

        if (nBlockEnd < pindexPrev->nHeight)
            return "Invalid ending block, starting block + (payment_cycle*payments) must be more than current height.";

        CBitcoinAddress address(params[5].get_str());
        if (!address.IsValid())
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid Pivx address");

        // Parse Pivx address
        CScript scriptPubKey = GetScriptForDestination(address.Get());
        CAmount nAmount = AmountFromValue(params[6]);
        uint256 hash = ParseHashV(params[7], "parameter 1");

        //create the proposal incase we're the first to make it
        CBudgetProposalBroadcast budgetProposalBroadcast(strProposalName, strURL, nPaymentCount, scriptPubKey, nAmount, nBlockStart, hash);

        std::string strError = "";
        int nConf = 0;
        if (!IsBudgetCollateralValid(hash, budgetProposalBroadcast.GetHash(), strError, budgetProposalBroadcast.nTime, nConf)) {
            return "Proposal FeeTX is not valid - " + hash.ToString() + " - " + strError;
        }

        if (!masternodeSync.IsBlockchainSynced()) {
            return "Must wait for client to sync with masternode network. Try again in a minute or so.";
        }

        // if(!budgetProposalBroadcast.IsValid(strError)){
        //     return "Proposal is not valid - " + budgetProposalBroadcast.GetHash().ToString() + " - " + strError;
        // }

        budget.mapSeenMasternodeBudgetProposals.insert(make_pair(budgetProposalBroadcast.GetHash(), budgetProposalBroadcast));
        budgetProposalBroadcast.Relay();
        if(budget.AddProposal(budgetProposalBroadcast)) {
            return budgetProposalBroadcast.GetHash().ToString();
        }
        return "Invalid proposal, see debug.log for details.";
        
    }

    if (strCommand == "vote-many") {
        if (params.size() != 3)
            throw runtime_error("Correct usage is 'mnbudget vote-many proposal-hash yes|no'");

        uint256 hash = ParseHashV(params[1], "parameter 1");
        std::string strVote = params[2].get_str();

        if (strVote != "yes" && strVote != "no") return "You can only vote 'yes' or 'no'";
        int nVote = VOTE_ABSTAIN;
        if (strVote == "yes") nVote = VOTE_YES;
        if (strVote == "no") nVote = VOTE_NO;

        int success = 0;
        int failed = 0;

        Object resultsObj;

        BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
            std::string errorMessage;
            std::vector<unsigned char> vchMasterNodeSignature;
            std::string strMasterNodeSignMessage;

            CPubKey pubKeyCollateralAddress;
            CKey keyCollateralAddress;
            CPubKey pubKeyMasternode;
            CKey keyMasternode;

            Object statusObj;

            if (!obfuScationSigner.SetKey(mne.getPrivKey(), errorMessage, keyMasternode, pubKeyMasternode)) {
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Masternode signing error, could not set key correctly: " + errorMessage));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }

            CMasternode* pmn = mnodeman.Find(pubKeyMasternode);
            if (pmn == NULL) {
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Can't find masternode by pubkey"));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }

            CBudgetVote vote(pmn->vin, hash, nVote);
            if (!vote.Sign(keyMasternode, pubKeyMasternode)) {
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Failure to sign."));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }


            std::string strError = "";
            if (budget.UpdateProposal(vote, NULL, strError)) {
                budget.mapSeenMasternodeBudgetVotes.insert(make_pair(vote.GetHash(), vote));
                vote.Relay();
                success++;
                statusObj.push_back(Pair("result", "success"));
            } else {
                failed++;
                statusObj.push_back(Pair("result", strError.c_str()));
            }

            resultsObj.push_back(Pair(mne.getAlias(), statusObj));
        }

        Object returnObj;
        returnObj.push_back(Pair("overall", strprintf("Voted successfully %d time(s) and failed %d time(s).", success, failed)));
        returnObj.push_back(Pair("detail", resultsObj));

        return returnObj;
    }

if(strCommand == "vote-alias")
    {
        if(params.size() != 4)
            throw runtime_error("Correct usage is 'mnbudget vote-alias proposal-hash yes|no alias-name'");

        uint256 hash;
        std::string strVote;

        hash = ParseHashV(params[1], "Proposal hash");
        strVote = params[2].get_str();
        std::string strAlias = params[3].get_str();

        if(strVote != "yes" && strVote != "no") return "You can only vote 'yes' or 'no'";
        int nVote = VOTE_ABSTAIN;
        if(strVote == "yes") nVote = VOTE_YES;
        if(strVote == "no") nVote = VOTE_NO;

        int success = 0;
        int failed = 0;

        std::vector<CMasternodeConfig::CMasternodeEntry> mnEntries;
        mnEntries = masternodeConfig.getEntries();

        Object resultsObj;

        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {

            if( strAlias != mne.getAlias()) continue;

            std::string errorMessage;
            std::vector<unsigned char> vchMasterNodeSignature;
            std::string strMasterNodeSignMessage;

            CPubKey pubKeyCollateralAddress;
            CKey keyCollateralAddress;
            CPubKey pubKeyMasternode;
            CKey keyMasternode;

            Object statusObj;

            if(!obfuScationSigner.SetKey(mne.getPrivKey(), errorMessage, keyMasternode, pubKeyMasternode)){
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Masternode signing error, could not set key correctly: " + errorMessage));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }

            CMasternode* pmn = mnodeman.Find(pubKeyMasternode);
            if(pmn == NULL)
            {
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Can't find masternode by pubkey"));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }

            CBudgetVote vote(pmn->vin, hash, nVote);
            if(!vote.Sign(keyMasternode, pubKeyMasternode)){
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Failure to sign."));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }


            std::string strError = "";
            if(budget.UpdateProposal(vote, NULL, strError)) {
                budget.mapSeenMasternodeBudgetVotes.insert(make_pair(vote.GetHash(), vote));
                vote.Relay();
                success++;
                statusObj.push_back(Pair("result", "success"));
            } else {
                failed++;
                statusObj.push_back(Pair("result", strError.c_str()));
            }

            resultsObj.push_back(Pair(mne.getAlias(), statusObj));
        }

        Object returnObj;
        returnObj.push_back(Pair("overall", strprintf("Voted successfully %d time(s) and failed %d time(s).", success, failed)));
        returnObj.push_back(Pair("detail", resultsObj));

        return returnObj;
}

    if (strCommand == "vote") {
        if (params.size() != 3)
            throw runtime_error("Correct usage is 'mnbudget vote proposal-hash yes|no'");

        uint256 hash = ParseHashV(params[1], "parameter 1");
        std::string strVote = params[2].get_str();

        if (strVote != "yes" && strVote != "no") return "You can only vote 'yes' or 'no'";
        int nVote = VOTE_ABSTAIN;
        if (strVote == "yes") nVote = VOTE_YES;
        if (strVote == "no") nVote = VOTE_NO;

        CPubKey pubKeyMasternode;
        CKey keyMasternode;
        std::string errorMessage;

        if (!obfuScationSigner.SetKey(strMasterNodePrivKey, errorMessage, keyMasternode, pubKeyMasternode))
            return "Error upon calling SetKey";

        CMasternode* pmn = mnodeman.Find(activeMasternode.vin);
        if (pmn == NULL) {
            return "Failure to find masternode in list : " + activeMasternode.vin.ToString();
        }

        CBudgetVote vote(activeMasternode.vin, hash, nVote);
        if (!vote.Sign(keyMasternode, pubKeyMasternode)) {
            return "Failure to sign.";
        }

        std::string strError = "";
        if (budget.UpdateProposal(vote, NULL, strError)) {
            budget.mapSeenMasternodeBudgetVotes.insert(make_pair(vote.GetHash(), vote));
            vote.Relay();
            return "Voted successfully";
        } else {
            return "Error voting : " + strError;
        }
    }

    if (strCommand == "projection") {
        Object resultObj;
        CAmount nTotalAllotted = 0;

        std::vector<CBudgetProposal*> winningProps = budget.GetBudget();
        BOOST_FOREACH (CBudgetProposal* pbudgetProposal, winningProps) {
            nTotalAllotted += pbudgetProposal->GetAllotted();

            CTxDestination address1;
            ExtractDestination(pbudgetProposal->GetPayee(), address1);
            CBitcoinAddress address2(address1);

            Object bObj;
            bObj.push_back(Pair("URL", pbudgetProposal->GetURL()));
            bObj.push_back(Pair("Hash", pbudgetProposal->GetHash().ToString()));
            bObj.push_back(Pair("BlockStart", (int64_t)pbudgetProposal->GetBlockStart()));
            bObj.push_back(Pair("BlockEnd", (int64_t)pbudgetProposal->GetBlockEnd()));
            bObj.push_back(Pair("TotalPaymentCount", (int64_t)pbudgetProposal->GetTotalPaymentCount()));
            bObj.push_back(Pair("RemainingPaymentCount", (int64_t)pbudgetProposal->GetRemainingPaymentCount()));
            bObj.push_back(Pair("PaymentAddress", address2.ToString()));
            bObj.push_back(Pair("Ratio", pbudgetProposal->GetRatio()));
            bObj.push_back(Pair("Yeas", (int64_t)pbudgetProposal->GetYeas()));
            bObj.push_back(Pair("Nays", (int64_t)pbudgetProposal->GetNays()));
            bObj.push_back(Pair("Abstains", (int64_t)pbudgetProposal->GetAbstains()));
            bObj.push_back(Pair("TotalPayment", ValueFromAmount(pbudgetProposal->GetAmount() * pbudgetProposal->GetTotalPaymentCount())));
            bObj.push_back(Pair("MonthlyPayment", ValueFromAmount(pbudgetProposal->GetAmount())));
            bObj.push_back(Pair("Alloted", ValueFromAmount(pbudgetProposal->GetAllotted())));
            bObj.push_back(Pair("TotalBudgetAlloted", ValueFromAmount(nTotalAllotted)));

            std::string strError = "";
            bObj.push_back(Pair("IsValid", pbudgetProposal->IsValid(strError)));
            bObj.push_back(Pair("IsValidReason", strError.c_str()));
            bObj.push_back(Pair("fValid", pbudgetProposal->fValid));

            resultObj.push_back(Pair(pbudgetProposal->GetName(), bObj));
        }

        return resultObj;
    }

    if (strCommand == "show") {
        std::string strShow = "valid";
        if (params.size() == 2)
            std::string strProposalName = params[1].get_str();

        Object resultObj;
        int64_t nTotalAllotted = 0;

        std::vector<CBudgetProposal*> winningProps = budget.GetAllProposals();
        BOOST_FOREACH (CBudgetProposal* pbudgetProposal, winningProps) {
            if (strShow == "valid" && !pbudgetProposal->fValid) continue;

            nTotalAllotted += pbudgetProposal->GetAllotted();

            CTxDestination address1;
            ExtractDestination(pbudgetProposal->GetPayee(), address1);
            CBitcoinAddress address2(address1);

            Object bObj;
            bObj.push_back(Pair("Name", pbudgetProposal->GetName()));
            bObj.push_back(Pair("URL", pbudgetProposal->GetURL()));
            bObj.push_back(Pair("Hash", pbudgetProposal->GetHash().ToString()));
            bObj.push_back(Pair("FeeHash", pbudgetProposal->nFeeTXHash.ToString()));
            bObj.push_back(Pair("BlockStart", (int64_t)pbudgetProposal->GetBlockStart()));
            bObj.push_back(Pair("BlockEnd", (int64_t)pbudgetProposal->GetBlockEnd()));
            bObj.push_back(Pair("TotalPaymentCount", (int64_t)pbudgetProposal->GetTotalPaymentCount()));
            bObj.push_back(Pair("RemainingPaymentCount", (int64_t)pbudgetProposal->GetRemainingPaymentCount()));
            bObj.push_back(Pair("PaymentAddress", address2.ToString()));
            bObj.push_back(Pair("Ratio", pbudgetProposal->GetRatio()));
            bObj.push_back(Pair("Yeas", (int64_t)pbudgetProposal->GetYeas()));
            bObj.push_back(Pair("Nays", (int64_t)pbudgetProposal->GetNays()));
            bObj.push_back(Pair("Abstains", (int64_t)pbudgetProposal->GetAbstains()));
            bObj.push_back(Pair("TotalPayment", ValueFromAmount(pbudgetProposal->GetAmount() * pbudgetProposal->GetTotalPaymentCount())));
            bObj.push_back(Pair("MonthlyPayment", ValueFromAmount(pbudgetProposal->GetAmount())));

            bObj.push_back(Pair("IsEstablished", pbudgetProposal->IsEstablished()));

            std::string strError = "";
            bObj.push_back(Pair("IsValid", pbudgetProposal->IsValid(strError)));
            bObj.push_back(Pair("IsValidReason", strError.c_str()));
            bObj.push_back(Pair("fValid", pbudgetProposal->fValid));

            resultObj.push_back(Pair(pbudgetProposal->GetName(), bObj));
        }

        return resultObj;
    }

    if (strCommand == "getinfo") {
        if (params.size() != 2)
            throw runtime_error("Correct usage is 'mnbudget getinfo profilename'");

        std::string strProposalName = params[1].get_str();

        CBudgetProposal* pbudgetProposal = budget.FindProposal(strProposalName);

        if (pbudgetProposal == NULL) return "Unknown proposal name";

        CTxDestination address1;
        ExtractDestination(pbudgetProposal->GetPayee(), address1);
        CBitcoinAddress address2(address1);

        Object obj;
        obj.push_back(Pair("Name", pbudgetProposal->GetName()));
        obj.push_back(Pair("Hash", pbudgetProposal->GetHash().ToString()));
        obj.push_back(Pair("FeeHash", pbudgetProposal->nFeeTXHash.ToString()));
        obj.push_back(Pair("URL", pbudgetProposal->GetURL()));
        obj.push_back(Pair("BlockStart", (int64_t)pbudgetProposal->GetBlockStart()));
        obj.push_back(Pair("BlockEnd", (int64_t)pbudgetProposal->GetBlockEnd()));
        obj.push_back(Pair("TotalPaymentCount", (int64_t)pbudgetProposal->GetTotalPaymentCount()));
        obj.push_back(Pair("RemainingPaymentCount", (int64_t)pbudgetProposal->GetRemainingPaymentCount()));
        obj.push_back(Pair("PaymentAddress", address2.ToString()));
        obj.push_back(Pair("Ratio", pbudgetProposal->GetRatio()));
        obj.push_back(Pair("Yeas", (int64_t)pbudgetProposal->GetYeas()));
        obj.push_back(Pair("Nays", (int64_t)pbudgetProposal->GetNays()));
        obj.push_back(Pair("Abstains", (int64_t)pbudgetProposal->GetAbstains()));
        obj.push_back(Pair("TotalPayment", ValueFromAmount(pbudgetProposal->GetAmount() * pbudgetProposal->GetTotalPaymentCount())));
        obj.push_back(Pair("MonthlyPayment", ValueFromAmount(pbudgetProposal->GetAmount())));

        obj.push_back(Pair("IsEstablished", pbudgetProposal->IsEstablished()));

        std::string strError = "";
        obj.push_back(Pair("IsValid", pbudgetProposal->IsValid(strError)));
        obj.push_back(Pair("IsValidReason", strError.c_str()));
        obj.push_back(Pair("fValid", pbudgetProposal->fValid));

        return obj;
    }

    if (strCommand == "getvotes") {
        if (params.size() != 2)
            throw runtime_error("Correct usage is 'mnbudget getvotes profilename'");

        std::string strProposalName = params[1].get_str();

        Object obj;

        CBudgetProposal* pbudgetProposal = budget.FindProposal(strProposalName);

        if (pbudgetProposal == NULL) return "Unknown proposal name";

        std::map<uint256, CBudgetVote>::iterator it = pbudgetProposal->mapVotes.begin();
        while (it != pbudgetProposal->mapVotes.end()) {
            Object bObj;
            bObj.push_back(Pair("nHash", (*it).first.ToString().c_str()));
            bObj.push_back(Pair("Vote", (*it).second.GetVoteString()));
            bObj.push_back(Pair("nTime", (int64_t)(*it).second.nTime));
            bObj.push_back(Pair("fValid", (*it).second.fValid));

            obj.push_back(Pair((*it).second.vin.prevout.ToStringShort(), bObj));

            it++;
        }


        return obj;
    }

    if (strCommand == "check") {
        budget.CheckAndRemove();

        return "Success";
    }

    return Value::null;
}

Value mnbudgetvoteraw(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 6)
        throw runtime_error(
            "mnbudgetvoteraw <masternode-tx-hash> <masternode-tx-index> <proposal-hash> <yes|no> <time> <vote-sig>\n"
            "Compile and relay a proposal vote with provided external signature instead of signing vote internally\n");

    uint256 hashMnTx = ParseHashV(params[0], "mn tx hash");
    int nMnTxIndex = params[1].get_int();
    CTxIn vin = CTxIn(hashMnTx, nMnTxIndex);

    uint256 hashProposal = ParseHashV(params[2], "Proposal hash");
    std::string strVote = params[3].get_str();

    if (strVote != "yes" && strVote != "no") return "You can only vote 'yes' or 'no'";
    int nVote = VOTE_ABSTAIN;
    if (strVote == "yes") nVote = VOTE_YES;
    if (strVote == "no") nVote = VOTE_NO;

    int64_t nTime = params[4].get_int64();
    std::string strSig = params[5].get_str();
    bool fInvalid = false;
    vector<unsigned char> vchSig = DecodeBase64(strSig.c_str(), &fInvalid);

    if (fInvalid)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Malformed base64 encoding");

    CMasternode* pmn = mnodeman.Find(vin);
    if (pmn == NULL) {
        return "Failure to find masternode in list : " + vin.ToString();
    }

    CBudgetVote vote(vin, hashProposal, nVote);
    vote.nTime = nTime;
    vote.vchSig = vchSig;

    if (!vote.SignatureValid(true)) {
        return "Failure to verify signature.";
    }

    std::string strError = "";
    if (budget.UpdateProposal(vote, NULL, strError)) {
        budget.mapSeenMasternodeBudgetVotes.insert(make_pair(vote.GetHash(), vote));
        vote.Relay();
        return "Voted successfully";
    } else {
        return "Error voting : " + strError;
    }
}

Value mnfinalbudget(const Array& params, bool fHelp)
{
    string strCommand;
    if (params.size() >= 1)
        strCommand = params[0].get_str();

    if (fHelp ||
        (strCommand != "suggest" && strCommand != "vote-many" && strCommand != "vote" && strCommand != "show" && strCommand != "getvotes"))
        throw runtime_error(
            "mnfinalbudget \"command\"... ( \"passphrase\" )\n"
            "Vote or show current budgets\n"
            "\nAvailable commands:\n"
            "  vote-many   - Vote on a finalized budget\n"
            "  vote        - Vote on a finalized budget\n"
            "  show        - Show existing finalized budgets\n"
            "  getvotes     - Get vote information for each finalized budget\n");

    if (strCommand == "vote-many") {
        if (params.size() != 2)
            throw runtime_error("Correct usage is 'mnfinalbudget vote-many BUDGET_HASH'");

        std::string strHash = params[1].get_str();
        uint256 hash(strHash);

        int success = 0;
        int failed = 0;

        Object resultsObj;

        BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
            std::string errorMessage;
            std::vector<unsigned char> vchMasterNodeSignature;
            std::string strMasterNodeSignMessage;

            CPubKey pubKeyCollateralAddress;
            CKey keyCollateralAddress;
            CPubKey pubKeyMasternode;
            CKey keyMasternode;

            Object statusObj;

            if (!obfuScationSigner.SetKey(mne.getPrivKey(), errorMessage, keyMasternode, pubKeyMasternode)) {
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Masternode signing error, could not set key correctly: " + errorMessage));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }

            CMasternode* pmn = mnodeman.Find(pubKeyMasternode);
            if (pmn == NULL) {
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Can't find masternode by pubkey"));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }


            CFinalizedBudgetVote vote(pmn->vin, hash);
            if (!vote.Sign(keyMasternode, pubKeyMasternode)) {
                failed++;
                statusObj.push_back(Pair("result", "failed"));
                statusObj.push_back(Pair("errorMessage", "Failure to sign."));
                resultsObj.push_back(Pair(mne.getAlias(), statusObj));
                continue;
            }

            std::string strError = "";
            if (budget.UpdateFinalizedBudget(vote, NULL, strError)) {
                budget.mapSeenFinalizedBudgetVotes.insert(make_pair(vote.GetHash(), vote));
                vote.Relay();
                success++;
                statusObj.push_back(Pair("result", "success"));
            } else {
                failed++;
                statusObj.push_back(Pair("result", strError.c_str()));
            }

            resultsObj.push_back(Pair(mne.getAlias(), statusObj));
        }

        Object returnObj;
        returnObj.push_back(Pair("overall", strprintf("Voted successfully %d time(s) and failed %d time(s).", success, failed)));
        returnObj.push_back(Pair("detail", resultsObj));

        return returnObj;
    }

    if (strCommand == "vote") {
        if (params.size() != 2)
            throw runtime_error("Correct usage is 'mnfinalbudget vote BUDGET_HASH'");

        std::string strHash = params[1].get_str();
        uint256 hash(strHash);

        CPubKey pubKeyMasternode;
        CKey keyMasternode;
        std::string errorMessage;

        if (!obfuScationSigner.SetKey(strMasterNodePrivKey, errorMessage, keyMasternode, pubKeyMasternode))
            return "Error upon calling SetKey";

        CMasternode* pmn = mnodeman.Find(activeMasternode.vin);
        if (pmn == NULL) {
            return "Failure to find masternode in list : " + activeMasternode.vin.ToString();
        }

        CFinalizedBudgetVote vote(activeMasternode.vin, hash);
        if (!vote.Sign(keyMasternode, pubKeyMasternode)) {
            return "Failure to sign.";
        }

        std::string strError = "";
        if (budget.UpdateFinalizedBudget(vote, NULL, strError)) {
            budget.mapSeenFinalizedBudgetVotes.insert(make_pair(vote.GetHash(), vote));
            vote.Relay();
            return "success";
        } else {
            return "Error voting : " + strError;
        }
    }

    if (strCommand == "show") {
        Object resultObj;

        std::vector<CFinalizedBudget*> winningFbs = budget.GetFinalizedBudgets();
        BOOST_FOREACH (CFinalizedBudget* finalizedBudget, winningFbs) {
            Object bObj;
            bObj.push_back(Pair("FeeTX", finalizedBudget->nFeeTXHash.ToString()));
            bObj.push_back(Pair("Hash", finalizedBudget->GetHash().ToString()));
            bObj.push_back(Pair("BlockStart", (int64_t)finalizedBudget->GetBlockStart()));
            bObj.push_back(Pair("BlockEnd", (int64_t)finalizedBudget->GetBlockEnd()));
            bObj.push_back(Pair("Proposals", finalizedBudget->GetProposals()));
            bObj.push_back(Pair("VoteCount", (int64_t)finalizedBudget->GetVoteCount()));
            bObj.push_back(Pair("Status", finalizedBudget->GetStatus()));

            std::string strError = "";
            bObj.push_back(Pair("IsValid", finalizedBudget->IsValid(strError)));
            bObj.push_back(Pair("IsValidReason", strError.c_str()));

            resultObj.push_back(Pair(finalizedBudget->GetName(), bObj));
        }

        return resultObj;
    }

    if (strCommand == "getvotes") {
        if (params.size() != 2)
            throw runtime_error("Correct usage is 'mnbudget getvotes budget-hash'");

        std::string strHash = params[1].get_str();
        uint256 hash(strHash);

        Object obj;

        CFinalizedBudget* pfinalBudget = budget.FindFinalizedBudget(hash);

        if (pfinalBudget == NULL) return "Unknown budget hash";

        std::map<uint256, CFinalizedBudgetVote>::iterator it = pfinalBudget->mapVotes.begin();
        while (it != pfinalBudget->mapVotes.end()) {
            Object bObj;
            bObj.push_back(Pair("nHash", (*it).first.ToString().c_str()));
            bObj.push_back(Pair("nTime", (int64_t)(*it).second.nTime));
            bObj.push_back(Pair("fValid", (*it).second.fValid));

            obj.push_back(Pair((*it).second.vin.prevout.ToStringShort(), bObj));

            it++;
        }


        return obj;
    }


    return Value::null;
}
