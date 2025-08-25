
#include <chrono>

#include "attacks.h"
#include "bitboards.h"
#include "play.h"
#include "test.h"

int main() {
    initAttacks();
    initMagic();
    Board board;
    board.initZobrist();
    while(true) {
        play(board);
    }
}
