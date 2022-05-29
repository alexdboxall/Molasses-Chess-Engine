
#include "Engine.hpp"
#include <random>

uint64_t pieceHashes[7][8][8][2];
uint64_t blackToMoveHash;
uint64_t enPassantFileHash[8];
uint64_t castlingHashes[4];			//WS, WL, BS, BL

void engineInit()
{
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<Hash> distr;

	blackToMoveHash = distr(eng);

	for (int i = 0; i < 8; ++i) {
		enPassantFileHash[i] = distr(eng);
	}
	for (int i = 0; i < 4; ++i) {
		castlingHashes[i] = distr(eng);
	}

	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			for (int p = 0; p < 7; ++p) {
				if (p == (int) Piece::DontCare) {
					pieceHashes[p][y][x][0] = 0;
					pieceHashes[p][y][x][1] = 0;
				} else {
					pieceHashes[p][y][x][0] = distr(eng);
					pieceHashes[p][y][x][1] = distr(eng);
				}
			}
		}
	}
}

GameState::GameState()
{
	whiteTurn = true;
	enPassantPossible = false;
	whiteLongCastlePossible = true;
	whiteShortCastlePossible = true;
	blackLongCastlePossible = true;
	blackShortCastlePossible = true;

	turnsSincePawnMoveOrCapture = 0;

	whiteKingPosition = Coordinate{ 4, 0 };
	blackKingPosition = Coordinate{ 4, 7 };

	outcome = Outcome::InProgress;

	for (int y = 2; y < 6; ++y) {
		for (int x = 0; x < 8; ++x) {
			board[y][x] = Square(Piece::DontCare, Player::Empty);
		}
	}

	for (int x = 0; x < 8; ++x) {
		board[1][x] = Square(Piece::Pawn, Player::White);
		board[6][x] = Square(Piece::Pawn, Player::Black);
	}

	const Piece pieceOrder[8] = {Piece::Rook, Piece::Knight, Piece::Bishop, Piece::Queen,
								  Piece::King, Piece::Bishop, Piece::Knight, Piece::Rook };

	for (int i = 0; i < 2; ++i) {
		for (int x = 0; x < 8; ++x) {
			board[i == 0 ? 0 : 7][x] = Square(pieceOrder[x], i == 0 ? Player::White : Player::Black);
		}
	}
}

void GameState::updateHistory()
{
	return;

	/*History h;

	h.enPassantFile = enPassantFile;
	h.enPassantPossible = enPassantPossible;
	h.blackLongCastlePossible = blackLongCastlePossible;
	h.blackShortCastlePossible = blackShortCastlePossible;
	h.whiteLongCastlePossible = whiteLongCastlePossible;
	h.whiteShortCastlePossible = whiteShortCastlePossible;

	memcpy(h.board, &board, sizeof(board));
	history.push_back(h);*/
}

void GameState::nextTurn()
{
	whiteTurn ^= true;
}

Hash GameState::genZobristHash()
{
	// we can optimise by not recomputing each time,
	// but instead XORing pieces in and out, but first we'll
	// make it work before we make it fast

	Hash hash = whiteTurn ? 0 : blackToMoveHash;
	if (enPassantPossible) {
		hash ^= enPassantFileHash[enPassantFile];
	}

	if (whiteShortCastlePossible) hash ^= castlingHashes[0];
	if (whiteLongCastlePossible)  hash ^= castlingHashes[1];
	if (blackShortCastlePossible) hash ^= castlingHashes[2];
	if (blackLongCastlePossible)  hash ^= castlingHashes[3];

	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			Square square = board[y][x];
			Player player = square.player;
			Piece piece = square.piece;

			if (player == Player::White) {
				hash ^= pieceHashes[(int) piece][y][x][0];

			} else if (player == Player::Black) {
				hash ^= pieceHashes[(int) piece][y][x][1];
			}
		}
	}

	return hash;
}

Square GameState::getSquare(int x, int y)
{
	if (isLocationValid(x, y)) {
		return board[y][x];
	}

	std::cerr << "Square coordinates out of bounds!";
	abort();
}

