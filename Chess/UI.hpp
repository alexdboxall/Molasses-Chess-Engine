#pragma once

#include <cstdint>
#include <SDL.h>
#include <SDL_ttf.h>

#define WINDOW_WIDTH	1000
#define WINDOW_HEIGHT	750
#define SQUARE_SIZE		68

// in the header, only the contents of this struct may rely on SDL
// in the implementation, it can be SDL specific
struct UI
{
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Renderer* renderer;
	SDL_Texture* pawnIcon[2];
	SDL_Texture* knightIcon[2];
	SDL_Texture* bishopIcon[2];
	SDL_Texture* rookIcon[2];
	SDL_Texture* queenIcon[2];
	SDL_Texture* kingIcon[2];
	SDL_Texture* squareIcon[2];
};

enum class UIMessage
{
	None,
	DisplayDraw50,
	DisplayDraw75,
	DisplayDrawRepetition,
	DisplayDrawAgree,
	DisplayDrawTime,
	DisplayDrawDeadPosition,
	DisplayDrawStalemate,
	DisplayWinWhite,
	DisplayWinWhiteTime,
	DisplayWinWhiteResign,
	DisplayWinBlack,
	DisplayWinBlackTime,
	DisplayWinBlackResign,
};

UI* UIInit();
void UIClear(UI* ui);
void UIPresent(UI* ui);
void UIDrawMessage(UI* ui, int x, int y, UIMessage msg);
void UIDrawRect(UI* ui, int x, int y, int w, int h, uint32_t col);
void UIDrawPiece(UI* ui, int x, int y, char piece, int highlight, bool blackSquare);
void UIDrawText(UI* ui, int x, int y, const char* text, uint32_t col, int size, bool centered);
void UICheckEvent(UI* ui);
void UIExit(UI* ui);

void handleQuit(UI* ui);
void handleMouseDown(int x, int y);
void handleMouseUp(int x, int y);
void handleMouseMove(int x, int y);
void handleKeyboard(int key);
void handleEscapeKey();
