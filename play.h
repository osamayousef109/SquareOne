
#ifndef PLAY_H
#define PLAY_H
#include <iostream>
#include "search.h"
inline void play(Board& board) {
    int from;
    int to;
    std::cin >> from >> to;
    generateMoves(board,0);
    for (int i=0;i<moveCount[0];i++) {
        Move move=moves[0][i];
        int f,t;
        f=fromSquare(move);
        t=toSquare(move);
        if (f==from&&t==to) {
            makeMove(board,move,0);
            break;
        }
    }
    generation++;
    int prevScore=eval(board,0);
    int score=-1;
    for (int depth=1;depth<=5;depth++) {
        int delta=50;
        int alpha=prevScore-delta;
        int beta=prevScore+delta;
        while (true) {
            score=alphaBeta(board,alpha,beta,depth,0);
            if (score<=alpha) {
                delta*=2;
                if (delta>MATE) {
                    score=alphaBeta(board, -INF, INF, depth, 0);
                    break;
                }
                alpha=prevScore-delta;
                continue;
            }
            if (score>=beta) {
                delta*=2;
                if (delta>MATE) {
                    score=alphaBeta(board,-INF,INF,depth,0);
                    break;
                }
                beta=prevScore+delta;
                continue;
            }
            prevScore=score;
            break;
        }
    }
    std::cout << score << std::endl;
    std::cout << fromSquare(bestMove) << " " << toSquare(bestMove) << std::endl;
    makeMove(board,bestMove,0);
}
#endif //PLAY_H