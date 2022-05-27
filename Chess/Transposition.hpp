#pragma once

#include "Engine.hpp"
#include "AI.hpp"
#include <vector>

// light : 4 million
// normal: 16 or 32 million
// hog	 : 64 million

// must be a power of 2
#define TRANSPOSITION_TABLE_SIZE 1024 * 1024 * 32
#define TRANSPOSITION_TABLE_SIZE_MASK (TRANSPOSITION_TABLE_SIZE - 1)

struct TranspositionEntry
{
	Hash hash;
	Evaluation evaluation;
	uint8_t depth;

	TranspositionEntry()
	{

	}

	TranspositionEntry(Hash hash_, Evaluation eval, int depth_)
	{
		hash = hash_;
		evaluation = eval;
		depth = depth_;
	}
};

class TranspositionTable
{
	std::vector<TranspositionEntry>* table;

public:
	TranspositionTable();
	bool contains(Hash hash);

	void set(Hash hask, Evaluation eval, int depth);
	TranspositionEntry get(Hash hask);
};
