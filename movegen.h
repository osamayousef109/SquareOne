

#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "bitboards.h"
#include "move.h"
constexpr U64 WSC = (1ULL<<5)+(1ULL<<6);
constexpr U64 WLC = (1ULL<<1)+(1ULL<<2)+(1ULL<<3);
constexpr U64 BSC = (1ULL<<61)+(1ULL<<62);
constexpr U64 BLC = (1ULL<<57)+(1ULL<<58)+(1ULL<<59);

inline void generateMoves(Board& board,int ply) {
    moveCount[ply]=0;
    int color=board.currentColor;
    U64 notOwn=~board.occupied[color];

    U64 pawns=board.piece[color][PAWN];
    U64 singlePushPawns=pawnsAble2Push(pawns,board.empty,color);
    U64 doublePushPawns=pawnsAble2DblPush(pawns,board.empty,color);
    while (singlePushPawns) {
        int from=__builtin_ctzll(singlePushPawns);
        int forward=(color==WHITE? 8:-8);
        int promoRank=(color==WHITE? 7:0);
        int to=from+forward;
        singlePushPawns&=singlePushPawns-1;
        if (to/8==promoRank) {
            for (int i=1;i<=4;i++) {
                addMove(board,from,to,i,false,false,ply);
            }
        }else {
            addMove(board,from,to,0,false,false,ply);
        }
    }
    while (doublePushPawns) {
        int from=__builtin_ctzll(doublePushPawns);
        int forward=(color==WHITE? 16:-16);
        int to=from+forward;
        doublePushPawns&=doublePushPawns-1;
        addMove(board,from,to,0,false,false,ply);
    }
    while (pawns) {
        int promoRank=(color==WHITE? 7:0);
        int from=__builtin_ctzll(pawns);
        pawns&=pawns-1;
        U64 moves = pawnAttacks[color][from] & (board.occupied[1-color]);
        while (moves) {
            int to=__builtin_ctzll(moves);
            moves&=moves-1;
            if (to/8==promoRank) {
                for (int i=1;i<=4;i++) {
                    addMove(board,from,to,i,false,false,ply);
                }
            }else {
                addMove(board,from,to,0,false,false,ply);
            }
        }
        if (board.enpassant!=-1) {
            U64 enpassant= pawnAttacks[color][from] & (1ULL<<board.enpassant);
            if (enpassant) {
                int to=__builtin_ctzll(enpassant);
                addMove(board,from,to,0,false,true,ply);
            }
        }
    }

    U64 knights=board.piece[color][KNIGHT];
    while (knights) {
        int from=__builtin_ctzll(knights);
        knights&=knights-1;
        U64 moves = knightAttacks[from] & ~board.occupied[color];
        while (moves) {
            int to=__builtin_ctzll(moves);
            moves&=moves-1;
            addMove(board,from,to,0,false,false,ply);
        }
    }

    U64 bishops=board.piece[color][BISHOP];
    while (bishops) {
        int from=__builtin_ctzll(bishops);
        bishops&=bishops-1;
        U64 moves = bishopAttacks(from,board.allOccupied) & notOwn;
        while (moves) {
            int to=__builtin_ctzll(moves);
            moves&=moves-1;
            addMove(board,from,to,0,false,false,ply);
        }
    }

    U64 rooks=board.piece[color][ROOK];
    while (rooks) {
        int from=__builtin_ctzll(rooks);
        rooks&=rooks-1;
        U64 moves = rookAttacks(from,board.allOccupied) & notOwn;
        while (moves) {
            int to=__builtin_ctzll(moves);
            moves&=moves-1;
            addMove(board,from,to,0,false,false,ply);
        }
    }

    U64 queens=board.piece[color][QUEEN];
    while (queens) {
        int from=__builtin_ctzll(queens);
        queens&=queens-1;
        U64 moves = (rookAttacks(from,board.allOccupied)|bishopAttacks(from,board.allOccupied)) & notOwn;
        while (moves) {
            int to=__builtin_ctzll(moves);
            moves&=moves-1;
            addMove(board,from,to,0,false,false,ply);
        }
    }

    U64 king=board.piece[color][KING];
    if (king) {
        int from=__builtin_ctzll(king);
        U64 moves = kingAttacks[from]&(~board.occupied[color]);
        while (moves) {
            int to=__builtin_ctzll(moves);
            moves&=moves-1;
            addMove(board,from,to,0,false,false,ply);
        }
        if (!isAttacked(board,color,from)) {
            if (color==WHITE) {
                if (board.castlingRights&(1<<0)) {
                    if (!(board.allOccupied&WSC)&&!isAttacked(board,WHITE,6)&&!isAttacked(board,WHITE,5))
                        addMove(board,4,6,0,true,false,ply);
                }
                if (board.castlingRights&(1<<1)) {
                    if (!(board.allOccupied&WLC)&&!isAttacked(board,WHITE,3)&&!isAttacked(board,WHITE,2))
                        addMove(board,4,2,0,true,false,ply);
                }
            }else {
                if (board.castlingRights&(1<<2)) {
                    if (!(board.allOccupied&BSC)&&!isAttacked(board,BLACK,61)&&!isAttacked(board,BLACK,62))
                        addMove(board,60,62,0,true,false,ply);
                }
                if (board.castlingRights&(1<<3)) {
                    if (!(board.allOccupied&BLC)&&!isAttacked(board,BLACK,58)&&!isAttacked(board,BLACK,59))
                        addMove(board,60,58,0,true,false,ply);
                }
            }
        }
    }
}
#endif //MOVEGEN_H
