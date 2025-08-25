//
// Created by MSi on 8/23/2025.
//

#ifndef TT_H
#define TT_H
#include <climits>

#include "types.h"

struct PerftEntry {
    U64 key;
    int depth;
    long long nodes;
};
enum Node{EXACT,LOWER_BOUND,UPPER_BOUND};
struct Entry {
    U64 key;
    Move bestMove;
    uint8_t depth;
    int score;
    uint8_t type;
    uint16_t age;
};
constexpr int TT_SIZE=(1<<20);
inline PerftEntry perftTable[TT_SIZE];
inline Entry dp[TT_SIZE][4];

inline bool probePerftTT(U64 key, int depth, long long& nodes) {
    PerftEntry& e = perftTable[key & (TT_SIZE - 1)];
    if (e.key == key && e.depth == depth) {
        nodes = e.nodes;
        return true;
    }
    return false;
}

inline void storePerftTT(U64 key, int depth, long long nodes) {
    PerftEntry& e = perftTable[key & (TT_SIZE - 1)];
    e.key = key;
    e.depth = depth;
    e.nodes = nodes;
}

inline bool probeTT(U64 key,int depth,int alpha,int beta,int& score,Move& bestMove){
    int idx=(int)(key&(TT_SIZE-1));
    for (int i=0;i<4;i++) {
        Entry& e=dp[idx][i];
        if (e.key==key) {
            bestMove=e.bestMove;
            if (e.depth>=depth) {
                int val=e.score;
                if (e.type==EXACT) {
                    score=val;
                    return true;
                } else if(e.type==LOWER_BOUND&&val>=beta) {
                    score=val;
                    return true;
                } else if (e.type==UPPER_BOUND&&val<=alpha) {
                    score=val;
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

inline void storeTT(U64 key,Move bestMove,uint8_t depth,int score,uint8_t type) {
    int idx=(int)(key&(TT_SIZE-1));
    Entry entry{key,bestMove,depth,score,type,generation};
    int oldest=-1;
    int age=generation;
    int deep=INT_MAX;
    int rep=-1;
    for (int i=0;i<4;i++) {
        Entry &old=dp[idx][i];
        if (old.key!=key) {
            if (old.age==0) {
                old=entry;
                return;
            }
            if (old.age<age) {
                oldest=i;
                age=old.age;
            }
            if (old.depth<deep) {
                deep=old.depth;
                rep=i;
            }
        }else {
            if (old.depth<=depth)
                old=entry;
            return;
        }
    }
    if (oldest!=-1) {
        dp[idx][oldest]=entry;
        return;
    }
    dp[idx][rep]=entry;
}
#endif //TT_H
