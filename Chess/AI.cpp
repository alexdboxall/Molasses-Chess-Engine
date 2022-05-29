
#include "AI.hpp"
#include <cassert>
#include <ctime>
#include <algorithm>

#include "Transposition.hpp"

#define PAWN_VALUE		100
#define KNIGHT_VALUE	300
#define BISHOP_VALUE	330
#define ROOK_VALUE		500
#define QUEEN_VALUE		900
#define KING_VALUE		75000

int knightMap[8][8] = {
	{0, 1, 2, 2, 2, 2, 1, 0},
	{1, 2, 3, 3, 3, 3, 2, 1},
	{2, 3, 4, 5, 5, 4, 3, 2},
	{2, 3, 5, 6, 6, 5, 3, 2},
	{2, 3, 5, 6, 6, 5, 3, 2},
	{2, 3, 4, 5, 5, 4, 3, 2},
	{1, 2, 3, 3, 3, 3, 2, 1},
	{0, 1, 2, 2, 2, 2, 1, 0},
};

int rookMap[8][8] = {
	{1, 1, 2, 3, 3, 2, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 0},
	{3, 6, 6, 6, 6, 6, 6, 3},
	{2, 2, 2, 2, 2, 2, 2, 2},
}; 

int pawnMap[8][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{3, 4, 4, 0, 0, 4, 4, 3},
	{3, 2, 1, 2, 2, 1, 2, 3},
	{2, 2, 2, 4, 4, 2, 2, 2},
	{3, 3, 4, 6, 6, 4, 3, 3},
	{4, 4, 5, 6, 6, 5, 4, 4},
	{9, 9, 9, 9, 9, 9, 9, 9},
	{9, 9, 9, 9, 9, 9, 9, 9},
};

int bishopMap[8][8] = {
	{2, 1, 1, 1, 1, 1, 1, 2},
	{1, 4, 2, 2, 2, 2, 4, 1},
	{1, 4, 4, 4, 4, 4, 4, 1},
	{1, 2, 5, 4, 4, 5, 2, 1},
	{1, 3, 3, 4, 4, 3, 3, 1},
	{1, 2, 3, 4, 4, 3, 2, 1},
	{1, 2, 2, 2, 2, 2, 2, 1},
	{0, 1, 1, 1, 1, 1, 1, 0},
};

int queenMap[8][8] = {
	{0, 2, 2, 3, 3, 2, 2, 0},
	{2, 4, 4, 4, 4, 4, 4, 2},
	{2, 4, 6, 6, 6, 6, 4, 2},
	{3, 4, 6, 6, 6, 6, 4, 3},
	{3, 4, 6, 6, 6, 6, 4, 3},
	{2, 4, 6, 6, 6, 6, 4, 2},
	{2, 4, 4, 4, 4, 4, 4, 2},
	{0, 2, 2, 3, 3, 2, 2, 0},
};

// DON'T USE IN THE ENDGAME
int kingMap[8][8] = {
	{6, 7, 5, 5, 5, 5, 7, 6},
	{5, 4, 3, 3, 3, 3, 4, 5},
	{3, 2, 2, 2, 2, 2, 2, 3},
	{2, 2, 2, 1, 1, 2, 2, 2},
	{1, 1, 1, 0, 0, 1, 1, 1},
	{1, 1, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 1, 1},
};

Evaluation evaluatePiece(int x, int y, Square square, int endgame)
{
	Evaluation baseScore = 0;
	
	int ly = square.player == Player::White ? y : 7 - y;

	switch (square.piece) {
	case Piece::Pawn:
		baseScore = PAWN_VALUE + endgame * 5 + pawnMap[ly][x] * 8;
		break;
	case Piece::Knight: 
		baseScore = KNIGHT_VALUE + knightMap[ly][x] * 8;
		break;
	case Piece::Bishop: 
		baseScore = BISHOP_VALUE + bishopMap[ly][x] * 8;
		break;
	case Piece::Rook:	
		baseScore = ROOK_VALUE + rookMap[ly][x] * 8;
		break;
	case Piece::Queen:	
		baseScore = QUEEN_VALUE	+ queenMap[ly][x] * 8;
		break;
	case Piece::King:	
		baseScore = KING_VALUE + kingMap[ly][x] * (10 - endgame);
		break;
	default:
		assert(false);
	}

	return baseScore;
}

int positionsEvaluated = 0;
int transpositionsUsed = 0;

Evaluation evaluatePosition(GameState state)
{
	Evaluation piecewiseEvaluation = 0;

	// a value between 0 and 10
	int endgame = 0;

	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			Square square = state.board[y][x];

			if (square.player == Player::Empty || square.piece == Piece::DontCare) {
				continue;
			}

			Evaluation pieceEval = evaluatePiece(x, y, square, endgame);

			piecewiseEvaluation += (square.player == Player::White) ? pieceEval : -pieceEval;
		}
	}

	// bishop pairs

	// king safety

	// protected pawns

	// isolated pawns

	// passed pawns

	++positionsEvaluated;

	return piecewiseEvaluation;
}

