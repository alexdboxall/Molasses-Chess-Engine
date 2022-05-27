#pragma once

#include <cstdint>

enum class Piece : uint8_t
{
	DontCare,
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King,
};

enum class Player : uint8_t
{
	Empty,
	White,
	Black,
};

class Square
{
public:
	Player player : 2;
	Piece piece	  : 3;

	Square()
	{
		piece = Piece::DontCare;
		player = Player::Empty;
	}

	Square(Piece piece_, Player player_)
	{
		piece = piece_;
		player = player_;
	}

	bool isEmpty()
	{
		return player == Player::Empty;
	}
};
