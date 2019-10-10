// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

static const unsigned int timeMainGenesisBlock = 1374408079;
uint256 hashMainGenesisBlock("0x00000c257b93a36e9a4318a64398d661866341331a984e2b486414fc5bb16ccd");
static uint256 nMainProofOfWorkLimit(~uint256(0) >> 20);

static const int64_t nGenesisBlockRewardCoin = 1 * COIN;
static const int64_t nBlockRewardStartCoin = 2048 * COIN;
static const int64_t nBlockRewardMinimumCoin = 1 * COIN;

/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */
static Checkpoints::MapCheckpoints mapCheckpoints =
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
        (2434772, uint256("0x0000000017564063a7ceec746f5cfb95d8f7803b1309a505a617d82ba2d3abbd"))
        (2437394, uint256("0x00000000dfcafbaa8c160fd07ab80e44660ceccd12d760b8b5fe6ab1aeec52d6"))
        (3235865, uint256("0x000000010f18f93a614a12dfe936319f52aa447f752836f4fa896310741dd32b"))
        (4101607, uint256("0x00000000228a00758187ef66622ca31893c2614a053e2e46efd955222d7fdf57"))
        (4223071, uint256("0x000000010f385d545029761683a3063c554e5f127a44e1cda99c9909ea4ff549"))
        (4256000, uint256("0x0000000024cfefc26f5c55f04e78a27816081b29b3fd0cb4ff812032bf95befd"))
        (7080827, uint256("0x000000000051756ae28645408c5220cb31ef78435c9f379ac49acb5156cac000"))
        ;
static const Checkpoints::CCheckpointData data = {
        &mapCheckpoints,
        1564249020, // * UNIX timestamp of last checkpoint block
        7990566,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        2880.0      // * estimated number of transactions per day after checkpoint
    };

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        ( 0, uint256("0x00000e5e37c42d6b67d0934399adfb0fa48b59138abb1a8842c88f4ca3d4ec96"))
        ;
static const Checkpoints::CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1373481000,
        0,
        2880
    };

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
        boost::assign::map_list_of
        ( 0, uint256("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"))
        ;
static const Checkpoints::CCheckpointData dataRegtest = {
        &mapCheckpointsRegtest,
        0,
        0,
        0
    };

class CMainParams : public CChainParams {
public:
    CMainParams() {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        //pchMessageStart[0] = 0xfe;
        //pchMessageStart[1] = 0xa5;
        //pchMessageStart[2] = 0x03;
        //pchMessageStart[3] = 0xdd;
        pchMessageStart[0] = 0xfc;
        pchMessageStart[1] = 0xa4;
        pchMessageStart[2] = 0x02;
        pchMessageStart[3] = 0xdc;

        vAlertPubKey = ParseHex("0493e6dc310a0e444cfb20f3234a238f77699806d47909a42481010c5ce68ff04d3babc959cd037bd3aa6ded929f2b9b4aa2f626786cd7f8495e5bb61e9cfebbc4");
        nDefaultPort = 13721;
        bnProofOfWorkLimit = nMainProofOfWorkLimit;
        nSubsidyHalvingInterval = 60480;
        nEnforceBlockUpgradeMajority = 7500;
        nRejectBlockOutdatedMajority = 9000;
        nToCheckBlockUpgradeMajority = 10000;
        nMinerThreads = 0;
        nTargetTimespan = 10 * 60; // ten minutes
        nTargetSpacing = 30; // 30 seconds
        nMaxTipAge = 24 * 60 * 60;
        nModifierUpdateBlock = 615800;
        nFirstPOSBlock = 4937000; // est 25 Nov 2017
        nFirstMasternodePaymentBlock = 7070000;
        nInflationCycleIntervalBlocks = 2*60*24*365;// approximately blocks in a inflation cycle (one year)
        nTreasuryPaymentIntervalBlocks = 2*60*24*30;// approximately treasury peyment interval one month
        nCoinbaseMaturity = 100;

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         *
         * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
         *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
         *   vMerkleTree: 4a5e1e
         */
        const char* pszTimestamp = "21 July 2013, The Guardian, Tesco boss says cheap food era is over";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = nGenesisBlockRewardCoin;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 112;
        genesis.nTime    = timeMainGenesisBlock;
        genesis.nBits    = nMainProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 12058113;

        assert(genesis.hashMerkleRoot == uint256("0x868b2fb28cb1a0b881480cc85eb207e29e6ae75cdd6d26688ed34c2d2d23c776"));

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == hashMainGenesisBlock);

