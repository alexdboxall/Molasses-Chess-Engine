
#include "AI.hpp"
#include <cassert>
#include <ctime>
#include <map>
#include <unordered_map>

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

struct Transposition
{
	Evaluation eval;
	int depth;

	Transposition()
	{

	}

	Transposition(Evaluation eval_, int depth_)
	{
		eval = eval_;
		depth = depth_;
	}
};

std::unordered_map<Hash, Transposition> transpositionTable;

Evaluation evaluatePositionNode(GameState state, int lookahead, int alpha, int beta)
{
	Hash hash = state.genZobristHash();

	int realLookahead = lookahead;

	if (transpositionTable.count(hash)) {
		Transposition transposition = transpositionTable[hash];

		if (transposition.depth >= lookahead) {
			++transpositionsUsed;
			return transposition.eval;
		}
	}

	if (lookahead == 0) {
		Evaluation eval = evaluatePosition(state);
		transpositionTable[hash] = Transposition(eval, 0);
		return eval;
	}

	auto moves = state.getLegalMoves();
	Evaluation bestEval = state.whiteTurn ? MAX_EVALUATION_BLACK : MAX_EVALUATION_WHITE;

	for (const Move& move : moves) {
		GameState stateCopy = state;
		stateCopy.makeMove(move);

		Evaluation evaluation = evaluatePositionNode(stateCopy, lookahead - 1, alpha, beta);
		if (minOrMax(evaluation, bestEval, state.whiteTurn)) {
			bestEval = evaluation;
		}

		if (state.whiteTurn && evaluation > alpha) alpha = evaluation;
		if (!state.whiteTurn && evaluation < beta) beta = evaluation;

		if (beta <= alpha) break;
	}

	transpositionTable[hash] = Transposition(bestEval, realLookahead);

	return bestEval;
}

Evaluation overallEvaluation = 0;

Move treeAI(GameState state, int lookahead) {
	time_t t = clock();

	positionsEvaluated = 0;
	transpositionsUsed = 0;

	auto moves = state.getLegalMoves();

	Move bestMove = moves[0];
	Evaluation bestEval = state.whiteTurn ? MAX_EVALUATION_BLACK : MAX_EVALUATION_WHITE;

	Evaluation alpha = MAX_EVALUATION_BLACK - 1000;
	Evaluation beta  = MAX_EVALUATION_WHITE + 1000;

	for (const Move& move : moves) {
		GameState stateCopy = state;
		stateCopy.makeMove(move);
		Evaluation evaluation = evaluatePositionNode(stateCopy, lookahead, alpha, beta);

		if (minOrMax(evaluation, bestEval, state.whiteTurn)) {
			bestMove = move;
			bestEval = evaluation;

			if (lookahead >= 4) overallEvaluation = (bestEval + overallEvaluation) / 2;
		}

		if (state.whiteTurn  && evaluation > alpha) alpha = evaluation;
		if (!state.whiteTurn && evaluation < beta ) beta  = evaluation;

		if (beta <= alpha) break;
	}
				
	if (lookahead >= 3) overallEvaluation = bestEval;

	time_t diff = clock() - t;
	double secs = (double) diff / (double) CLOCKS_PER_SEC;
	printf("Evaluated %d positions in %.1f secs. (%d saved by the table)\n", positionsEvaluated, secs, transpositionsUsed);
	
	return bestMove;
}

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