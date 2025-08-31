//
// Created by MSi on 8/25/2025.
//

#ifndef SEARCH_H
#define SEARCH_H
#include "evaluation.h"
#include "movegen.h"
#include "TT.h"
inline Move bestMove=-1;
constexpr int TT_SCORE=1e8;
constexpr int NULL_R = 2;
constexpr int LMR_MIN_DEPTH = 3;
constexpr int LMR_LATE_MOVES = 4;
inline int moveScores[MAX_PLY][MAX_MOVES];
inline int MVV_LVA[6][6] = {
    // Attacker:   P     N     B     R     Q     K
    /* Victim P */ {105, 104, 103, 102, 101, 100},
    /* Victim N */ {205, 204, 203, 202, 201, 200},
    /* Victim B */ {305, 304, 303, 302, 301, 300},
    /* Victim R */ {405, 404, 403, 402, 401, 400},
    /* Victim Q */ {505, 504, 503, 502, 501, 500},
    /* Victim K */ {605, 604, 603, 602, 601, 600}
};
inline Move killer[MAX_PLY][2];
inline int historyScore[64][64];
constexpr int KILLER1_SCORE=1000000;
constexpr int KILLER2_SCORE=KILLER1_SCORE-1;
inline void add_killer(int ply, Move m) {
    if (m==killer[ply][0]) return;
    killer[ply][1] = killer[ply][0];
    killer[ply][0] = m;
}
inline void add_history(Move m, int depth) {
    int f=fromSquare(m);
    int t=toSquare(m);
    historyScore[f][t]+=depth*depth;
}
inline void clear_history() {
    for (auto & f : historyScore)
        for (int & t : f)
            t = 0;
}
inline void clear_killers() {
    for (auto & i : killer)
        for (unsigned int & k : i)
            k = 0;
}
inline void makeNullMove(Board &board) {
    board.zobristKey^=board.colorKey;
    if (board.ep_is_capturable(board.enpassant,board.currentColor))
        board.zobristKey^=board.enpassantFile[board.enpassant&7];
    board.currentColor=(Color)(1-board.currentColor);
    board.enpassant=-1;
}

inline void undoNullMove(Board &board,int enpassant) {
    board.currentColor=(Color)(1-board.currentColor);
    board.zobristKey^=board.colorKey;
    board.enpassant=enpassant;
    if (board.ep_is_capturable(enpassant,board.currentColor))
        board.zobristKey^=board.enpassantFile[enpassant&7];

}
inline bool isPinned(const Board &board, int side, int fromSq, U64 occ) {
    int kingSq=board.kingPos[side];
    U64 from_bb=1ULL<<fromSq;
    U64 occ2=occ&~from_bb;
    U64 enemyRooksQueens   = board.piece[side^1][ROOK]  | board.piece[side^1][QUEEN];
    U64 enemyBishopsQueens = board.piece[side^1][BISHOP] | board.piece[side^1][QUEEN];
    if (rookAttacks(kingSq, occ2) & enemyRooksQueens) return true;
    if (bishopAttacks(kingSq, occ2) & enemyBishopsQueens) return true;
    return false;
}
inline int getLeastValuableAttacker(U64 attackers, int side, U64 temp_piece[2][6], int &outPieceType) {
    for (int p = PAWN; p <= KING; ++p) {
        U64 piece_attackers = attackers & temp_piece[side][p];
        if (piece_attackers) {
            outPieceType = p;
            return __builtin_ctzll(piece_attackers);
        }
    }
    outPieceType = -1;
    return -1;
}

