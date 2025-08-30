//
// Created by MSi on 8/25/2025.
//

#ifndef EVALUATION_H
#define EVALUATION_H

constexpr int INF=1e7;
constexpr int MATE=32000;
constexpr int mg_value[6] = { 82, 337, 365, 477, 1025,  0};
constexpr int eg_value[6] = { 94, 281, 297, 512,  936,  0};
inline int file_of(int sq) { return sq & 7; }
inline int rank_of(int sq) { return sq >> 3; }

// Central areas
static constexpr U64 CENTER_4 =
    (1ULL<<27)|(1ULL<<28)|(1ULL<<35)|(1ULL<<36);
static constexpr U64 CENTER_16 =
    ((1ULL<<18)|(1ULL<<19)|(1ULL<<20)|(1ULL<<21)|
     (1ULL<<26)|(1ULL<<27)|(1ULL<<28)|(1ULL<<29)|
     (1ULL<<34)|(1ULL<<35)|(1ULL<<36)|(1ULL<<37)|
     (1ULL<<42)|(1ULL<<43)|(1ULL<<44)|(1ULL<<45));

// ---------- Tapered Evaluation Constants ----------
// Each feature now has a middlegame (MG) and endgame (EG) value.
constexpr int ISOLATED_PAWN_PENALTY[2] = {10, 20}; // MG, EG
constexpr int DOUBLED_PAWN_PENALTY[2]  = {15, 25}; // MG, EG
constexpr int PASSED_PAWN_BONUS[2]     = {20, 50}; // MG, EG
constexpr int KING_PAWN_SHIELD_BONUS[2]= {15, 5};  // MG, EG
constexpr int KING_ATTACKER_PENALTY[2] = {40, 10}; // MG, EG
constexpr int ROOK_OPEN_FILE_BONUS[2]  = {25, 15}; // MG, EG
constexpr int ROOK_SEMIOPEN_FILE_BONUS[2] = {15, 10}; // MG, EG
constexpr int BISHOP_PAIR_BONUS[2]     = {50, 60}; // MG, EG
constexpr int CENTER_CONTROL_BONUS[2]  = {8, 4};   // MG, EG

// Pre-computed masks
inline U64 allForwardRanksMask[2][64]; // [color][square]
inline U64 kingFrontPawnShield[64];
inline U64 kingBackPawnShield[64];


constexpr int mg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

constexpr int eg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr int mg_knight_table[64] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

