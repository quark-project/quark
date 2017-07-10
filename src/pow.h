// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include <stdint.h>

class CBlockHeader;
class CBlockIndex;
class uint256;

static const int64_t nTargetTimespan = 10 * 60; // 10 minutes
static const int64_t nTargetSpacing = 30; // 30 seconds
static const int64_t nInterval = nTargetTimespan / nTargetSpacing; // 20 blocks

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock);
unsigned int GetNextPoSTargetRequired(const CBlockIndex* pindexLast);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits);
uint256 GetBlockProof(const CBlockIndex& block);

#endif // BITCOIN_POW_H
