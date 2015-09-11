// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"
#include "key.h"
#include "txdb.h"
#include "base58.h"

#include <stdint.h>

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double SIGCHECK_VERIFICATION_FACTOR = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64_t nTimeLastCheckpoint;
        int64_t nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    bool fEnabled = true;

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (      0, uint256("0x00000c257b93a36e9a4318a64398d661866341331a984e2b486414fc5bb16ccd"))
        (  41056, uint256("0x000000001f12305bf0443551030d9f18c5d7b1a6b7eb8e899b1b26fc45924ade"))
        (  81847, uint256("0x00000000c164428877cd4d46e2facc881b6b0a803e44a02c1f3b279ae7d58c32"))
        ( 308484, uint256("0x000000016bd2ef95ae4a456c6114cd7736a4219de5b75b2139c840650144e143"))
        ( 380481, uint256("0x00000003064d1fdbe86f35bfce8c54f88a80ef773e820ca86ae820ed6c4defcc"))
        ( 404998, uint256("0x000000004a815d04f437dd83d84866a8a07865f5b47030668a8096df0615361f"))
        ( 411932, uint256("0x000000001f3c7ec7251ebc1670fb3f772b42e25356fa02468c02c89199617cd5"))
        ( 423094, uint256("0x0000000007001e561197a35026b7c9bbaf0b9a1c918a41d9e7d638e44459f116"))
        ( 443157, uint256("0x000000000b103e119485969439ab2203b5578be3fb8b3aab512ebebaca1bce81"))
        ( 458433, uint256("0x000000000318a428560180bb8166321a6b20ae78fc0a9b3c560d30476859b2b5"))
        ( 464836, uint256("0x00000000079e9a16f173bf610f2ceddc5659aa7e9df2366dea01e346c37f9692"))
        ( 467282, uint256("0x0000000004a17401913be0aa29af7ace3335d58a846938d4fee0c749e4828d1d"))
        ( 473033, uint256("0x000000000515c71eb7c3de0574d5f6c632d8de9053c626aba22ae3a9eff67e9c"))
        ( 538178, uint256("0x000000000a13e56dc5d7962d4e3a852ff24055aa15096085d8173faf95172f4d"))
        ( 621138, uint256("0x0000000016a7d31cabbc6257c53d3b58f82f1a897d79066dabcb5ce5b031f8ca"))
        ( 714001, uint256("0x000000001d2b41db149991d5e01aee448042de6ac94e12c5ae6299e4fb129f5a"))
        ( 797370, uint256("0x000000001b24a2f70ce1e50c19d5f3dd77fbd6e0f0a3eb61b95ceaafb8435636"))
        ( 895901, uint256("0x0000000016db7c64fb4bb6475fbb06dca656d32b7864a2d045612660106d411c"))
        ( 972235, uint256("0x000000004e92bead093b946351cd2e7125d23e36042687497561db00a77b6ae8"))
        (1161321, uint256("0x000000001f0e7c685ceaf5c5b04b1ebed2a1d8d588715ef6f6877af0015a6f2c"))
        (1620970, uint256("0x0000000091dd05190ee8abb2fd4a946938ccd6401d16406e417546e19d2165ab"))
        (2046709, uint256("0x000000007f6f146dbe428b0e277847889921d362844d52ade87049839711e026"))
        (2356006, uint256("0x00000000f6b58b63543b61032c677fa133e6d6783b2096c587068a233377e91a"))
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1441907737, // * UNIX timestamp of last checkpoint block
        3006269,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        2880.0      // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        ( 0, uint256("0x00000e5e37c42d6b67d0934399adfb0fa48b59138abb1a8842c88f4ca3d4ec96"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1373481000,
        0,
        2880.0
    };

    static MapCheckpoints mapCheckpointsRegtest =
        boost::assign::map_list_of
        ( 0, uint256("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"))
        ;
    static const CCheckpointData dataRegtest = {
        &mapCheckpointsRegtest,
        0,
        0,
        0
    };

    const CCheckpointData &Checkpoints() {
        if (Params().NetworkID() == CChainParams::TESTNET)
            return dataTestnet;
        else if (Params().NetworkID() == CChainParams::MAIN)
            return data;
        else
            return dataRegtest;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (!fEnabled)
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex, bool fSigchecks) {
        if (pindex==NULL)
            return 0.0;

        int64_t nNow = time(NULL);

        double fSigcheckVerificationFactor = fSigchecks ? SIGCHECK_VERIFICATION_FACTOR : 1.0;
        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkpoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!fEnabled)
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!fEnabled)
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
	
    uint256 GetLatestHardenedCheckpoint()
    {
        LogPrintf("GetLatestHardenedCheckpoint\n");
        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;
        
        return (checkpoints.rbegin()->second);
    }
 
    // ppcoin: synchronized checkpoint (centrally broadcasted)
    uint256 hashSyncCheckpoint = 0;
    uint256 hashPendingCheckpoint = 0;
    CSyncCheckpoint checkpointMessage;
    CSyncCheckpoint checkpointMessagePending;
    uint256 hashInvalidCheckpoint = 0;
    CCriticalSection cs_hashSyncCheckpoint;
    std::string strCheckpointWarning;   
	
    // ppcoin: only descendant of current sync-checkpoint is allowed
    bool ValidateSyncCheckpoint(uint256 hashCheckpoint)
    {
        LogPrintf("ValidateSyncCheckpoint: hashCheckpoint=%s\n", hashCheckpoint.ToString().c_str());
        
        if ( (hashSyncCheckpoint == 0) || (!mapBlockIndex.count(hashSyncCheckpoint)) )
        {
            // NO SYNC CHECKPOINT
            return true;
        }
        
        if (!mapBlockIndex.count(hashSyncCheckpoint))
            return error("ValidateSyncCheckpoint: block index missing for current sync-checkpoint %s", hashSyncCheckpoint.ToString().c_str());
        if (!mapBlockIndex.count(hashCheckpoint))
            return error("ValidateSyncCheckpoint: block index missing for received sync-checkpoint %s", hashCheckpoint.ToString().c_str());
        
        CBlockIndex* pindexSyncCheckpoint = mapBlockIndex[hashSyncCheckpoint];
        CBlockIndex* pindexCheckpointRecv = mapBlockIndex[hashCheckpoint];

        if (pindexCheckpointRecv->nHeight <= pindexSyncCheckpoint->nHeight)
        {
            // Received an older checkpoint, trace back from current checkpoint
            // to the same height of the received checkpoint to verify
            // that current checkpoint should be a descendant block
            CBlockIndex* pindex = pindexSyncCheckpoint;
            while (pindex->nHeight > pindexCheckpointRecv->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("ValidateSyncCheckpoint: pprev1 null - block index structure failure");
            if (pindex->GetBlockHash() != hashCheckpoint)
            {
                hashInvalidCheckpoint = hashCheckpoint;
                return error("ValidateSyncCheckpoint: new sync-checkpoint %s is conflicting with current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
            }
            return false; // ignore older checkpoint
        }

        // Received checkpoint should be a descendant block of the current
        // checkpoint. Trace back to the same height of current checkpoint
        // to verify.
        CBlockIndex* pindex = pindexCheckpointRecv;
        while (pindex->nHeight > pindexSyncCheckpoint->nHeight)
            if (!(pindex = pindex->pprev))
                return error("ValidateSyncCheckpoint: pprev2 null - block index structure failure");
        if (pindex->GetBlockHash() != hashSyncCheckpoint)
        {
            hashInvalidCheckpoint = hashCheckpoint;
            return error("ValidateSyncCheckpoint: new sync-checkpoint %s is not a descendant of current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
        }
        
        LogPrintf("ValidateSyncCheckpoint: OK\n");
        return true;
    }
	
    bool WriteSyncCheckpoint(const uint256& hashCheckpoint)
    {
        if (!pblocktree->WriteSyncCheckpoint(hashCheckpoint))
        {
            return error("WriteSyncCheckpoint(): failed to write to txdb sync checkpoint %s", hashCheckpoint.ToString().c_str());
        }

        hashSyncCheckpoint = hashCheckpoint;
        return true;
    }

    bool IsSyncCheckpointEnforced()
    {
        return (GetBoolArg("-checkpointenforce", true) || mapArgs.count("-checkpointkey")); // checkpoint master node is always enforced
    }

    bool AcceptPendingSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint != 0 && mapBlockIndex.count(hashPendingCheckpoint))
        {
            if (!ValidateSyncCheckpoint(hashPendingCheckpoint))
            {
                hashPendingCheckpoint = 0;
                checkpointMessagePending.SetNull();
                LogPrintf("AcceptPendingSyncCheckpoint: FAIL1\n");
                return false;
            }

            CBlockIndex* pindexCheckpoint = mapBlockIndex[hashPendingCheckpoint];
            if (IsSyncCheckpointEnforced() && !pindexCheckpoint->IsInMainChain())
            {
                CBlock block;
                if (!ReadBlockFromDisk(block, pindexCheckpoint))
                    return error("AcceptPendingSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
                CValidationState state;
                LogPrintf("AcceptPendingSyncCheckpoint: ConnectTip\n");
                // if (!SetBestChain(state, pindexCheckpoint))
                if (!ConnectTip(state, pindexCheckpoint))
                {
                    hashInvalidCheckpoint = hashPendingCheckpoint;
                    return error("AcceptPendingSyncCheckpoint: SetBestChain failed for sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
                }
            }

            if (!WriteSyncCheckpoint(hashPendingCheckpoint))
                return error("AcceptPendingSyncCheckpoint(): failed to write sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
            hashPendingCheckpoint = 0;
            checkpointMessage = checkpointMessagePending;
            checkpointMessagePending.SetNull();
            LogPrintf("AcceptPendingSyncCheckpoint : sync-checkpoint at %s\n", hashSyncCheckpoint.ToString().c_str());
            // relay the checkpoint
            if (!checkpointMessage.IsNull())
            {
                BOOST_FOREACH(CNode* pnode, vNodes)
                    checkpointMessage.RelayTo(pnode);
            }
            return true;
        }
        return false;
    }

    // Automatically select a suitable sync-checkpoint 
    uint256 AutoSelectSyncCheckpoint()
    {
        // Search backward for a block with specified depth policy
        // const CBlockIndex *pindex = pindexBest;
        // while (pindex->pprev && pindex->nHeight + (int)GetArg("-checkpointdepth", -1) > pindexBest->nHeight)
        const CBlockIndex *pindex = chainActive.Tip();
        while (pindex->pprev && pindex->nHeight + (int)GetArg("-checkpointdepth", -1) > chainActive.Tip()->nHeight)
            pindex = pindex->pprev;
        return pindex->GetBlockHash();
    }

    // Check against synchronized checkpoint
    bool CheckSyncCheckpoint(const uint256& hashBlock, const CBlockIndex* pindexPrev)
    {
        int nHeight = pindexPrev->nHeight + 1;
        LogPrintf("CheckSyncCheckpoint: nHeight=%d, hashSyncCheckpoint=%s\n", nHeight, hashSyncCheckpoint.ToString().c_str());

        LOCK(cs_hashSyncCheckpoint);
        
        if ((hashSyncCheckpoint == 0) || (mapBlockIndex.count(hashSyncCheckpoint) == 0))
            return true;
            
        // sync-checkpoint should always be accepted block
        // assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];

        if (nHeight > pindexSync->nHeight)
        {
            // trace back to same height as sync-checkpoint
            const CBlockIndex* pindex = pindexPrev;
            while (pindex->nHeight > pindexSync->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("CheckSyncCheckpoint: pprev null - block index structure failure");
            if (pindex->nHeight < pindexSync->nHeight || pindex->GetBlockHash() != hashSyncCheckpoint)
                return false; // only descendant of sync-checkpoint can pass check
        }
        if (nHeight == pindexSync->nHeight && hashBlock != hashSyncCheckpoint)
            return false; // same height with sync-checkpoint
        if (nHeight < pindexSync->nHeight && !mapBlockIndex.count(hashBlock))
            return false; // lower height than sync-checkpoint
        return true;
    }
    
    bool WantedByPendingSyncCheckpoint(uint256 hashBlock)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint == 0)
            return false;
        if (hashBlock == hashPendingCheckpoint)
            return true;
        if (mapOrphanBlocksSyncCheckpoint.count(hashPendingCheckpoint)
            && hashBlock == WantedByOrphan(mapOrphanBlocksSyncCheckpoint[hashPendingCheckpoint]))
            return true;
        return false;
    }

    // ppcoin: reset synchronized checkpoint to last hardened checkpoint
    bool ResetSyncCheckpoint()
    {
        LogPrintf("ResetSyncCheckpoint\n");
        LOCK(cs_hashSyncCheckpoint);
        const uint256& hash = Checkpoints::GetLatestHardenedCheckpoint();
        if (mapBlockIndex.count(hash) && !mapBlockIndex[hash]->IsInMainChain())
        {
            // checkpoint block accepted but not yet in main chain
            LogPrintf("ResetSyncCheckpoint: SetBestChain to hardened checkpoint %s\n", hash.ToString().c_str());
            
            CBlock block;
            if (!ReadBlockFromDisk(block, mapBlockIndex[hash]))
                return error("ResetSyncCheckpoint: ReadFromDisk failed for hardened checkpoint %s", hash.ToString().c_str());
            CValidationState state;
            LogPrintf("ResetSyncCheckpoint: ConnectTip\n");
            // if (!SetBestChain(state, mapBlockIndex[hash]))
            if (!ConnectTip(state, mapBlockIndex[hash]))
            {
                return error("ResetSyncCheckpoint: SetBestChain failed for hardened checkpoint %s", hash.ToString().c_str());
            }
        }
        else if(!mapBlockIndex.count(hash))
        {
            // checkpoint block not yet accepted
            hashPendingCheckpoint = hash;
            checkpointMessagePending.SetNull();
            LogPrintf("ResetSyncCheckpoint: pending for sync-checkpoint %s\n", hashPendingCheckpoint.ToString().c_str());
        }

        if (!WriteSyncCheckpoint((mapBlockIndex.count(hash) && mapBlockIndex[hash]->IsInMainChain())? hash : Params().HashGenesisBlock()))
            return error("ResetSyncCheckpoint: failed to write sync checkpoint %s", hash.ToString().c_str());
        LogPrintf("ResetSyncCheckpoint: sync-checkpoint reset to %s\n", hashSyncCheckpoint.ToString().c_str());
        return true;
    }

    void AskForPendingSyncCheckpoint(CNode* pfrom)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (pfrom && hashPendingCheckpoint != 0 && (!mapBlockIndex.count(hashPendingCheckpoint)) && (!mapOrphanBlocksSyncCheckpoint.count(hashPendingCheckpoint)))
            pfrom->AskFor(CInv(MSG_BLOCK, hashPendingCheckpoint));
    }
    
    // Verify sync checkpoint master pubkey and reset sync checkpoint if changed
    bool CheckCheckpointPubKey()
    {
        std::string strPubKey = "";
        std::string strMasterPubKey = TestNet() ? CSyncCheckpoint::strTestPubKey : CSyncCheckpoint::strMainPubKey;
        if (!pblocktree->ReadCheckpointPubKey(strPubKey) || strPubKey != strMasterPubKey)
        {
            // write checkpoint master key to db
            if (!pblocktree->WriteCheckpointPubKey(strMasterPubKey))
                return error("CheckCheckpointPubKey() : failed to write new checkpoint master key to db");
            if (!ResetSyncCheckpoint())
                return error("CheckCheckpointPubKey() : failed to reset sync-checkpoint");
        }
        return true;
    }

    bool SetCheckpointPrivKey(std::string strPrivKey)
    {
        // Test signing a sync-checkpoint with genesis block
        CSyncCheckpoint checkpoint;
        checkpoint.hashCheckpoint = Params().HashGenesisBlock();
        CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
        sMsg << (CUnsignedSyncCheckpoint)checkpoint;
        checkpoint.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());

        CBitcoinSecret vchSecret;
        if (!vchSecret.SetString(strPrivKey))
            return error("SendSyncCheckpoint: Checkpoint master key invalid");
        // bool fCompressed;
        // CSecret secret = vchSecret.GetSecret(fCompressed);
        CKey secret = vchSecret.GetKey();
        // key.SetSecret(secret, fCompressed); // if key is not correct openssl may crash
        CKey key(secret);
        if (!key.Sign(Hash(checkpoint.vchMsg.begin(), checkpoint.vchMsg.end()), checkpoint.vchSig))
            return false;

        // Test signing successful, proceed
        CSyncCheckpoint::strMasterPrivKey = strPrivKey;
        return true;
    }

    bool SendSyncCheckpoint(uint256 hashCheckpoint)
    {
        LogPrintf("SendSyncCheckpoint: hashCheckpoint=%s\n", hashCheckpoint.ToString().c_str());
        
        CSyncCheckpoint checkpoint;
        checkpoint.hashCheckpoint = hashCheckpoint;
        CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
        sMsg << (CUnsignedSyncCheckpoint)checkpoint;
        checkpoint.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());

        if (CSyncCheckpoint::strMasterPrivKey.empty())
            return error("SendSyncCheckpoint: Checkpoint master key unavailable.");
        CBitcoinSecret vchSecret;
        if (!vchSecret.SetString(CSyncCheckpoint::strMasterPrivKey))
            return error("SendSyncCheckpoint: Checkpoint master key invalid");
        // bool fCompressed;
        // CSecret secret = vchSecret.GetSecret(fCompressed);
        CKey secret = vchSecret.GetKey();
        // key.SetSecret(secret, fCompressed); // if key is not correct openssl may crash
        CKey key(secret);
        if (!key.Sign(Hash(checkpoint.vchMsg.begin(), checkpoint.vchMsg.end()), checkpoint.vchSig))
            return error("SendSyncCheckpoint: Unable to sign checkpoint, check private key?");

        if(!checkpoint.ProcessSyncCheckpoint(NULL))
        {
            LogPrintf("WARNING: SendSyncCheckpoint: Failed to process checkpoint.\n");
            return false;
        }

        // Relay checkpoint
        {
            LOCK(cs_vNodes);
            BOOST_FOREACH(CNode* pnode, vNodes)
                checkpoint.RelayTo(pnode);
        }
        return true;
    }

    // Is the sync-checkpoint outside maturity window?
    bool IsMatureSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];
        // return (nBestHeight >= pindexSync->nHeight + COINBASE_MATURITY);
        return (chainActive.Tip()->nHeight >= pindexSync->nHeight + COINBASE_MATURITY);
    }

    // Is the sync-checkpoint too old?
    bool IsSyncCheckpointTooOld(unsigned int nSeconds)
    {
        LOCK(cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];
        return (pindexSync->GetBlockTime() + nSeconds < GetAdjustedTime());
    }

    // ppcoin: find block wanted by given orphan block
    uint256 WantedByOrphan(const CBlock* pblockOrphan)
    {
        // Work back to the first block in the orphan chain
        while (mapOrphanBlocksSyncCheckpoint.count(pblockOrphan->hashPrevBlock))
            pblockOrphan = mapOrphanBlocksSyncCheckpoint[pblockOrphan->hashPrevBlock];
        return pblockOrphan->hashPrevBlock;
    }

}