Coordinate GameState::findKing(Player player)
{
	if (player == Player::White) return whiteKingPosition;
	else return blackKingPosition;
}

bool GameState::inCheckRightNow(Player player)
{
	Coordinate kingLocation = findKing(player);
	return isSquareUnderAttack(kingLocation.first, kingLocation.second);
}

bool GameState::inCheckAfterMove(Player player, int sx, int sy, int dx, int dy, Piece promotionType)
{
	GameState stateCopy = GameState(*this);
	stateCopy.makeMoveWithoutLegalityCheck(sx, sy, dx, dy, promotionType);

	return stateCopy.inCheckRightNow(player);
}

bool GameState::isSquareUnderAttack(int x, int y)
{
	return false;

	if (!isLocationValid(x, y)) {
		return false;
	}

	Square square = getSquare(x, y);

	// stop recurision on king's vision by not checking the opponent's king (which will also
	// check if they are under attack)
	auto moves = getPseudoLegalMoves(false);
	for (const auto& move : moves) {
		Coordinate src = std::get<0>(move);
		Coordinate dst = std::get<1>(move);

		if (dst.first == x && dst.second == y) {
			return true;
		}
	}

	return false;
}

void GameState::makeMove(int sx, int sy, int dx, int dy, Piece promotionType)
{
	makeMoveWithoutLegalityCheck(sx, sy, dx, dy, promotionType);
	checkForGameOver();
}

void GameState::makeMove(Move move)
{
	auto s = std::get<0>(move);
	auto d = std::get<1>(move);

	makeMove(s.first, s.second, d.first, d.second, std::get<2>(move));
}

void GameState::setKingPosition(int x, int y, Player player)
{
	if (player == Player::White) whiteKingPosition = Coordinate{ x, y };
	else blackKingPosition = Coordinate{ x, y };
}

void GameState::makeMoveWithoutLegalityCheck(int sx, int sy, int dx, int dy, Piece promotionType)
{
	/*
	'Without legality check' means that the inputs are assumed to have taken into account
		- preventing self capturing
		- preventing jumping over/through pieces
		- out of bounds
		- moving empty pieces
		- castling rules (using the variables, enough room to do, not in/out/through check)
		- en passant possible (using the variables)
		- checks
	*/

	Square src = getSquare(sx, sy);
	Square dst = getSquare(dx, dy);

	bool capture = dst.player != Player::Empty;

	// perform the move
	if (isLongCastle(src, sx, dx, dy)) {
		setKingPosition(dx, dy, src.player);
		board[dy][dx] = src;
		board[sy][dx + 1] = getSquare(0, sy);
		board[sy][0]  = Square(Piece::DontCare, Player::Empty);
		board[sy][sx]  = Square(Piece::DontCare, Player::Empty);
		capture = false;
		
	} else if (isShortCastle(src, sx, dx, dy)) {
		board[dy][dx] = src;
		board[sy][dx - 1] = getSquare(7, sy);
		board[sy][7] = Square(Piece::DontCare, Player::Empty);
		board[sy][sx] = Square(Piece::DontCare, Player::Empty);
		capture = false;

	} else if (src.piece == Piece::Pawn && isEmptySquare(dx, dy) && sx != dx) {
		// pawn has moved diagonally, but landed on a blank square
		// therefore we have en passant

		// move the piece
		board[dy][dx] = src;
		board[sy][sx] = Square(Piece::DontCare, Player::Empty);

		// do the capture
		if (src.player == Player::White) {
			board[dy - 1][dx] = Square(Piece::DontCare, Player::Empty);
		} else {
			board[dy + 1][dx] = Square(Piece::DontCare, Player::Empty);
		}

		capture = true;

	} else {
		board[dy][dx] = src;
		board[sy][sx] = Square(Piece::DontCare, Player::Empty);
	}

	// handle en passant
	enPassantPossible = allowsEnPassant(src, sy, dy);
	if (enPassantPossible) {
		enPassantFile = sx;
	}

	// handle pawn promotion
	if (src.piece == Piece::Pawn && (dy == 0 || dy == 7)) {
		board[dy][dx].piece = promotionType;
	}

	// prevent all castling if the king was moved
	if (src.piece == Piece::King) {
		preventShortCastleForPlayer(src.player);
		preventLongCastleForPlayer(src.player);
	}

	// prevent one side of castling if the rook was moved
	if (src.piece == Piece::Rook) {
		if (sx == 0) {
			preventLongCastleForPlayer(src.player);
		} else {
			preventShortCastleForPlayer(src.player);
		}
	}

	// prevent one side of castling if the rook was captured
	if (dst.piece == Piece::Rook) {
		if (dx == 0) {
			preventLongCastleForPlayer(dst.player);
		} else {
			preventShortCastleForPlayer(dst.player);
		}
	}

	// turn cleanup
	if (src.piece == Piece::Pawn || capture) {
		turnsSincePawnMoveOrCapture = 0;
	} else {
		++turnsSincePawnMoveOrCapture;
	}
	updateHistory();
	nextTurn();
}