constexpr int eg_knight_table[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

constexpr int mg_bishop_table[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

constexpr int eg_bishop_table[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

constexpr int mg_rook_table[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

constexpr int eg_rook_table[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

constexpr int mg_queen_table[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

constexpr int eg_queen_table[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

constexpr int mg_king_table[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

constexpr int eg_king_table[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};
constexpr const int* mg_pesto_table[6] =
{
    mg_pawn_table,
    mg_knight_table,
    mg_bishop_table,
    mg_rook_table,
    mg_queen_table,
    mg_king_table
};

constexpr const int* eg_pesto_table[6] =
{
    eg_pawn_table,
    eg_knight_table,
    eg_bishop_table,
    eg_rook_table,
    eg_queen_table,
    eg_king_table
};
constexpr int gamephaseInc[6] = {0,1,1,2,4,0};
inline int mg_table[2][6][64];
inline int eg_table[2][6][64];

inline void init_tables()
{
    for  (int p = PAWN;p <= KING; p++) {
        for (int sq = 0; sq < 64; sq++) {
            mg_table[WHITE][p]  [sq] = mg_value[p] + mg_pesto_table[p][sq];
            eg_table[WHITE][p]  [sq] = eg_value[p] + eg_pesto_table[p][sq];
            mg_table[BLACK][p][sq] = -(mg_value[p] + mg_pesto_table[p][sq^56]);
            eg_table[BLACK][p][sq] = -(eg_value[p] + eg_pesto_table[p][sq^56]);
        }
    }
}
inline int mg_score;
inline int eg_score;
inline int phase;
inline int maxPhase;
inline U64 attacksTo(const Board& board, int toSq, int side) {
    U64 attackers = 0ULL;
    U64 occ = board.allOccupied;
    attackers |= pawnAttacks[side^1][toSq] & board.piece[side][PAWN];
    attackers |= knightAttacks[toSq] & board.piece[side][KNIGHT];
    attackers |= bishopAttacks(toSq, occ) & (board.piece[side][BISHOP] | board.piece[side][QUEEN]);
    attackers |= rookAttacks(toSq, occ) & (board.piece[side][ROOK]     | board.piece[side][QUEEN]);
    attackers |= kingAttacks[toSq] & board.piece[side][KING];
    return attackers;
}
inline void pawn_structure_score(const Board& board, int color, int& mg, int& eg) {
    U64 pawns = board.piece[color][PAWN];
    U64 enemyPawns = board.piece[color^1][PAWN];

    for (int f=0; f<8; ++f) {
        int cnt = __builtin_popcountll(pawns & fileMask(f));
        if (cnt > 1) {
            mg -= DOUBLED_PAWN_PENALTY[0] * (cnt - 1);
            eg -= DOUBLED_PAWN_PENALTY[1] * (cnt - 1);
        }
    }

    U64 temp = pawns;
    while (temp) {
        int sq = __builtin_ctzll(temp);
        temp &= temp-1;
        int f = file_of(sq);

        if ((((f > 0) ? fileMask(f-1) : 0ULL) | ((f < 7) ? fileMask(f+1) : 0ULL)) & pawns == 0) {
            mg -= ISOLATED_PAWN_PENALTY[0];
            eg -= ISOLATED_PAWN_PENALTY[1];
        }

        U64 forward_mask = allForwardRanksMask[color][sq];
        U64 enemy_zone = forward_mask & (((f > 0) ? fileMask(f-1) : 0ULL) | fileMask(f) | ((f < 7) ? fileMask(f+1) : 0ULL));
        if ((enemyPawns & enemy_zone) == 0) {
            mg += PASSED_PAWN_BONUS[0];
            eg += PASSED_PAWN_BONUS[1];
        }
    }
}

inline void king_safety_score(const Board& board, int color, int& mg, int& eg) {
    int kingSq = __builtin_ctzll(board.piece[color][KING]);
    if (kingSq < 0 || kingSq > 63) return;

    int shield_count = 0;
    U64 king_front_span = (color == WHITE) ? kingFrontPawnShield[kingSq] : kingBackPawnShield[kingSq];
    shield_count = __builtin_popcountll(board.piece[color][PAWN] & king_front_span);
    mg += KING_PAWN_SHIELD_BONUS[0] * shield_count;
    eg += KING_PAWN_SHIELD_BONUS[1] * shield_count;

    // *** BUG FIX ***: We must find attackers from the enemy side (color^1)
    U64 attackers_bb = attacksTo(board, kingSq, color^1);
    int num_attackers = __builtin_popcountll(attackers_bb);
    mg -= KING_ATTACKER_PENALTY[0] * num_attackers;
    eg -= KING_ATTACKER_PENALTY[1] * num_attackers;
}

inline void piece_activity_score(const Board& board, int color, int& mg, int& eg) {
    // Rooks
    U64 rooks = board.piece[color][ROOK];
    while (rooks) {
        int sq = __builtin_ctzll(rooks);
        rooks &= rooks-1;
        U64 file = fileMask(file_of(sq));
        if ((board.piece[color][PAWN] & file) == 0) {
            if ((board.piece[color^1][PAWN] & file) == 0) {
                mg += ROOK_OPEN_FILE_BONUS[0];
                eg += ROOK_OPEN_FILE_BONUS[1];
            } else {
                mg += ROOK_SEMIOPEN_FILE_BONUS[0];
                eg += ROOK_SEMIOPEN_FILE_BONUS[1];
            }
        }
    }

    // Bishop Pair
    if (__builtin_popcountll(board.piece[color][BISHOP]) >= 2) {
        mg += BISHOP_PAIR_BONUS[0];
        eg += BISHOP_PAIR_BONUS[1];
    }

    // Center Control
    int center_pawns = __builtin_popcountll(board.piece[color][PAWN] & CENTER_16);
    int center_pieces = __builtin_popcountll(board.occupied[color] & CENTER_16);
    mg += (center_pawns * 2 + center_pieces) * CENTER_CONTROL_BONUS[0];
    eg += (center_pawns * 2 + center_pieces) * CENTER_CONTROL_BONUS[1];
}
inline void init_masks() {

    for (int sq = 0; sq < 64; ++sq) {
        allForwardRanksMask[WHITE][sq] = 0ULL;
        for (int r = rank_of(sq) + 1; r < 8; ++r) {
            allForwardRanksMask[WHITE][sq] |= rankMask(r);
        }

        allForwardRanksMask[BLACK][sq] = 0ULL;
        for (int r = rank_of(sq) - 1; r >= 0; --r) {
            allForwardRanksMask[BLACK][sq] |= rankMask(r);
        }

        U64 king_bb = 1ULL << sq;
        int f = file_of(sq);

        U64 front_shield = (king_bb << 8);
        if (f > 0) front_shield |= (king_bb << 7);
        if (f < 7) front_shield |= (king_bb << 9);
        kingFrontPawnShield[sq] = front_shield;

        U64 back_shield = (king_bb >> 8);
        if (f > 0) back_shield |= (king_bb >> 9);
        if (f < 7) back_shield |= (king_bb >> 7);
        kingBackPawnShield[sq] = back_shield;
    }
}
inline void init_eval(Board& board) {
    init_tables();
    init_masks();
    mg_score = 0; eg_score = 0; phase = 0; maxPhase=0;
    U64 occ = board.allOccupied;
    while (occ) {
        int sq = __builtin_ctzll(occ);
        occ&=occ-1;
        int piece = board.mailbox[sq];
        int color = ((1ULL<<sq)&board.occupied[0])? WHITE : BLACK;
        mg_score += mg_table[color][piece][sq];
        eg_score += eg_table[color][piece][sq];
        phase += gamephaseInc[piece];
    }
    maxPhase=phase;
}

inline int eval(Board& board) {
    // 1. Get base scores (incrementally updated by make/unmake)
    int total_mg = mg_score;
    int total_eg = eg_score;

    // 2. Calculate scores for each feature for both sides
    int mg_w = 0, eg_w = 0, mg_b = 0, eg_b = 0;
    pawn_structure_score(board, WHITE, mg_w, eg_w);
    pawn_structure_score(board, BLACK, mg_b, eg_b);
    king_safety_score(board, WHITE, mg_w, eg_w);
    king_safety_score(board, BLACK, mg_b, eg_b);
    piece_activity_score(board, WHITE, mg_w, eg_w);
    piece_activity_score(board, BLACK, mg_b, eg_b);

    // 3. Add feature scores to the total
    total_mg += (mg_w - mg_b);
    total_eg += (eg_w - eg_b);

    // 4. Taper the final score
    int current_phase = phase;
    if (current_phase > maxPhase) current_phase = maxPhase; // Cap phase for promotions

    // Avoid division by zero on boards with only kings
    if (maxPhase == 0) return 0;

    int final_score = (total_mg * current_phase + total_eg * (maxPhase - current_phase)) / maxPhase;

    return (board.currentColor == WHITE) ? final_score : -final_score;
}

#endif //EVALUATION_H
