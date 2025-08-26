#include "test.h"
#include "movegen.h"
#include "move.h"
#include "TT.h"

long long perft(Board& board,int depth,int ply) {
    if (depth==0) {
        return 1;
    }
    long long nodes=0;
    if (probePerftTT(board.zobristKey,depth,nodes)) {
        return nodes;
    }
    generateMoves(board,ply);
    for (int i=0;i<moveCount[ply];i++) {
        makeMove(board,moves[ply][i],ply);
        nodes+=perft(board,depth-1,ply+1);
        unmakeMove(board,ply);
    }
    storePerftTT(board.zobristKey,depth,nodes);
    return nodes;
}
