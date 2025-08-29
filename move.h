
#ifndef MOVE_H
#define MOVE_H
#include "types.h"
#include "bitboards.h"
#include "evaluation.h"
inline void makeMove(Board& board,Move move,int ply) {
    History h{};
    EvalDelta d{0,0,0};
    int from=fromSquare(move);
    int to=toSquare(move);
    int promo=promotionPiece(move);
    int flag=moveFlags(move);
    int piece=board.mailbox[from];
    int color=board.currentColor;
    int enemy=1-color;
    h.captured=board.mailbox[to];
    h.prevEnpassant=board.enpassant;
    h.captureSquare=to;
    h.zobristKey=board.zobristKey;
    d.mg   -= mg_table[color][piece][from];
    d.eg   -= eg_table[color][piece][from];
    d.mg   += mg_table[color][piece][to];
    d.eg   += eg_table[color][piece][to];
    if (board.ep_is_capturable(board.enpassant, color)) {
        board.zobristKey ^= board.enpassantFile[board.enpassant & 7];
    }
    if (!(flag&FLAG_CAPTURE)) h.captureSquare=-1;
    h.move=move;
    h.castlingRights=board.castlingRights;
    h.halfMoveClock=board.halfMoveClock;
    U64 fromBB=(1ULL<<from);
    U64 toBB=(1ULL<<to);
    if (flag&(FLAG_CAPTURE)&&!(flag&FLAG_EN_PASSANT)) {
        board.piece[enemy][h.captured]&=~(toBB);
        board.occupied[enemy]&=~(toBB);
        board.zobristKey^=board.pieceKey[enemy][h.captured][to];
        d.mg   -= mg_table[enemy][h.captured][to];
        d.eg   -= eg_table[enemy][h.captured][to];
        d.phase -= gamephaseInc[h.captured];
    }
    board.mailbox[to]=piece;
    board.mailbox[from]=EMPTY;
    board.piece[color][piece]&=~fromBB;
    board.allOccupied&=~fromBB;
    board.occupied[color]&=~fromBB;
    board.piece[color][piece]|=toBB;
    board.allOccupied|=toBB;
    board.occupied[color]|=toBB;
    board.zobristKey^=board.pieceKey[color][piece][from];
    board.zobristKey^=board.pieceKey[color][piece][to];
    if (flag&(FLAG_EN_PASSANT)) {
        int back=(color==WHITE? -8:8);
        back+=to;
        h.captured=board.mailbox[back];
        board.mailbox[back]=EMPTY;
        h.captureSquare=back;
        U64 backBB=(1ULL<<back);
        board.allOccupied&=~backBB;
        board.piece[enemy][PAWN]&=~backBB;
        board.occupied[enemy]&=~backBB;
        board.zobristKey^=board.pieceKey[enemy][PAWN][back];
        d.mg   -= mg_table[enemy][PAWN][back];
        d.eg   -= eg_table[enemy][PAWN][back];
        d.phase -= gamephaseInc[PAWN];
    }
    if (flag&(FLAG_CASTLE)) {
        if (to>from) {
            int end=7;
            if (color) end=63;
            board.mailbox[end]=EMPTY;
            board.mailbox[end-2]=ROOK;
            U64 endBB=(1ULL<<end);
            U64 finBB=(1ULL<<(end-2));
            board.piece[color][ROOK]&=~endBB;
            board.piece[color][ROOK]|=finBB;
            board.allOccupied&=~endBB;
            board.allOccupied|=finBB;
            board.occupied[color]&=~endBB;
            board.occupied[color]|=finBB;
            board.zobristKey^=board.pieceKey[color][ROOK][end];
            board.zobristKey^=board.pieceKey[color][ROOK][end-2];
            d.mg   -= mg_table[color][ROOK][end];
            d.eg   -= eg_table[color][ROOK][end];
            d.mg   += mg_table[color][ROOK][end-2];
            d.eg   += eg_table[color][ROOK][end-2];
        }else {
            int end=0;
            if (color) end=56;
            board.mailbox[end]=EMPTY;
            board.mailbox[end+3]=ROOK;
            U64 endBB=(1ULL<<end);
            U64 finBB=(1ULL<<(end+3));
            board.piece[color][ROOK]&=~endBB;
            board.piece[color][ROOK]|=finBB;
            board.allOccupied&=~endBB;
            board.allOccupied|=finBB;
            board.occupied[color]&=~endBB;
            board.occupied[color]|=finBB;
            board.zobristKey^=board.pieceKey[color][ROOK][end];
            board.zobristKey^=board.pieceKey[color][ROOK][end+3];
            d.mg   -= mg_table[color][ROOK][end];
            d.eg   -= eg_table[color][ROOK][end];
            d.mg   += mg_table[color][ROOK][end+3];
            d.eg   += eg_table[color][ROOK][end+3];
        }
    }
    if (promo) {
        board.mailbox[to]=promo;
        board.piece[color][piece]&=~toBB;
        board.piece[color][promo]|=toBB;
        board.zobristKey^=board.pieceKey[color][piece][to];
        board.zobristKey^=board.pieceKey[color][promo][to];
        d.mg   -= mg_table[color][PAWN][to];
        d.eg   -= eg_table[color][PAWN][to];
        d.phase -= gamephaseInc[PAWN];

        d.mg   += mg_table[color][promo][to];
        d.eg   += eg_table[color][promo][to];
    }
    if (piece!=PAWN&&!(flag&(FLAG_CAPTURE)))
        board.halfMoveClock++;
    else
        board.halfMoveClock=0;
    board.zobristKey ^= board.castleKey[board.castlingRights];
    if (piece==KING) {
        if (color==WHITE)
            board.castlingRights&=12;
        else
            board.castlingRights&=3;
        board.kingPos[color]=to;
    }
    if (from==0||to==0)
        board.castlingRights&=13;
    if (from==7||to==7)
        board.castlingRights&=14;
    if (from==56||to==56)
        board.castlingRights&=7;
    if (from==63||to==63)
        board.castlingRights&=11;
    board.zobristKey^=board.castleKey[board.castlingRights];
    if (piece==PAWN&&abs(to/8-from/8)==2) {
        int back=(board.currentColor==WHITE? -8:8);
        board.enpassant=to+back;
    }else
        board.enpassant=-1;
    if (board.ep_is_capturable(board.enpassant,enemy))
        board.zobristKey^=board.enpassantFile[board.enpassant&7];
    h.d=d;
    mg_score += d.mg;
    eg_score += d.eg;
    phase   += d.phase;
    history[ply][historyCount[ply]++]=h;
    board.empty = ~board.allOccupied;
    board.zobristKey^=board.colorKey;
    board.currentColor=(Color)(1-board.currentColor);
}
inline void unmakeMove(Board& board,int ply) {
    History h=history[ply][--historyCount[ply]];
    Move move=h.move;
    int from=fromSquare(move);
    int to=toSquare(move);
    int promo=promotionPiece(move);
    int flag=moveFlags(move);
    int piece=board.mailbox[to];
    int enemy=board.currentColor;
    int color=1-enemy;
    board.mailbox[from]=board.mailbox[to];
    board.mailbox[to]=EMPTY;
    U64 fromBB=(1ULL<<from);
    U64 toBB=(1ULL<<to);
    board.piece[color][piece]&=~toBB;
    board.piece[color][piece]|=fromBB;
    board.allOccupied&=~toBB;
    board.allOccupied|=fromBB;
    board.occupied[color]&=~toBB;
    board.occupied[color]|=fromBB;
    if (h.captureSquare!=-1) {
        board.mailbox[h.captureSquare]=h.captured;
        U64 back=(1ULL<<h.captureSquare);
        board.piece[enemy][h.captured]|=back;
        board.allOccupied|=back;
        board.occupied[enemy]|=back;
    }
    board.castlingRights=h.castlingRights;
    board.enpassant=h.prevEnpassant;
    board.halfMoveClock=h.halfMoveClock;
    if (board.mailbox[from]==KING)
        board.kingPos[color]=from;
    if (flag&(FLAG_CASTLE)) {
        if (to>from) {
            int end=7;
            if (color) end=63;
            board.mailbox[end]=ROOK;
            board.mailbox[end-2]=EMPTY;
            U64 finBB=(1ULL<<end);
            U64 endBB=(1ULL<<(end-2));
            board.piece[color][ROOK]&=~endBB;
            board.piece[color][ROOK]|=finBB;
            board.allOccupied&=~endBB;
            board.allOccupied|=finBB;
            board.occupied[color]&=~endBB;
            board.occupied[color]|=finBB;
        }else {
            int end=0;
            if (color) end=56;
            board.mailbox[end]=ROOK;
            board.mailbox[end+3]=EMPTY;
            U64 finBB=(1ULL<<end);
            U64 endBB=(1ULL<<(end+3));
            board.piece[color][ROOK]&=~endBB;
            board.piece[color][ROOK]|=finBB;
            board.allOccupied&=~endBB;
            board.allOccupied|=finBB;
            board.occupied[color]&=~endBB;
            board.occupied[color]|=finBB;
        }
    }
    if (promo) {
        board.piece[color][piece]&=~fromBB;
        board.mailbox[from]=PAWN;
        board.piece[color][PAWN]|=fromBB;
    }
    board.empty=~board.allOccupied;
    board.zobristKey=h.zobristKey;
    board.currentColor=(Color)(1-board.currentColor);
    mg_score -= h.d.mg;
    eg_score -= h.d.eg;
    phase   -= h.d.phase;
}
inline void addMove(Board& board,int from,int to,int promo,bool castle,bool enpassant,int ply) {
    int flag=0;
    if (castle)
        flag|=FLAG_CASTLE;
    if (enpassant) {
        flag|=FLAG_EN_PASSANT;
        flag|=FLAG_CAPTURE;
    }
    if (promo)
        flag|=FLAG_PROMOTION;
    int pieceTaken=board.mailbox[to];
    int color=board.currentColor;
    if (pieceTaken!=EMPTY) {
        flag|=FLAG_CAPTURE;
    }
    Move move=make_move(from,to,promo,flag);
    makeMove(board,move,ply);

    if (!isAttacked(board,color,board.kingPos[color])) {
        moves[ply][moveCount[ply]++]=move;
    }
    unmakeMove(board,ply);
}
#endif //MOVE_H
