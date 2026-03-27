<div align="center">

# ♟️ SquareOne

**A High-Performance, UCI-Compliant Chess Engine written in modern C++**

[![C++](https://img.shields.io/badge/C++-17%2B-blue.svg)](https://isocpp.org/)
[![UCI Supported](https://img.shields.io/badge/Protocol-UCI-green.svg)]()
[![Lichess Bot](https://img.shields.io/badge/Lichess-2000%2B%20Elo-black.svg)](https://lichess.org/)

</div>

Welcome to **SquareOne**, a strong, open-source chess engine built from scratch in C++. Designed for speed and efficiency, the engine utilizes a highly optimized bitboard representation and advanced search heuristics to achieve playing strengths exceeding 2000 Elo. 

It is fully compatible with the Universal Chess Interface (UCI) protocol, allowing it to be seamlessly integrated into popular chess GUIs (like Arena or Cute Chess) or run as an automated bot on Lichess.

---

## 🚀 Key Features & Performance

- **UCI Compliant:** Ready to plug and play with standard chess graphical interfaces.
- **Blazing Fast Move Generation:** Capable of searching up to **30 Million nodes per second** on modern hardware.
- **Lichess API Integration:** Includes support/scripts for running as an automated bot on the Lichess platform.
- **Estimated Strength:** 2000+ Elo in rapid/blitz time controls.

---

## 🧠 Engine Architecture

The engine is built on several core optimizations standard in modern computer chess:

### Board Representation
- **Bitboards:** The board state is represented using 64-bit integers (bitboards), allowing for parallelized bitwise operations to calculate move pseudo-legality and board occupancy instantly.
- **Magic Bitboards:** Utilizes magic bitboard hashing for ultra-fast sliding piece (Rook, Bishop, Queen) attack generation.

### Search Algorithm
The decision-making core relies on the **Negamax framework** combined with highly tuned search extensions and reductions:
- **Alpha-Beta Pruning:** Core optimization to eliminate branches that cannot yield a better outcome.
- **Iterative Deepening:** Searches progressively deeper, ensuring the engine always has a viable move to play if time runs out, while improving move ordering for subsequent depths.
- **Null-Move Pruning (NMP):** Aggressively prunes branches where passing a turn still results in a strong position (beta cutoff), vastly increasing search depth.
- **Late Move Reductions (LMR):** Searches moves considered "less likely" to be good at a shallower depth to save time for tactical lines.

### Evaluation
- **PeSTo's evaluation tables, Tapered Eval and a custom Hand-Crafted Evaluation function focusing on material, mobility, king safety, and pawn structure**
---

## ⚙️ Building the Engine

### Prerequisites
- A modern C++ compiler supporting C++17 or higher (GCC, Clang, MSVC).
- **CMake** (v3.15+)

### Compilation
Clone the repository and build using CMake:

    git clone https://github.com/osamayousef109/Chussy
    cd Chussy
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release

> **Pro-Tip:** Always ensure you compile the engine in `Release` mode (`-O3` flag). Compiling in `Debug` mode will drastically reduce the node-per-second count and overall playing strength.

---

## 🎮 Usage

### Using a Chess GUI (Recommended)
Because this is a UCI engine, it does not come with its own graphical board. To play against it:
1. Download a GUI like [Cute Chess](https://cutechess.com/) or [Arena Chess](http://www.playwitharena.de/).
2. Go to Engine Management > Add New Engine.
3. Select the compiled `.exe` (or binary) you just built.

### Running as a Lichess Bot
To connect the engine to Lichess, you will need the `lichess-bot` bridge.
1. Create a Bot account on Lichess.
2. Clone the official [lichess-bot repository](https://github.com/lichess-bot-lichess-bot).
3. Point the configuration `config.yml` file to your compiled engine executable.

---

## 🤝 Acknowledgements

- **Chess Programming Wiki:** An invaluable resource for understanding bitboards and search heuristics.
- **Lichess API:** For providing an excellent platform for bot testing and matchmaking.
