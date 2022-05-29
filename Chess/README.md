# Molasses Chess Engine and AI
A chess engine and AI written in C++ and uses SDL2 for the interface. The AI uses alpha-beta pruning, transposition tables, and iterative deepening. Check detection is currently very slow, and therefore has been disabled for now.

![Chess](icons/documentation_img.png "Chess")

The alpha-beta pruning logic used in `AI.cpp` is based on its implementation in Sebastian Lague's chess AI (https://github.com/SebLague/Chess-AI), under the MIT license (see `AI.hpp` for the full license.)