// ppcoin: sync-checkpoint master key
const std::string CSyncCheckpoint::strMainPubKey = "0466aa7cf205be5c40f114c80d0d4087959508ace5642c9b849af1ba78d7c6b969f3e8d36b3d44e5a0ac1d2d8f3e6f7452055713943870700385544c2a04c5aa55";
const std::string CSyncCheckpoint::strTestPubKey = "041ba70a9e3afd1c0c13b7577e4f71ede2eee884df617fa28bfb0ee3fe993b9cc2835c16b794e46095bf425c4e2cdc2e628becdb196f0302840282d3d32d6c69bd";
std::string CSyncCheckpoint::strMasterPrivKey = "";

// ppcoin: verify signature of sync-checkpoint message
bool CSyncCheckpoint::CheckSignature()
{
    std::string strMasterPubKey = TestNet() ? CSyncCheckpoint::strTestPubKey : CSyncCheckpoint::strMainPubKey;
//    if (!key.Set(ParseHex(strMasterPubKey)))
//        return error("CSyncCheckpoint::CheckSignature() : SetPubKey failed");
    CPubKey key(ParseHex(strMasterPubKey));
    if (!key.Verify(Hash(vchMsg.begin(), vchMsg.end()), vchSig))
        return error("CSyncCheckpoint::CheckSignature() : verify signature failed");

    // Now unserialize the data
    CDataStream sMsg(vchMsg, SER_NETWORK, PROTOCOL_VERSION);
    sMsg >> *(CUnsignedSyncCheckpoint*)this;
    return true;
}

