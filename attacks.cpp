
#include "attacks.h"
#include "types.h"
void initAttacks() {
    precomputeLineBB();
    for (int sq=0;sq<64;sq++) {
        U64 nBB=0ULL;
        U64 wpBB=0ULL;
        U64 bpBB=0ULL;
        U64 kBB=0ULL;
        int rank=sq/8,file=sq%8;
        int knight[8][2]={
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
            {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
        };
        int king [8][2]={
            {1,0},{-1,0},{0,1},{0,-1},
            {1,1},{1,-1},{-1,1},{-1,-1}
        };
        for (auto& m:knight) {
            int r = rank + m[0], f = file + m[1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8)
                nBB |= 1ULL << (r * 8 + f);
        }
        for (auto& m:king) {
            int r = rank + m[0], f = file + m[1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8)
                kBB |= 1ULL << (r * 8 + f);
        }
        int wPawn[2][2]={
            {1,1},{1,-1}
        };
        int bPawn[2][2]={
            {-1,1},{-1,-1}
        };
        for (auto& m:wPawn) {
            int r = rank + m[0], f = file + m[1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8)
                wpBB |= 1ULL << (r * 8 + f);
        }
        for (auto& m:bPawn) {
            int r = rank + m[0], f = file + m[1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8)
                bpBB |= 1ULL << (r * 8 + f);
        }
        knightAttacks[sq] = nBB;
        pawnAttacks[WHITE][sq]=wpBB;
        pawnAttacks[BLACK][sq]=bpBB;
        kingAttacks[sq]=kBB;
    }
}