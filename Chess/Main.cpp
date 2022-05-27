
#include <cstdio>
#include <iostream>
#include <atomic>
#include <thread>
#include <cmath>
#include <cassert>

#include "Engine.hpp"
#include "UI.hpp"
#include "AI.hpp"

void handleQuit(UI* ui)
{
	UIExit(ui);
}

const int xMargin = (WINDOW_WIDTH - 8 * SQUARE_SIZE) / 2;
const int yMargin = (WINDOW_HEIGHT - 8 * SQUARE_SIZE) / 2;

int clickedX = -1;
int clickedY = -1;

int selectedX = -1;
int selectedY = -1;

bool gotMouseDown = false;

void handleMouseDown(int x, int y)
{
	if (x < xMargin || x > xMargin + 8 * SQUARE_SIZE || y < yMargin || y > yMargin + 8 * SQUARE_SIZE) {
		clickedX = -1;
		clickedY = -1;
		return;
	}

	clickedX = (x - xMargin) / SQUARE_SIZE;
	clickedY = 7 - ((y - yMargin) / SQUARE_SIZE);

	gotMouseDown = true;
}

void handleMouseUp(int x, int y)
{

}

void handleMouseMove(int x, int y)
{

}

void handleKeyboard(int key)
{

}

void handleEscapeKey()
{
	selectedX = -1;
	selectedY = -1;
}

UIMessage currentMessage = UIMessage::None;

bool userInputMode = true;
std::atomic<bool> aiThreadRunning(false);
std::thread* aiThread = nullptr;

void aiThreadFunc(GameState* state)
{
	double eval = evaluatePosition(*state);
	eval /= 100;

	printf("%+.1f\n", eval);

	Move bestMove;
	time_t t = clock();
	int depth = 3;
	do {
		bestMove = treeAI(*state, depth++);
	} while ((clock() - t) / CLOCKS_PER_SEC < 3);

	state->makeMove(bestMove);

	printf("%+.1f\n", eval);

	userInputMode = true;

	// I bet this is going to cause problems
	aiThreadRunning = false;
}

int whiteMilliseconds = 15 * 60 * 1000 + 999;
int blackMilliseconds = 15 * 60 * 1000 + 999;

