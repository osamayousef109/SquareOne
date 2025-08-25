//
// Created by MSi on 8/25/2025.
//

#ifndef EVALUATION_H
#define EVALUATION_H
#include "movegen.h"

constexpr int INF=1e7;
constexpr int MATE=32000;
constexpr int pieceValue[5]={100,320,330,500,900};
inline int eval(Board& board,int ply) {
    int wq=__builtin_popcountll(board.piece[WHITE][QUEEN]);
    int wr=__builtin_popcountll(board.piece[WHITE][ROOK]);
    int wb=__builtin_popcountll(board.piece[WHITE][BISHOP]);
    int wn=__builtin_popcountll(board.piece[WHITE][KNIGHT]);
    int wp=__builtin_popcountll(board.piece[WHITE][PAWN]);
    int bq=__builtin_popcountll(board.piece[BLACK][QUEEN]);
    int br=__builtin_popcountll(board.piece[BLACK][ROOK]);
    int bb=__builtin_popcountll(board.piece[BLACK][BISHOP]);
    int bn=__builtin_popcountll(board.piece[BLACK][KNIGHT]);
    int bp=__builtin_popcountll(board.piece[BLACK][PAWN]);
    int material=900*(wq-bq)+500*(wr-br)+300*(wn+wb-bn-bb)+100*(wp-bp);
    generateMoves(board,ply);
    int wm=moveCount[ply];
    if (wm==0) {
        if (isAttacked(board,board.currentColor,board.kingPos[board.currentColor]))
            return -INF+ply;
        return 0;
    }
    moveCount[ply]=0;
    board.currentColor=(Color)(1-board.currentColor);
    generateMoves(board,ply);
    int bm=moveCount[ply];
    board.currentColor=(Color)(1-board.currentColor);
    int mobility=50*(wm-bm);
    int perspective = (board.currentColor == WHITE) ? 1 : -1;
    return (material+mobility)*perspective;
}

#endif //EVALUATION_H