void GameState::checkForGameOver()
{
	return;

	if (turnsSincePawnMoveOrCapture >= 150) {
		// 75 move rule
		outcome = Outcome::DrawBy75Moves;

	} else if (turnsSincePawnMoveOrCapture >= 100) {
		// 50 move rule

		// allow players to force a draw
	}

	auto legalMoves = getLegalMoves();

	if (legalMoves.size() == 0) {
		auto kingLocation = findKing(whiteTurn ? Player::White : Player::Black);
		whiteTurn = !whiteTurn;

		if (isSquareUnderAttack(kingLocation.first, kingLocation.second)) {
			//note that we just inverted whose turn it is
			outcome = whiteTurn ? Outcome::WhiteWon : Outcome::BlackWon;	

		} else {
			outcome = Outcome::DrawByStalemate;
		}

		whiteTurn = !whiteTurn;
	}
}


std::vector<Move> GameState::getPseudoLegalMoves(bool includeKing)
{
	std::vector<Move> moves;

	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			Square square = board[y][x];

			// stop recurision on king's vision when doing 'is king under attack before castling' check
			if (!includeKing && square.piece == Piece::King) {
				continue;
			}

			if ((whiteTurn && square.player == Player::White) || (!whiteTurn && square.player == Player::Black)) {
				std::vector<Coordinate> locations = getPieceVision(x, y);
				for (const Coordinate& c : locations) {
					// TODO: if promotion, add all of the pieces you can promote to as moves
					moves.push_back(Move{Coordinate(x, y), c, Piece::Queen});
				}
			}
		}
	}

	return moves;
}

std::vector<Move> GameState::getLegalMoves()
{
	std::vector<Move> legalMoves;

	if (outcome != Outcome::InProgress) {
		return legalMoves;
	}

	auto pseudoMoves = getPseudoLegalMoves();

	for (const auto& move : pseudoMoves) {
		Coordinate src = std::get<0>(move);
		Coordinate dst = std::get<1>(move);
		if (!inCheckAfterMove(whiteTurn ? Player::White : Player::Black, src.first, src.second, dst.first, dst.second, std::get<2>(move))) {
			legalMoves.push_back(move);
		}
	}

	return legalMoves;
}

std::vector<Coordinate> GameState::getSlidingVision(Square square, int x, int y, int startIndex, int endIndex)
{
	std::vector<Coordinate> locations;

	int directions[8][2] = {
		{1, 0},
		{-1, 0},
		{0, 1},
		{0, -1},
		{1, 1},
		{1, -1},
		{-1, 1},
		{-1, -1}
	};

	for (int i = startIndex; i < endIndex; ++i) {
		int dirX = directions[i][0];
		int dirY = directions[i][1];

		int newX = x;
		int newY = y;

		for (int j = 1; j < 8; ++j) {
			newX += dirX;
			newY += dirY;

			// can't go outside the board
			if (!isLocationValid(newX, newY)) {
				break;
			}

			// can't capture ourselves
			if (isSamePlayer(square, newX, newY)) {
				break;
			}

			locations.push_back(Coordinate(newX, newY));

			// can't jump over pieces
			if (isOtherPlayer(square, newX, newY)) {
				break;
			}
		}
	}

	return locations;
}