// ppcoin: process synchronized checkpoint
bool CSyncCheckpoint::ProcessSyncCheckpoint(CNode* pfrom)
{   
    if (!CheckSignature())
        return false;

    LOCK(Checkpoints::cs_hashSyncCheckpoint);
    if (!mapBlockIndex.count(hashCheckpoint))
    {
        // We haven't received the checkpoint chain, keep the checkpoint as pending
        Checkpoints::hashPendingCheckpoint = hashCheckpoint;
        Checkpoints::checkpointMessagePending = *this;
        LogPrintf("ProcessSyncCheckpoint: pending for sync-checkpoint %s\n", hashCheckpoint.ToString().c_str());
        // Ask this guy to fill in what we're missing
        if (pfrom)
        {
            // pfrom->PushGetBlocks(pindexBest, hashCheckpoint);
			PushGetBlocks(pfrom, chainActive.Tip(), hashCheckpoint);
            // ask directly as well in case rejected earlier by duplicate
            // proof-of-stake because getblocks may not get it this time
            pfrom->AskFor(CInv(MSG_BLOCK, mapOrphanBlocksSyncCheckpoint.count(hashCheckpoint)? Checkpoints::WantedByOrphan(mapOrphanBlocksSyncCheckpoint[hashCheckpoint]) : hashCheckpoint));
        }
        return false;
    }

    if (!Checkpoints::ValidateSyncCheckpoint(hashCheckpoint))
        return false;

    CBlockIndex* pindexCheckpoint = mapBlockIndex[hashCheckpoint];
    if (!pindexCheckpoint->IsInMainChain())
    {
        // checkpoint chain received but not yet main chain
        CBlock block;
        if (!ReadBlockFromDisk(block, pindexCheckpoint))
            return error("ProcessSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());
        CValidationState state;
        LogPrintf("ProcessSyncCheckpoint: ConnectTip\n");
        // if (!SetBestChain(state, pindexCheckpoint))
        if (!ConnectTip(state, pindexCheckpoint))
        {
            Checkpoints::hashInvalidCheckpoint = hashCheckpoint;
            return error("ProcessSyncCheckpoint: SetBestChain failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());
        }
    }

    if (!Checkpoints::WriteSyncCheckpoint(hashCheckpoint))
        return error("ProcessSyncCheckpoint(): failed to write sync checkpoint %s", hashCheckpoint.ToString().c_str());
    Checkpoints::checkpointMessage = *this;
    Checkpoints::hashPendingCheckpoint = 0;
    Checkpoints::checkpointMessagePending.SetNull();
    
    LogPrintf("ProcessSyncCheckpoint: sync-checkpoint at %s\n", hashCheckpoint.ToString().c_str());
    return true;
}
