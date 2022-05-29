#pragma once

#include "Engine.hpp"
#include "AI.hpp"
#include <vector>

// must be a power of 2
#define TRANSPOSITION_TABLE_SIZE 1024 * 1024 * 4
#define TRANSPOSITION_TABLE_SIZE_MASK (TRANSPOSITION_TABLE_SIZE - 1)

#define ENTRY_LIMIT	1024 * 1024 * 32

enum class TranspositionType : uint8_t
{
	Exact,
	MinBound,
	MaxBound,
};

struct TranspositionEntry
{
	Hash hash;
	Evaluation evaluation;
	uint16_t depth : 14;
	uint16_t type  : 2;
	uint16_t move;
	Move actualMove;

	TranspositionEntry()
	{

	}

	TranspositionEntry(Hash hash_, Evaluation eval, int depth_, int move_, TranspositionType type_, Move actualMove_)
	{
		hash = hash_;
		evaluation = eval;
		depth = depth_;
		move = move_;
		type = (int) type_;
		actualMove = actualMove_;
	}
};

class TranspositionTable
{
	std::vector<TranspositionEntry>* table;
	int move = 0;
	int cutoffMove = 0;
	int entries = 0;
	int moveOfLastCutoffChange = 0;

	int rAdded = 0;
	int rReplaced = 0;
public:
	void incrementMove();

	TranspositionTable();
	bool contains(Hash hash);

	void set(Hash hash, Evaluation eval, int depth, TranspositionType type, Move move);
	TranspositionEntry get(Hash hash);
	Evaluation getReorderingValue(Move m, GameState s, int depthWanted);
};
