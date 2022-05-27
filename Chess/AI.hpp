#pragma once
#include <cstdint>
#include "Engine.hpp"

typedef int Evaluation;

#define MAX_EVALUATION_WHITE	 100000000
#define MAX_EVALUATION_BLACK	-100000000

Evaluation evaluatePosition(GameState state);
Evaluation evaluatePiece(int x, int y, Square square, int endgame);
Evaluation evaluatePositionAfterMove(GameState state, Move move);

Move greedyAI(GameState state);
Move treeAI(GameState state, int depth);

extern Evaluation overallEvaluation;