        vSeeds.push_back(CDNSSeedData("seed1.qrknet.info", "seed1.qrknet.info"));
        vSeeds.push_back(CDNSSeedData("seed2.qrknet.info", "seed2.qrknet.info"));
        vSeeds.push_back(CDNSSeedData("seed3.qrknet.info", "seed3.qrknet.info"));
        vSeeds.push_back(CDNSSeedData("seed4.qrknet.info", "seed4.qrknet.info"));
        vSeeds.push_back(CDNSSeedData("seed5.qrknet.info", "seed5.qrknet.info"));
        vSeeds.push_back(CDNSSeedData("seed6.qrknet.info", "seed6.qrknet.info"));
        vSeeds.push_back(CDNSSeedData("seed7.qrknet.info", "seed7.qrknet.info"));
        vSeeds.push_back(CDNSSeedData("seed8.qrknet.info", "seed8.qrknet.info"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(58);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(9);
        base58Prefixes[SECRET_KEY] =     list_of(186);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4);

        //convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        nPoolMaxTransactions = 3;
        nStakeMinAge = 30 * 24 * 60 * 60;//about 30 days

        strSporkKey = "0499A7AF4806FC6DE640D23BC5936C29B77ADF2174B4F45492727F897AE63CF8D27B2F05040606E0D14B547916379FA10716E344E745F880EDC037307186AA25B7";
        strSporkKeyOld = "04B433E6598390C992F4F022F20D3B4CBBE691652EE7C48243B81701CBDB7CC7D7BF0EE09E154E6FCBF2043D65AF4E9E97B89B5DBAF830D83B9B7F469A6C45A717";
        nEnforceNewSporkKey = 1525158000; //!> Sporks signed after (GMT): Tuesday, May 1, 2018 7:00:00 AM GMT must use the new spork key
        nRejectOldSporkKey = 1527811200; //!> Fully reject old spork key after (GMT): Friday, June 1, 2018 12:00:00 AM

        //strObfuscationPoolDummyAddress = "y57cqfGRkekRyDRNeJiLtYVEbvhXrNbmox";
        strTreasuryPaymentAddress = "QVvYypXoiRXcJpJpRVkftKEjMkXbd6UKJv";
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        //pchMessageStart[0] = 0x01;
        //pchMessageStart[1] = 0x1a;
        //pchMessageStart[2] = 0x39;
        //pchMessageStart[3] = 0xf7;
        pchMessageStart[0] = 0x02;
        pchMessageStart[1] = 0x1b;
        pchMessageStart[2] = 0x3a;
        pchMessageStart[3] = 0xf8;
        vAlertPubKey = ParseHex("04218bc3f08237baa077cb1b0e5a81695fcf3f5b4e220b4ad274d05a31d762dd4e191efa7b736a24a32d6fd9ac1b5ebb2787c70e9dfad0016a8b32f7bd2520dbd5");
        nDefaultPort = 23721;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 10 * 60; // ten minutes
        nTargetSpacing = 30; // 30 seconds
        nMaxTipAge = 0x7fffffff;
        nModifierUpdateBlock = 51197; //approx Mon, 17 Apr 2017 04:00:00 GMT
        nFirstPOSBlock = 20;
        nFirstMasternodePaymentBlock = 30;
        nInflationCycleIntervalBlocks = 12*30;// approximately blocks in a inflation cycle (120 minutes)
        nTreasuryPaymentIntervalBlocks = 30; //approximately 10 minutes
        nCoinbaseMaturity = 2;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1373481000;
        genesis.nNonce = 905523645;
        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000e5e37c42d6b67d0934399adfb0fa48b59138abb1a8842c88f4ca3d4ec96"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("testseed1.qrknet.info", "testseed1.qrknet.info"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(119);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(199);
        base58Prefixes[SECRET_KEY]     = list_of(247);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94);

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        nStakeMinAge = 30*60 ;// about 30 minutes

