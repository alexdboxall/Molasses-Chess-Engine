#include "Transposition.hpp"
#include <cassert>

TranspositionTable::TranspositionTable()
{
	table = new std::vector<TranspositionEntry>[TRANSPOSITION_TABLE_SIZE];

	for (int i = 0; i < TRANSPOSITION_TABLE_SIZE; ++i) {
		table[i] = std::vector<TranspositionEntry>();
	}
}

bool TranspositionTable::contains(Hash hash)
{
	Hash index = hash & TRANSPOSITION_TABLE_SIZE_MASK;
	
	for (const auto& entry : table[index]) {
		if (entry.hash == hash) {
			return true;
		}
	}

	return false;
}

TranspositionEntry TranspositionTable::get(Hash hash)
{
	Hash index = hash & TRANSPOSITION_TABLE_SIZE_MASK;

	for (const auto& entry : table[index]) {
		if (entry.hash == hash) {
			return entry;
		}
	}

	assert(false);
}

void TranspositionTable::set(Hash hash, Evaluation eval, int depth)
{
	Hash index = hash & TRANSPOSITION_TABLE_SIZE_MASK;

	for (auto& entry : table[index]) {
		if (entry.hash == hash) {
			entry.evaluation = eval;
			entry.depth = depth;
			return;
		}
	}

	table[index].push_back(TranspositionEntry(hash, eval, depth));
}