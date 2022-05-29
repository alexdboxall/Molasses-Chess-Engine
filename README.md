# Molasses Chess Engine and AI
A chess engine and AI written in C++ and uses SDL2 for the interface. The AI uses alpha-beta pruning, transposition tables, and iterative deepening. Check detection is currently very slow, and therefore has been disabled for now.

![Chess](https://github.com/alexdboxall/Molasses-Chess-Engine/blob/master/icons/documentation_img.png "Chess")

The alpha-beta pruning logic is based on the implementation in Sebastian Lague's chess AI (https://github.com/SebLague/Chess-AI), under the MIT license (see `AI.cpp` for the full license.)