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
inline int quiesce(Board& board,int alpha,int beta,int ply) {
    int standPat=eval(board,ply);
    if (standPat>=beta)
        return standPat;
    if (standPat>alpha)
        alpha=standPat;
    generateMoves(board,ply);
    for (int i=0;i<moveCount[ply];i++) {
        int mx=i;
        for (int j=i+1;j<moveCount[ply];j++) {
            if (moveScores[ply][j]>moveScores[ply][mx])
                mx=j;
        }
        std::swap(moveScores[ply][i],moveScores[ply][mx]);
        std::swap(moves[ply][i],moves[ply][mx]);
    }
    for (int i=0;i<moveCount[ply];i++) {
        Move move=moves[ply][i];
        int flag=moveFlags(move);
        if (flag&(FLAG_CAPTURE)) {
            int gain=pieceValue[board.mailbox[toSquare(move)]];
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
        }
    }
    return alpha;
}
inline int alphaBeta(Board& board,int alpha,int beta,int depth,int ply) {
    int ttscore=-1;
    Move ttmove=0;
    int ogAlpha=alpha;
    if (probeTT(board.zobristKey,depth,alpha,beta,ttscore,ttmove)&&ply) {
        if (ttscore > MATE - 1000) ttscore -= ply;
        if (ttscore < -MATE + 1000) ttscore += ply;
        return ttscore;
    }
    if (depth==0)
        return quiesce(board,alpha,beta,ply);
    generateMoves(board,ply);
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
    if (bestValue>MATE-1000) storeScore += ply;
    if (bestValue<-MATE+1000) storeScore -= ply;
    storeTT(board.zobristKey,best,depth,storeScore,type);

    return bestValue;
}

#endif //SEARCH_H