int main(int argc, char** argv)
{
	engineInit();

	GameState state;
	UI* ui = UIInit();
	
	clock_t previousClock = 0;
	clock_t clocksPlayed = 0;
	bool firstMovePlayed = false;

	assert(CLOCKS_PER_SEC >= 50);

	while (true) {
		if (firstMovePlayed) {
			clock_t currentClock = clock();
			if (previousClock == 0) previousClock = currentClock;
			clocksPlayed += currentClock - previousClock;
			previousClock = currentClock;


			while (clocksPlayed > CLOCKS_PER_SEC / 50) {
				clocksPlayed -= CLOCKS_PER_SEC / 50;

				if (state.whiteTurn) {
					whiteMilliseconds -= 20;
				} else {
					blackMilliseconds -= 20;
				}
			}
		}
		
		UICheckEvent(ui);

		if (userInputMode) {
			if (aiThread != nullptr) {
				// delete aiThread;
				aiThread = nullptr;
			}

			if (gotMouseDown) {
				gotMouseDown = false;

				if (selectedX == -1 && selectedY == -1) {
					// first click
					selectedX = clickedX;
					selectedY = clickedY;

				} else {
					// second click

					Piece promotion = Piece::DontCare;

					std::vector<Move> moves = state.getLegalMoves();

					for (const Move& legalMove : moves) {
						if (std::get<0>(legalMove).first == selectedX &&
							std::get<0>(legalMove).second == selectedY &&
							std::get<1>(legalMove).first == clickedX &&
							std::get<1>(legalMove).second == clickedY) {

							state.makeMove(selectedX, selectedY, clickedX, clickedY, promotion);

							if (state.outcome == Outcome::WhiteWon) {
								currentMessage = UIMessage::DisplayWinWhite;

							} else if (state.outcome == Outcome::BlackWon) {
								currentMessage = UIMessage::DisplayWinBlack;

							} else if (state.outcome == Outcome::DrawByStalemate) {
								currentMessage = UIMessage::DisplayDrawStalemate;
							}

							userInputMode = false;
							firstMovePlayed = true;

							handleEscapeKey();
							break;
						}
					}

					handleEscapeKey();
				}

			}
		} else {
			firstMovePlayed = true;
			
			if (!aiThreadRunning) {
				aiThreadRunning = true;
				aiThread = new std::thread(aiThreadFunc, &state);
			}
		}

		UIClear(ui);

		for (int y = 0; y < 8; ++y) {
			for (int x = 0; x < 8; ++x) {
				Square square = state.board[y][x];
				char pieceChar = 'P';

				if (square.piece == Piece::Knight)	pieceChar = 'N';
				if (square.piece == Piece::Bishop)	pieceChar = 'B';
				if (square.piece == Piece::Rook)	pieceChar = 'R';
				if (square.piece == Piece::Queen)	pieceChar = 'Q';
				if (square.piece == Piece::King)	pieceChar = 'K';

				if (square.player == Player::Black) pieceChar += 32;

				std::vector<Move> moves = state.getLegalMoves();

				bool highlight = false;

				for (const Move& legalMove : moves) {

					if (selectedX == -1) {
						if (std::get<0>(legalMove).first == x &&
							std::get<0>(legalMove).second == y) {
							highlight = 1;
							break;
						}

					} else {
						if (x == selectedX && y == selectedY) highlight = 1;

						if (std::get<0>(legalMove).first == selectedX &&
							std::get<0>(legalMove).second == selectedY &&
							std::get<1>(legalMove).first == x &&
							std::get<1>(legalMove).second == y) {

							highlight = 1;
							break;
						}
					}
				}

				if (square.player == Player::Empty) {
					pieceChar = (x + y) & 1 ? 'z' : 'Z';
				}
				
				if (!userInputMode) highlight = 0;
				UIDrawPiece(ui, x * SQUARE_SIZE + xMargin, (7 - y) * SQUARE_SIZE + yMargin, pieceChar, highlight, !((x + y) & 1));
			}
		}

		double trueEval = (double) overallEvaluation / 100.0;
		double eval = trueEval;
		if (eval < -600) eval = -25;
		else if (eval < -23) eval = -23;
		else if (eval > 600) eval = 25;
		else if (eval > 23) eval = 23;

		int barWidth = 40;

		double evalBarX = (xMargin - barWidth) / 2;
		double evalBarY = WINDOW_HEIGHT / 4;
		double midVal = sqrt(abs(eval));
		double blackEvalFraction = (eval < 0 ? 5 + midVal : 5 - midVal) / 10.0;

		int distance = blackEvalFraction * WINDOW_HEIGHT / 2;

		char evalString[16];
		sprintf(evalString, "%+.1f", trueEval);
		if (abs(trueEval) > 600) {
			strcpy(evalString, "MATE");
		}
		UIDrawText(ui, evalBarX + barWidth / 2, WINDOW_HEIGHT * 3 / 4 + 15, evalString, 0xFFFFFF, 20, true);

		int whiteSeconds = whiteMilliseconds / 1000;
		int blackSeconds = blackMilliseconds / 1000;

		char whiteTimeStr[32];
		char blackTimeStr[32];
		sprintf(whiteTimeStr, "%d:%02d", whiteSeconds / 60, whiteSeconds % 60);
		sprintf(blackTimeStr, "%d:%02d", blackSeconds / 60, blackSeconds % 60);
		UIDrawText(ui, WINDOW_WIDTH - xMargin + 20, yMargin, blackTimeStr, 0xFFFFFF, 20, false);
		UIDrawText(ui, WINDOW_WIDTH - xMargin + 20, WINDOW_HEIGHT - yMargin - 20, whiteTimeStr, 0xFFFFFF, 20, false);


		UIDrawRect(ui, evalBarX, evalBarY, barWidth, WINDOW_HEIGHT / 2, 0);
		UIDrawRect(ui, evalBarX, evalBarY + distance, barWidth, WINDOW_HEIGHT / 2 - distance, 0xFFFFFF);
		UIDrawMessage(ui, (WINDOW_WIDTH - 350) / 2, (yMargin - 50) / 2, currentMessage);
		UIPresent(ui);
	}

	return 0;
}