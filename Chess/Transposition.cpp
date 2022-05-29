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


Evaluation TranspositionTable::getReorderingValue(Move m, GameState s, int depthWanted)
{
	s.makeMove(m);
	Hash hash = s.genZobristHash();

	Hash index = hash & TRANSPOSITION_TABLE_SIZE_MASK;

	for (const auto& entry : table[index]) {
		if (entry.hash == hash && entry.depth >= depthWanted) {
			return entry.evaluation;
		}
	}

	return 0;
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

void TranspositionTable::incrementMove()
{
	++move;
	if (entries >= ENTRY_LIMIT) {
		++cutoffMove;
	}
	printf("move: %d, cutoff: %d. %d added, %d replaced\n", move, cutoffMove, rAdded, rReplaced);
	rAdded = 0;
	rReplaced = 0;
}

void TranspositionTable::set(Hash hash, Evaluation eval, int depth, TranspositionType type, Move mv)
{
	Hash index = hash & TRANSPOSITION_TABLE_SIZE_MASK;

	TranspositionEntry* replacementEntry = nullptr;
	for (auto& entry : table[index]) {
		if (entry.hash == hash) {
			entry.evaluation = eval;
			entry.depth = depth;
			entry.move = move;
			entry.type = (int) type;
			entry.actualMove = mv;
			return;
		}

		if (entry.move < cutoffMove && (replacementEntry == nullptr || entry.depth < replacementEntry->depth)) {
			replacementEntry = &entry;
		}
	}

	if (entries < ENTRY_LIMIT || replacementEntry == nullptr) {
		table[index].push_back(TranspositionEntry(hash, eval, depth, move, type, mv));
		++entries;
		++rAdded;
	} else {
		*replacementEntry = TranspositionEntry(hash, eval, depth, move, type, mv);
		++rReplaced;
	}
}