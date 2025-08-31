
#ifndef PLAY_H
#define PLAY_H
#include <iostream>
#include "search.h"
inline std::string squareToString(int square) {
    if (square < 0 || square > 63) return ""; // Should not happen
    int file = square % 8;
    int rank = square / 8;
    std::string s;
    s += (char)('a' + file);
    s += (char)('1' + rank);
    return s;
}
inline std::string moveToString(int fromSquare, int toSquare, char promotionPiece) {
    std::string moveStr = squareToString(fromSquare) + squareToString(toSquare);
    if (promotionPiece != ' ' && promotionPiece != '\0') {
        moveStr += promotionPiece;
    }
    return moveStr;
}
inline std::string play(Board& board,const SearchLimits& searchLimits) {
    TIME_LIMIT=searchLimits.movetime;
    generation++;
    int prevScore=0;
    int score=-1;
    bestMove=-1;
    start=std::chrono::high_resolution_clock::now();
    int finalDepth=0;
    for (int depth=1;depth<=searchLimits.depth;depth++) {
        int delta=50;
        int alpha=prevScore-delta;
        int beta=prevScore+delta;
        while (true) {
            score=alphaBeta(board,alpha,beta,depth,0);
            if (outOfTime()||!isSearching) {
                bestMove=prevBestMove;
                score=prevScore;
                break;
            }
            if (score<=alpha) {
                delta*=2;
                if (delta>MATE) {
                    score=alphaBeta(board, -INF, INF, depth, 0);
                    if (outOfTime()||!isSearching) {
                        bestMove=prevBestMove;
                        score=prevScore;
                        break;
                    }
                    break;
                }
                alpha=prevScore-delta;
                continue;
            }
            if (score>=beta) {
                delta*=2;
                if (delta>MATE) {
                    score=alphaBeta(board,-INF,INF,depth,0);
                    if (outOfTime()||!isSearching) {
                        bestMove=prevBestMove;
                        score=prevScore;
                        break;
                    }
                    break;
                }
                beta=prevScore+delta;
                continue;
            }
            break;
        }
        if (outOfTime()||!isSearching) {
            bestMove=prevBestMove;
            score=prevScore;
            break;
        }
        prevScore=score;
        prevBestMove=bestMove;
        finalDepth=depth;
    }
    int promo=promotionPiece(bestMove);
    char promoPiece;
    switch (promo) {
        case 1 : promoPiece = 'n'; break;
        case 2 : promoPiece = 'b'; break;
        case 3 : promoPiece = 'r'; break;
        case 4 : promoPiece = 'q'; break;
        default: promoPiece = '\0';
    }
    return moveToString(fromSquare(bestMove),toSquare(bestMove),promoPiece);
}
#endif //PLAY_H