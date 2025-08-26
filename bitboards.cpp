//
// Created by MSi on 8/23/2025.
//

#include "bitboards.h"

#include <iostream>
#include <random>

#include "types.h"
#include "magic.h"
U64 attack_table[107648];// ~840 KiB all rook and bishop attacks, less with constructive collisions optimization
void initMagic() {
    U64* ptr = attack_table;
    for (int sq = 0; sq < 64; ++sq) {
        int bits = RBits[sq];
        const int tableSize = 1 << bits;
        U64 mask = RMask[sq];
        U64 magic = RMagic[sq];
        mRookTbl[sq].magic = magic;
        mRookTbl[sq].mask  = mask;
        mRookTbl[sq].ptr   = ptr;
        mRookTbl[sq].shift = 64 - bits;
        for (U64 s = mask; ; s = (s - 1) & mask) {
            U64 occ = s;
            int idx = (int)((occ * magic) >> (64 - bits));
            U64 att = rookAttackRay(sq, occ);
            ptr[idx] = att;
            if (s == 0) break;
        }
        ptr += tableSize;
    }
    for (int sq = 0; sq < 64; ++sq) {
        int bits = BBits[sq];
        const int tableSize = 1 << bits;
        U64 mask = BMask[sq];
        U64 magic = BMagic[sq];
        mBishopTbl[sq].magic = magic;
        mBishopTbl[sq].mask  = mask;
        mBishopTbl[sq].ptr   = ptr;
        mBishopTbl[sq].shift = 64 - bits;
        for (U64 s = mask; ; s = (s - 1) & mask) {
            U64 occ = s;
            int idx = (int)((occ * magic) >> (64 - bits));
            U64 att = bishopAttackRay(sq, occ);
            ptr[idx] = att;
            if (s == 0) break;
        }
        ptr += tableSize;
    }
}
void printBitboard(U64 bb) {
    for (int rank=7;rank>=0;rank--) {
        std::cout << rank+1 << "  ";
        for (int file=0;file<8;file++) {
            int sq=rank*8+file; //
            std::cout << ((bb>>sq)&1ULL? "1 " : ". ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << "  a b c d e f g h" << std::endl << std::endl;
}