#pragma once

#include "Piece.hpp"
#include <vector>
#include <iostream>
#include <tuple>

typedef Square Board[8][8];
typedef uint64_t Hash;
typedef std::pair<int, int> Coordinate;
typedef std::tuple<Coordinate, Coordinate, Piece> Move;

enum class Outcome
{
	InProgress,
	WhiteWon,
	BlackWon,
	DrawByStalemate,
	DrawBy75Moves,
};

struct History
{
	Board board;

	bool enPassantPossible;
	int enPassantFile;

	bool whiteLongCastlePossible;
	bool whiteShortCastlePossible;
	bool blackLongCastlePossible;
	bool blackShortCastlePossible;
};

bool isLocationValid(int x, int y);
Player togglePlayer(Player player);
bool allowsEnPassant(Square sq, int srcRank, int dstRank);
bool isLongCastle(Square src, int sx, int dx, int dy);
bool isShortCastle(Square src, int sx, int dx, int dy);
bool isFirstRankPawn(Square sq, int rank);

void engineInit();

class GameState
{
private:
	std::vector<Coordinate> getPawnVision(Square square, int x, int y);
	std::vector<Coordinate> getKnightVision(Square square, int x, int y);
	std::vector<Coordinate> getKingVision(Square square, int x, int y);
	std::vector<Coordinate> getSlidingVision(Square square, int x, int y, int startIndex, int endIndex);

	std::vector<Move> getPseudoLegalMoves(bool includeKing = true);
	bool inCheckRightNow(Player player);

public:
	bool whiteTurn;

	Coordinate whiteKingPosition;
	Coordinate blackKingPosition;

	int turnsSincePawnMoveOrCapture;

	Outcome outcome;
	Board board;
	
	bool enPassantPossible;
	int enPassantFile;

	bool whiteLongCastlePossible;
	bool whiteShortCastlePossible;
	bool blackLongCastlePossible;
	bool blackShortCastlePossible;
		
	//std::vector<History> history;			//needed to detect three/five fold repetition

	GameState();

	Hash genZobristHash();

	std::vector<Move> getLegalMoves();

	Coordinate findKing(Player player);
	void setKingPosition(int x, int y, Player player);

	bool inCheckAfterMove(Player player, int sx, int sy, int dx, int dy, Piece promotionType);
	bool isSquareUnderAttack(int x, int y);

	void updateHistory();
	void nextTurn();
	void checkForGameOver();
	Square getSquare(int x, int y);
	void makeMoveWithoutLegalityCheck(int sx, int sy, int dx, int dy, Piece promotionType = Piece::DontCare);
	void makeMove(int sx, int sy, int dx, int dy, Piece promotionType = Piece::DontCare);
	void makeMove(Move move);
	void preventShortCastleForPlayer(Player p);
	void preventLongCastleForPlayer(Player p);

	bool isSamePlayer(Square a, int x, int y);
	bool isOtherPlayer(Square a, int x, int y);
	bool isCapturable(Square a, int x, int y);
	bool isEmptySquare(int x, int y);

	std::vector<Coordinate> getPieceVision(int x, int y);
};