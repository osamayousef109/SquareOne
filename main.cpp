#include "attacks.h"
#include "bitboards.h"
#include "play.h"

int main() {
    initAttacks();
    initMagic();
    Board board;
    board.initZobrist();
    init_eval(board);
    while(true) {
        play(board);
    }
}