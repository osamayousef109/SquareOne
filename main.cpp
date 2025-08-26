
#include <chrono>

#include "attacks.h"
#include "bitboards.h"
#include "play.h"
#include "test.h"

int main() {
    initAttacks();
    initMagic();
    Board board;
    board.setFen("1r5k/p1p2R2/6Q1/7p/NP6/P6P/6P1/4q2K w - - 3 36");
    board.initZobrist();
    while(true) {
        play(board);
    }
}