std::vector<Coordinate> GameState::getKnightVision(Square square, int x, int y)
{
	std::vector<Coordinate> locations;

	if (isCapturable(square, x + 1, y + 2)) locations.push_back(Coordinate(x + 1, y + 2));
	if (isCapturable(square, x + 2, y + 1)) locations.push_back(Coordinate(x + 2, y + 1));
	if (isCapturable(square, x - 1, y + 2)) locations.push_back(Coordinate(x - 1, y + 2));
	if (isCapturable(square, x - 2, y + 1)) locations.push_back(Coordinate(x - 2, y + 1));
	if (isCapturable(square, x + 1, y - 2)) locations.push_back(Coordinate(x + 1, y - 2));
	if (isCapturable(square, x + 2, y - 1)) locations.push_back(Coordinate(x + 2, y - 1));
	if (isCapturable(square, x - 1, y - 2)) locations.push_back(Coordinate(x - 1, y - 2));
	if (isCapturable(square, x - 2, y - 1)) locations.push_back(Coordinate(x - 2, y - 1));

	return locations;
}

std::vector<Coordinate> GameState::getKingVision(Square square, int x, int y)
{
	std::vector<Coordinate> locations;

	if (isCapturable(square, x + 1, y + 0)) locations.push_back(Coordinate(x + 1, y + 0));
	if (isCapturable(square, x - 1, y + 0)) locations.push_back(Coordinate(x - 1, y + 0));
	if (isCapturable(square, x + 0, y + 1)) locations.push_back(Coordinate(x + 0, y + 1));
	if (isCapturable(square, x + 0, y - 1)) locations.push_back(Coordinate(x + 0, y - 1));
	if (isCapturable(square, x + 1, y + 1)) locations.push_back(Coordinate(x + 1, y + 1));
	if (isCapturable(square, x - 1, y + 1)) locations.push_back(Coordinate(x - 1, y + 1));
	if (isCapturable(square, x + 1, y - 1)) locations.push_back(Coordinate(x + 1, y - 1));
	if (isCapturable(square, x - 1, y - 1)) locations.push_back(Coordinate(x - 1, y - 1));

	bool shortCastleWorks = (square.player == Player::White && whiteShortCastlePossible) || (square.player == Player::Black && blackShortCastlePossible);
	bool longCastleWorks  = (square.player == Player::White && whiteLongCastlePossible)  || (square.player == Player::Black && blackLongCastlePossible);

	// for the 'square under attack' functions, we need it to be the opponent's move (we can't attack ourselves!)
	whiteTurn ^= true;
	if (isLocationValid(x, y) && (shortCastleWorks || longCastleWorks) && !isSquareUnderAttack(x, y)) {
		if (shortCastleWorks && isEmptySquare(x + 1, y) && isEmptySquare(x + 2, y) && !isSquareUnderAttack(x + 1, y)) {
			locations.push_back(Coordinate(x + 2, y));
		}

		if (longCastleWorks && isEmptySquare(x - 1, y) && isEmptySquare(x - 2, y) && isEmptySquare(x - 3, y) && !isSquareUnderAttack(x - 1, y) && !isSquareUnderAttack(x - 2, y)) {
			locations.push_back(Coordinate(x - 2, y));
		}
	}

	whiteTurn ^= true;

	return locations;
}