inline int see(Board &board, int toSq, int targetType, int fromSq, int attackerType, int side) {
    const int MAX_DEPTH = 32;
    int gain[MAX_DEPTH];
    int d = 0;
    U64 from_bb = 1ULL << fromSq;
    U64 to_bb   = 1ULL << toSq;
    U64 temp_piece[2][6];
    for (int s = 0; s < 2; ++s)
        for (int p = 0; p < 6; ++p)
            temp_piece[s][p] = board.piece[s][p];
    U64 occ = board.allOccupied;
    gain[d] = mg_value[targetType];
    int opponent = side ^ 1;
    temp_piece[opponent][targetType] &= ~to_bb;
    temp_piece[side][attackerType] &= ~from_bb;
    temp_piece[side][attackerType] |= to_bb;
    occ &= ~from_bb;
    int pieceOnSqType = attackerType;
    while (true) {
        side ^= 1;
        d++;
        if (d >= MAX_DEPTH - 1) break;
        gain[d] = mg_value[pieceOnSqType]-gain[d-1];
        U64 attackers = 0ULL;

        attackers |= pawnAttacks[side^1][toSq] & temp_piece[side][PAWN];
        attackers |= knightAttacks[toSq] & temp_piece[side][KNIGHT];
        attackers |= bishopAttacks(toSq, occ) & (temp_piece[side][BISHOP] | temp_piece[side][QUEEN]);
        attackers |= rookAttacks(toSq, occ) & (temp_piece[side][ROOK]     | temp_piece[side][QUEEN]);
        attackers |= kingAttacks[toSq] & temp_piece[side][KING];

        int kingSq = board.kingPos[side];
        U64 attackers_copy = attackers;

        while (attackers_copy) {
            int fromSq = __builtin_ctzll(attackers_copy);
            U64 from_bb = 1ULL << fromSq;

            U64 occ_without_attacker = occ & ~from_bb;
            U64 enemyRooksQueens   = temp_piece[side^1][ROOK]  | temp_piece[side^1][QUEEN];
            U64 enemyBishopsQueens = temp_piece[side^1][BISHOP] | temp_piece[side^1][QUEEN];

            if ((rookAttacks(kingSq, occ_without_attacker) & enemyRooksQueens) ||
                (bishopAttacks(kingSq, occ_without_attacker) & enemyBishopsQueens))
            {
                if ((LineBB[kingSq][toSq] & from_bb) == 0) {
                    attackers &= ~from_bb;
                }
            }
            attackers_copy &= attackers_copy - 1;
        }


        int nextAttackerSq = -1;
        int nextAttackerType = -1;
        nextAttackerSq = getLeastValuableAttacker(attackers, side, temp_piece, nextAttackerType);

        if (nextAttackerSq == -1) {
            break;
        }
        U64 attacker_from_bb = 1ULL << nextAttackerSq;
        int capturedSide = side ^ 1;
        temp_piece[capturedSide][pieceOnSqType] &= ~to_bb;
        temp_piece[side][nextAttackerType] &= ~attacker_from_bb;
        temp_piece[side][nextAttackerType] |= to_bb;
        occ &= ~attacker_from_bb;
        pieceOnSqType = nextAttackerType;
    }
    while (--d > 0) {
        gain[d-1]= -std::max(-gain[d-1],gain[d]);
    }
    return gain[0];
}

