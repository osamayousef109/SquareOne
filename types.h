//
// Created by MSi on 8/23/2025.
//

#ifndef TYPES_H
#define TYPES_H
#include <atomic>
#include <cstdint>
#include <chrono>
#include <thread>
using U64 = uint64_t;
using Move = uint32_t;
constexpr uint8_t FLAG_CAPTURE   = 1 << 0;
constexpr uint8_t FLAG_EN_PASSANT= 1 << 1;
constexpr uint8_t FLAG_CASTLE    = 1 << 2;
constexpr uint8_t FLAG_PROMOTION = 1 << 3;
enum Piece {PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING,EMPTY};
enum Color {WHITE,BLACK,NO_COLOR};
inline uint16_t generation=0;
inline auto start=std::chrono::high_resolution_clock::now();
inline std::atomic<bool> isSearching(false);
inline std::thread searchThread;
inline int TIME_LIMIT=5000;
inline Move prevBestMove=-1;
struct EvalDelta { int mg, eg, phase; };
struct History {
    Move move;
    int captured;
    uint8_t castlingRights;
    int prevEnpassant;
    int captureSquare;
    int halfMoveClock;
    U64 zobristKey;
    EvalDelta d;
};
struct SearchLimits {
    int wtime = 10000, btime = 10000, winc = 0, binc = 0;
    int movestogo = 30;
    int depth = 32;
    long long nodes = -1;
    int movetime = 5000;
    bool infinite = false;
    bool ponder = false;
};
inline Move make_move(int from,int to,int promo=0,int flags=0) {
    return (from) | (to << 6) | (promo << 12) | (flags << 16);
}
inline int fromSquare(Move m)      { return  (int)m        & 0x3F; }
inline int toSquare(Move m)        { return (int)(m >> 6)  & 0x3F; }
inline int promotionPiece(Move m)  { return (int)(m >> 12) & 0xF;  }
inline int moveFlags(Move m)       { return (int)(m >> 16) & 0x3F; }
inline bool outOfTime() {
    auto now=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> elapsed=now-start;
    return elapsed.count()>=TIME_LIMIT;
}
#endif //TYPES_H