        strSporkKey = "0499A7AF4806FC6DE640D23BC5936C29B77ADF2174B4F45492727F897AE63CF8D27B2F05040606E0D14B547916379FA10716E344E745F880EDC037307186AA25B7";
        strSporkKeyOld = "04B433E6598390C992F4F022F20D3B4CBBE691652EE7C48243B81701CBDB7CC7D7BF0EE09E154E6FCBF2043D65AF4E9E97B89B5DBAF830D83B9B7F469A6C45A717";
        nEnforceNewSporkKey = 1525158000; //!> Sporks signed after (GMT): Tuesday, May 1, 2018 7:00:00 AM GMT must use the new spork key
        nRejectOldSporkKey = 1527811200; //!> Fully reject old spork key after (GMT): Friday, June 1, 2018 12:00:00 AM

        //strObfuscationPoolDummyAddress = "y57cqfGRkekRyDRNeJiLtYVEbvhXrNbmox";
        strTreasuryPaymentAddress = "q4GiYqMebegEkmzD9CxUiV4WmtSP1Kyo23";
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 10 * 60; // ten minutes
        nTargetSpacing = 30; // 30 seconds
        bnProofOfWorkLimit = ~uint256(0);
        nMaxTipAge = 24 * 60 * 60;
        genesis.nTime = 1296688602;
        genesis.nBits = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 3;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 18444;
        nFirstPOSBlock = 20;
        nFirstMasternodePaymentBlock = 30;
        nInflationCycleIntervalBlocks = 12*30;// approximately blocks in a inflation cycle (120 minutes)
        nTreasuryPaymentIntervalBlocks = 30; //approximately 10 minutes
        nCoinbaseMaturity = 2;

        nStakeMinAge = 60;// 10 minutes
        strTreasuryPaymentAddress = "q2MXaSB7XGz51ZYCeGHBAYDWVsTsKUgxut";
        // assert(hashGenesisBlock == uint256("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        nPoolMaxTransactions = 2;
        nStakeMinAge = 30*60 ;// about 30 minutes

        strSporkKey = "0499A7AF4806FC6DE640D23BC5936C29B77ADF2174B4F45492727F897AE63CF8D27B2F05040606E0D14B547916379FA10716E344E745F880EDC037307186AA25B7";
        strSporkKeyOld = "04B433E6598390C992F4F022F20D3B4CBBE691652EE7C48243B81701CBDB7CC7D7BF0EE09E154E6FCBF2043D65AF4E9E97B89B5DBAF830D83B9B7F469A6C45A717";
        nEnforceNewSporkKey = 1525158000; //!> Sporks signed after (GMT): Tuesday, May 1, 2018 7:00:00 AM GMT must use the new spork key
        nRejectOldSporkKey = 1527811200; //!> Fully reject old spork key after (GMT): Friday, June 1, 2018 12:00:00 AM

        //strObfuscationPoolDummyAddress = "y57cqfGRkekRyDRNeJiLtYVEbvhXrNbmox";
        strTreasuryPaymentAddress = "q4GiYqMebegEkmzD9CxUiV4WmtSP1Kyo23";
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams {
public:
    CUnitTestParams() {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 18445;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval)  { nSubsidyHalvingInterval=anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority)  { nEnforceBlockUpgradeMajority=anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority)  { nRejectBlockOutdatedMajority=anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority)  { nToCheckBlockUpgradeMajority=anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks)  { fDefaultConsistencyChecks=afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) {  fAllowMinDifficultyBlocks=afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams *pCurrentParams = 0;

CModifiableParams *ModifiableParams()
{
   assert(pCurrentParams);
   assert(pCurrentParams==&unitTestParams);
   return (CModifiableParams*)&unitTestParams;
}

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        case CBaseChainParams::UNITTEST:
            return unitTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