inline void order_moves(Board& board,int ply) {
    for (int i=0;i<moveCount[ply];i++) {
        Move m=moves[ply][i];
        if (moveFlags(m)&(FLAG_CAPTURE)) {
            int t=toSquare(m);
            int f=fromSquare(m);
            int fromPiece=board.mailbox[f];
            int toPiece=board.mailbox[t];
            int score=see(board,t,toPiece,f,fromPiece,board.currentColor);
            if (score>=0)
                moveScores[ply][i]=2000000+MVV_LVA[toPiece][fromPiece];
            else
                moveScores[ply][i]=-1000000+MVV_LVA[toPiece][fromPiece];
            continue;
        } else if ((m==killer[ply][0])) {
            moveScores[ply][i] = KILLER1_SCORE;
            continue;
        } else if (m==killer[ply][1]) {
            moveScores[ply][i] = KILLER2_SCORE;
            continue;
        }
        int f=fromSquare(moves[ply][i]);
        int t=toSquare(moves[ply][i]);
        moveScores[ply][i]=historyScore[f][t];
    }
}
inline int quiesce(Board& board,int alpha,int beta,int ply) {
    if (outOfTime()||!isSearching) {
        bestMove=prevBestMove;
        return -INF;
    }
    int standPat=eval(board);
    if (ply>=63) return standPat;
    if (standPat>=beta)
        return standPat;
    if (standPat>alpha)
        alpha=standPat;
    generateMoves(board,ply);
    order_moves(board,ply);
    for (int i=0;i<moveCount[ply];i++) {
        int mx=i;
        for (int j=i+1;j<moveCount[ply];j++) {
            if (moveScores[ply][j]>moveScores[ply][mx])
                mx=j;
        }
        std::swap(moveScores[ply][i],moveScores[ply][mx]);
        std::swap(moves[ply][i],moves[ply][mx]);
        Move move=moves[ply][i];
        int flag=moveFlags(move);
        if (flag&(FLAG_CAPTURE)) {
            int f=fromSquare(move);
            int t=toSquare(move);
            int gain=see(board,t,board.mailbox[t],f,board.mailbox[f],board.currentColor);
            if (standPat+gain+100<=alpha) {
                continue;
            }
            makeMove(board,move,ply);
            int score=-quiesce(board,-beta,-alpha,ply+1);
            unmakeMove(board,ply);
            if (outOfTime()||!isSearching) {
                bestMove=prevBestMove;
                return -INF;
            }
            if (score>=beta)
                return score;
            if (score>alpha)
                alpha=score;
        }
    }
    return alpha;
}
inline int alphaBeta(Board& board,int alpha,int beta,int depth,int ply) {
    if (outOfTime()||!isSearching) {
        bestMove=prevBestMove;
        return -INF;
    }
    if (ply==0) {
        clear_history();
        clear_killers();
    }
    int ttscore=-1;
    Move ttmove=0;
    int ogAlpha=alpha;
    if (ply&&probeTT(board.zobristKey,depth,alpha,beta,ttscore,ttmove)) {
        return ttscore;
    }
    if (depth==0)
        return quiesce(board,alpha,beta,ply);
    generateMoves(board,ply);
    order_moves(board,ply);
    if (moveCount[ply]==0) {
        if (isAttacked(board,board.currentColor,board.kingPos[board.currentColor]))
            return -MATE+ply;
        return 0;
    }
    int bestValue=-INF;
    Move best=moves[ply][0];
    if (ttmove!=0) {
        for (int i=0;i<moveCount[ply];i++) {
            if (ttmove==moves[ply][i]) {
                moveScores[ply][i]=TT_SCORE;
            }
        }
    }
    bool inCheck=isAttacked(board, board.currentColor, board.kingPos[board.currentColor]);
    if (!inCheck&&depth>=(NULL_R+1)) {
        int count = 0;
        count += ((board.piece[WHITE][PAWN]|board.piece[BLACK][PAWN])!=0ULL);
        count += ((board.piece[WHITE][KNIGHT]|board.piece[BLACK][KNIGHT])!=0ULL);
        count += ((board.piece[WHITE][BISHOP]|board.piece[BLACK][BISHOP])!=0ULL);
        count += ((board.piece[WHITE][ROOK]|board.piece[BLACK][ROOK])!=0ULL);
        bool queen=((board.piece[WHITE][QUEEN]|board.piece[BLACK][QUEEN])!=0ULL);
        if (!(count<=2&&!queen)){
            int oldEP=board.enpassant;
            makeNullMove(board);
            int score=-alphaBeta(board,-beta,-beta+1,depth-1-NULL_R,ply+1);
            undoNullMove(board,oldEP);
            if (outOfTime()||!isSearching) {
                bestMove=prevBestMove;
                return -INF;
            }
            if (score>=beta) {
                if (depth-2>2) {
                    int verify=alphaBeta(board,alpha,beta,depth-2,ply+1);
                    if (outOfTime()||!isSearching) {
                        bestMove=prevBestMove;
                        return -INF;
                    }
                    if (verify>=beta) return verify;
                } else {
                    return beta;
                }
            }
        }
    }
    for (int i=0;i<moveCount[ply];i++) {
        int mx=i;
        for (int j=i+1;j<moveCount[ply];j++) {
            if (moveScores[ply][j]>moveScores[ply][mx]) {
                mx=j;
            }
        }
        std::swap(moveScores[ply][i],moveScores[ply][mx]);
        std::swap(moves[ply][i],moves[ply][mx]);
        makeMove(board,moves[ply][i],ply);
        int score;
        int flag=moveFlags(moves[ply][i]);
        if (depth >= 5 && i >= 4 && !(flag&FLAG_CAPTURE) && !(flag&FLAG_PROMOTION)) {
            int r = 1;
            if (depth>=6) r=2;
            if (depth>=10) r=3;
            int reducedDepth = depth - 1 - r;
            if (reducedDepth < 0) reducedDepth = 0;
            score = -alphaBeta(board, -alpha-1, -alpha, reducedDepth, ply+1);
            if (score > alpha) {
                score = -alphaBeta(board, -beta, -alpha, depth-1, ply+1);
            }
        } else {
            score = -alphaBeta(board, -beta, -alpha, depth-1, ply+1);
        }
        unmakeMove(board,ply);
        if (outOfTime()||!isSearching) {
            bestMove=prevBestMove;
            return -INF;
        }
        if (score>bestValue) {
            bestValue=score;
            best=moves[ply][i];
        }
        if (score>alpha)
            alpha=score;
        if (alpha>=beta) {
            if (!(moveFlags(moves[ply][i])&FLAG_CAPTURE)) {
                add_killer(ply,moves[ply][i]);
                add_history(moves[ply][i],depth);
            }
            break;
        }
    }
    if (ply==0)
        bestMove=best;
    uint8_t type=EXACT;
    if (bestValue<=ogAlpha) {
        type=UPPER_BOUND;
    } else if (bestValue>=beta) {
        type=LOWER_BOUND;
    }
    int storeScore = bestValue;
    storeTT(board.zobristKey,best,depth,storeScore,type);

    return bestValue;
}

#endif //SEARCH_H
