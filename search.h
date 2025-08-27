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
    gain[d] = pieceValue[targetType];
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
        gain[d] = pieceValue[pieceOnSqType]-gain[d-1];
        U64 attackers = 0ULL;

        attackers |= pawnAttacks[side^1][toSq] & temp_piece[side][PAWN];
        attackers |= knightAttacks[toSq] & temp_piece[side][KNIGHT];
        attackers |= bishopAttacks(toSq, occ) & (temp_piece[side][BISHOP] | temp_piece[side][QUEEN]);
        attackers |= rookAttacks(toSq, occ) & (temp_piece[side][ROOK]     | temp_piece[side][QUEEN]);
        attackers |= kingAttacks[toSq] & temp_piece[side][KING];
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
                moveScores[ply][i]=2000000+MVV_LVA[fromPiece][toPiece];
            else
                moveScores[ply][i]=-1000000+MVV_LVA[fromPiece][toPiece];
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
    int standPat=eval(board,ply);
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
            if (score>=beta)
                return score;
            if (score>alpha)
                alpha=score;
        }else {
            break;
        }
    }
    return alpha;
}
inline int alphaBeta(Board& board,int alpha,int beta,int depth,int ply) {
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
        int score=-alphaBeta(board,-beta,-alpha,depth-1,ply+1);
        unmakeMove(board,ply);
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