std::vector<Coordinate> GameState::getPawnVision(Square square, int x, int y)
{
	std::vector<Coordinate> locations;

	int forward = square.player == Player::White ? 1 : -1;

	// can only move forward if empty
	if (isEmptySquare(x, y + forward)) {
		locations.push_back(Coordinate(x, y + forward));

		// only if the first move is possible can we move two squares,
		// and it is on the first rank
		if (isEmptySquare(x, y + forward * 2) && isFirstRankPawn(board[y][x], y)) {
			locations.push_back(Coordinate(x, y + forward * 2));
		}
	}

	// captures
	if (isOtherPlayer(square, x - 1, y + forward)) {
		locations.push_back(Coordinate(x - 1, y + forward));
	}
	if (isOtherPlayer(square, x + 1, y + forward)) {
		locations.push_back(Coordinate(x + 1, y + forward));
	}

	// en passant
	if (enPassantPossible && (x == enPassantFile + 1 || x == enPassantFile - 1)) {
		if (square.player == Player::White && y == 4) {
			locations.push_back(Coordinate(enPassantFile, 5));

		} else if (square.player == Player::Black && y == 3) {
			locations.push_back(Coordinate(enPassantFile, 2));
		}
	}

	return locations;
}

std::vector<Coordinate> GameState::getPieceVision(int x, int y)
{
	Square square = getSquare(x, y);
	if (square.player == Player::Empty) {
		return std::vector<Coordinate>();
	}

	switch (square.piece) {
	case Piece::Pawn:	return getPawnVision(square, x, y);
	case Piece::Knight: return getKnightVision(square, x, y);
	case Piece::Bishop: return getSlidingVision(square, x, y, 4, 8);
	case Piece::Rook:	return getSlidingVision(square, x, y, 0, 4);
	case Piece::Queen:	return getSlidingVision(square, x, y, 0, 8);
	case Piece::King:	return getKingVision(square, x, y);
	default:
		std::cerr << "That piece doesn't have vision implemented!";
		abort();
	}

	// shouldn't get here
	return std::vector<Coordinate>();
}

void GameState::preventShortCastleForPlayer(Player p)
{
	if (p == Player::White) {
		whiteShortCastlePossible = false;
	} else {
		blackShortCastlePossible = false;
	}
}

void GameState::preventLongCastleForPlayer(Player p)
{
	if (p == Player::White) {
		whiteLongCastlePossible = false;
	} else {
		blackLongCastlePossible = false;
	}
}

bool GameState::isSamePlayer(Square a, int x, int y)
{
	return isLocationValid(x, y) && board[y][x].player == a.player && a.player != Player::Empty && board[y][x].player != Player::Empty;
}

bool GameState::isCapturable(Square a, int x, int y)
{
	return isLocationValid(x, y) && !isSamePlayer(a, x, y);
}

bool GameState::isOtherPlayer(Square a, int x, int y)
{
	return isLocationValid(x, y) && board[y][x].player != a.player && a.player != Player::Empty && board[y][x].player != Player::Empty;
}

bool GameState::isEmptySquare(int x, int y)
{
	return isLocationValid(x, y) && board[y][x].player == Player::Empty;
}

bool isLongCastle(Square src, int sx, int dx, int dy)
{
	return (src.piece == Piece::King) && (dx == sx - 2) && (dy == 0 || dy == 7);
}

bool isShortCastle(Square src, int sx, int dx, int dy)
{
	return (src.piece == Piece::King) && (dx == sx + 2) && (dy == 0 || dy == 7);
}

bool isFirstRankPawn(Square sq, int rank)
{
	return (sq.player == Player::White && rank == 1) || (sq.player == Player::Black && rank == 6);
}

bool allowsEnPassant(Square sq, int srcRank, int dstRank)
{
	if (sq.piece == Piece::Pawn) {
		bool srcGood = isFirstRankPawn(sq, srcRank);
		bool dstGood = (sq.player == Player::White && dstRank == 3) || (sq.player == Player::Black && dstRank == 4);
		return srcGood && dstGood;

	} else {
		return false;
	}
}

bool isLocationValid(int x, int y)
{
	return x >= 0 && y >= 0 && x < 8 && y < 8;
}

Player togglePlayer(Player player)
{
	if (player == Player::White) return Player::Black;
	else if (player == Player::Black) return Player::White;

	std::cerr << "Cannot toggle an empty player!";
	abort();
}