Evaluation evaluatePositionAfterMove(GameState state, Move move)
{
	auto s = std::get<0>(move);
	auto d = std::get<1>(move);
	
	state.makeMoveWithoutLegalityCheck(s.first, s.second, d.first, d.second, std::get<2>(move));

	return evaluatePosition(state);
}

bool minOrMax(Evaluation newValue, Evaluation currentBest, bool whiteTurn)
{
	if (whiteTurn) {
		return newValue > currentBest;
	} else {
		return newValue < currentBest;
	}
}

TranspositionTable transpositionTable;


/* BEGIN DERIVATE CODE
* 
* The following code is heavy inspired by Sebastian Lague's Chess-AI
* 
* https://github.com/SebLague/Chess-AI
* 

MIT License

Copyright (c) 2021 Sebastian Lague

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

Evaluation bestEvalThisIteration;
Move bestMoveThisIteration;

Evaluation evaluatePositionNode(GameState state, int lookahead, int alpha, int beta, int depth)
{
	if (stopAI) {
		return alpha;
	}

	Hash hash = state.genZobristHash();

	int realLookahead = lookahead;

	if (transpositionTable.contains(hash)) {
		TranspositionEntry transposition = transpositionTable.get(hash);

		if (transposition.depth >= lookahead) {
			if (transposition.type == (int) TranspositionType::Exact) {
				++transpositionsUsed;
				return transposition.evaluation;
			
			} else if (transposition.type == (int) TranspositionType::MaxBound && transposition.evaluation <= alpha) {
				// the stored eval is 'beta', so we can abort searching if beta <= alpha
				//printf("BETA transpos. used.\n");
				++transpositionsUsed;
				return transposition.evaluation;

			} else if (transposition.type == (int) TranspositionType::MinBound && transposition.evaluation >= beta) {
				// the stored eval is 'alpha', so we can abort searching if alpha >= beta
				//printf("ALPHA transpos. used.\n");
				++transpositionsUsed;
				return transposition.evaluation;
				
			}
		}
	}

	if (lookahead == 0) {
		Evaluation eval = evaluatePosition(state);
		//transpositionTable.set(hash, eval, 0, TranspositionType::Exact);
		return state.whiteTurn ? eval : -eval;
	}


	auto moves = state.getLegalMoves();
	/*if (moves.size() > 2) {
		int partialSortLength = moves.size() > 4 ? 3 : 1;
		std::partial_sort(moves.begin(), moves.begin() + 3, moves.end(),
			[state, lookahead](Move& a, Move& b) { return abs(transpositionTable.getReorderingValue(a, state, 0)) > abs(transpositionTable.getReorderingValue(b, state, 0)); }
		);
	}*/

	Evaluation bestEval = state.whiteTurn ? MAX_EVALUATION_BLACK : MAX_EVALUATION_WHITE;

	TranspositionType transType = TranspositionType::MaxBound;
	Move bestMoveHere;
	bool foundAMove = false;

	for (const Move& move : moves) {
		GameState stateCopy = state;
		stateCopy.makeMove(move);

		Evaluation evaluation = -evaluatePositionNode(stateCopy, lookahead - 1, -beta, -alpha, depth + 1);
		
		if (evaluation >= beta) {
			transpositionTable.set(hash, beta, lookahead, TranspositionType::MinBound, move);
			return beta;
		}

		if (evaluation > alpha) {
			transType = TranspositionType::Exact;
			bestMoveHere = move;
			foundAMove = true;

			alpha = evaluation;
			if (depth == 0) {
				bestMoveThisIteration = move;
				bestEvalThisIteration = evaluation;
			}
		}

	}

	if (foundAMove) {
		transpositionTable.set(hash, alpha, realLookahead, transType, bestMoveHere);
	}

	return alpha;
}

bool stopAI = false;

Move treeAI(GameState state, int lookahead, bool firstOnMove) {
	if (firstOnMove) {
		transpositionTable.incrementMove();
	}
	time_t t = clock();

	positionsEvaluated = 0;
	transpositionsUsed = 0;

	auto moves = state.getLegalMoves();

	bestMoveThisIteration = moves[0];
	bestEvalThisIteration = evaluatePositionAfterMove(state, bestMoveThisIteration);

	evaluatePositionNode(state, lookahead, MAX_EVALUATION_BLACK - 1000, MAX_EVALUATION_WHITE + 1000, 0);

	time_t diff = clock() - t;
	double secs = (double) diff / (double) CLOCKS_PER_SEC;
	
	if (!stopAI) printf("Evaluated %d positions in %.1f secs. (%d saved by the table)\n", positionsEvaluated, secs, transpositionsUsed);
	else printf("Timeout.\n");

	return bestMoveThisIteration;
}

/* END DERIVATE CODE */

Move greedyAI(GameState state)
{
	auto moves = state.getLegalMoves();

	Move bestMove = moves[0];
	Evaluation bestEvaluation = evaluatePositionAfterMove(state, bestMove);

	for (const Move& move : moves) {
		Evaluation evaluation = evaluatePositionAfterMove(state, move);

		if (minOrMax(evaluation, bestEvaluation, state.whiteTurn)) {
			bestMove = move;
			bestEvaluation = evaluation;
		}
	}

	return bestMove;
}
