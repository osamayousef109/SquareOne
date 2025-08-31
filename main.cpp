#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <atomic>
#include <chrono>

// --- Your Engine's Headers ---
// Include the headers for your chess engine's core components.
#include "attacks.h"
#include "bitboards.h"
#include "evaluation.h"
#include "play.h" // Assuming your search/play logic is here

// --- Forward Declarations & Globals ---

// A global board object. Most UCI engines have one.
Board board;

// A flag to control the search thread.

// --- Function Prototypes for UCI ---
void uciLoop();
void parsePosition(std::istringstream& is);
void parseGo(std::istringstream& is);

// --- Main Function ---
// This replaces your current main function.
int main() {
    // --- Your Engine's Initialization ---
    // Initialize all your engine's components here, as you did before.
    initAttacks();
    initMagic();
    board.initZobrist(); // Assuming board needs Zobrist keys
    init_eval(board);    // Assuming evaluation needs initialization

    // Start the main UCI communication loop.
    uciLoop();

    // If a search is running when we quit, we should wait for it to finish.
    if (searchThread.joinable()) {
        isSearching = false;
        searchThread.join();
    }

    return 0;
}

// --- UCI Communication Loop ---
// This function handles all communication with the GUI.
void uciLoop() {
    std::string line;
    std::cout.setf(std::ios::unitbuf); // Make sure output is sent immediately

    while (std::getline(std::cin, line)) {
        std::istringstream is(line);
        std::string token;
        is >> token;

        if (token == "uci") {
            // Respond with engine identification and options.
            std::cout << "id name Chussy" << std::endl;
            std::cout << "id author Unicticon" << std::endl;
            // Add any UCI options your engine supports here.
            std::cout << "uciok" << std::endl;
        } else if (token == "isready") {
            // The GUI sends this to check if the engine is ready for commands.
            std::cout << "readyok" << std::endl;
        } else if (token == "ucinewgame") {
            board=Board{};
            board.initZobrist();
            init_eval(board);
        } else if (token == "position") {
            parsePosition(is);
        } else if (token == "go") {
            parseGo(is);
        } else if (token == "stop") {
            // Stop the search and return the best move found so far.
            if (isSearching) {
                isSearching = false;
                if(searchThread.joinable()) {
                    searchThread.join();
                }
            }
        } else if (token == "quit") {
            // The GUI is closing.
            if (isSearching) {
                isSearching = false;
                 if(searchThread.joinable()) {
                    searchThread.join();
                }
            }
            break;
        }
    }
}
int promoCharToPiece(char c) {
    c = tolower(c);
    if (c == 'q') return QUEEN;
    if (c == 'r') return ROOK;
    if (c == 'b') return BISHOP;
    if (c == 'n') return KNIGHT;
    return 0;
}
// Parses the "position" command
void parsePosition(std::istringstream& is) {
    std::string token, fen;
    is >> token;

    if (token == "startpos") {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        is >> token; // Consume "moves" token if it exists
    } else if (token == "fen") {
        while (is >> token && token != "moves") {
            fen += token + " ";
        }
    }
    board=Board{};
    board.setFen(fen);
    board.initZobrist();
    init_eval(board);

    // Parse the moves list that follows the position setup.
    if (token == "moves") {
        while (is >> token) {
            int fromFile = token[0] - 'a';
            int fromRank = token[1] - '1';
            int fromSq = fromRank * 8 + fromFile;

            int toFile = token[2] - 'a';
            int toRank = token[3] - '1';
            int toSq = toRank * 8 + toFile;

            int promotion=0;
            if (token.length() == 5) {
                promotion = promoCharToPiece(token[4]);
            }
            generateMoves(board,0);
            for (int i=0;i<moveCount[0];i++) {
                Move move=moves[0][i];
                int from=fromSquare(move);
                int to=toSquare(move);
                int promo=promotionPiece(move);
                if (from==fromSq&&to==toSq&&promo==promotion) {
                    makeMove(board,move,0);
                    break;
                }
            }
        }
    }
}
// Parses the "go" command
void parseGo(std::istringstream& is) {
    if (isSearching) {
        isSearching = false;
        if (searchThread.joinable()) searchThread.join();
    }
    if (searchThread.joinable()) searchThread.join();
    SearchLimits limits;
    std::string token;
    while (is >> token) {
        if (token == "wtime") is >> limits.wtime;
        else if (token == "btime") is >> limits.btime;
        else if (token == "winc") is >> limits.winc;
        else if (token == "binc") is >> limits.binc;
        else if (token == "movestogo") is >> limits.movestogo;
        else if (token == "depth") is >> limits.depth;
        else if (token == "nodes") is >> limits.nodes;
        else if (token == "movetime") is >> limits.movetime;
        else if (token == "infinite") limits.infinite = true;
        else if (token == "ponder") limits.ponder = true;
    }

    // start a thread with a copy of board to avoid concurrent modifications
    isSearching = true;
    searchThread = std::thread([limits]() {
        // copy the board
        Board searchBoard = board;

        // Ensure your search function takes limits and reads isSearching to stop.
        // Example API: Move search(Board&, SearchLimits&)
        std::string best = play(searchBoard, limits); // adapt to your API

        // When search finishes (or was stopped), print bestmove
        std::cout << "bestmove " << best << std::endl;
        isSearching = false;
    });
}