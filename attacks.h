
#ifndef ATTACKS_H
#define ATTACKS_H
#include "types.h"
constexpr U64 rankMask(int r) {
 return 0xFFULL << (8*r);
}
constexpr U64 fileMask(int f) {
 return 0x0101010101010101ULL << f;
}
constexpr U64 shiftSouth(U64 b) {return b >> 8;}
constexpr U64 shiftNorth(U64 b) {return b << 8;}

inline U64 pawnsAble2Push(U64 pawns, U64 empty,int color) {
 if (color==WHITE)
  return shiftSouth(empty) & pawns;
 return shiftNorth(empty) & pawns;
}

inline U64 pawnsAble2DblPush(U64 pawns, U64 empty,int color) {
 if (color==WHITE) {
  const U64 rank4 = rankMask(3);
  U64 emptyRank3 = shiftSouth(empty & rank4) & empty;
  return pawnsAble2Push(pawns, emptyRank3, color);
 }
 const U64 rank5 = rankMask(4);
 U64 emptyRank6 = shiftNorth(empty & rank5) & empty;
 return pawnsAble2Push(pawns, emptyRank6, color);
}
void initAttacks();
inline U64 pawnAttacks[2][64];
inline U64 kingAttacks[64];
inline U64 knightAttacks[64];

inline U64 LineBB[64][64];

inline int rankOf(int sq) {
    return sq >> 3;
}

inline int fileOf(int sq) {
    return sq & 7;
}

inline void precomputeLineBB() {
    U64 rankMasks[8];
    U64 fileMasks[8];
    U64 diagMasks[15];
    U64 antiDiagMasks[15];

    for(int i = 0; i < 8; ++i) {
        rankMasks[i] = 0;
        fileMasks[i] = 0;
    }
    for(int i = 0; i < 15; ++i) {
        diagMasks[i] = 0;
        antiDiagMasks[i] = 0;
    }

    for (int sq = 0; sq < 64; ++sq) {
        int r = rankOf(sq);
        int f = fileOf(sq);
        U64 sq_bb = 1ULL << sq;

        rankMasks[r] |= sq_bb;
        fileMasks[f] |= sq_bb;
        diagMasks[r - f + 7] |= sq_bb;
        antiDiagMasks[r + f] |= sq_bb;
    }

    for (int s1 = 0; s1 < 64; ++s1) {
        for (int s2 = 0; s2 < 64; ++s2) {
            LineBB[s1][s2] = 0;

            if (s1 == s2) continue;

            int r1 = rankOf(s1), f1 = fileOf(s1);
            int r2 = rankOf(s2), f2 = fileOf(s2);

            if (r1 == r2) {
                LineBB[s1][s2] = rankMasks[r1];
            } else if (f1 == f2) {
                LineBB[s1][s2] = fileMasks[f1];
            } else if (r1 - f1 == r2 - f2) {
                LineBB[s1][s2] = diagMasks[r1 - f1 + 7];
            } else if (r1 + f1 == r2 + f2) {
                LineBB[s1][s2] = antiDiagMasks[r1 + f1];
            }
        }
    }
}
#endif //ATTACKS_H
