
#include "UI.hpp"
#include <cassert>
#include <SDL_image.h>
#include <cstdio>
#include <cstring>

#define ROOT_FILEPATH "../"

UI* UIInit()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("Molasses", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN);

	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	IMG_Init(IMG_INIT_PNG);
	TTF_Init();



	UI* ui = new UI();
	ui->window = window;
	ui->screenSurface = screenSurface;
	ui->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	ui->pawnIcon[0] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/wp.png"));
	ui->pawnIcon[1] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/bp.png"));
	ui->knightIcon[0] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/wn.png"));
	ui->knightIcon[1] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/bn.png"));
	ui->bishopIcon[0] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/wb.png"));
	ui->bishopIcon[1] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/bb.png"));
	ui->rookIcon[0] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/wr.png"));
	ui->rookIcon[1] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/br.png"));
	ui->queenIcon[0] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/wq.png"));
	ui->queenIcon[1] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/bq.png"));
	ui->kingIcon[0] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/wk.png"));
	ui->kingIcon[1] = SDL_CreateTextureFromSurface(ui->renderer, IMG_Load(ROOT_FILEPATH "icons/bk.png"));
	ui->squareIcon[0] = SDL_CreateTextureFromSurface(ui->renderer, SDL_LoadBMP(ROOT_FILEPATH "icons/w.bmp"));
	ui->squareIcon[1] = SDL_CreateTextureFromSurface(ui->renderer, SDL_LoadBMP(ROOT_FILEPATH "icons/b.bmp"));

	return ui;
}

void UIExit(UI* ui)
{
	SDL_Window* window = ui->window;

	SDL_DestroyWindow(window);
	SDL_Quit();
	exit(0);
}

void UIClear(UI* ui)
{
	SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
	SDL_RenderClear(ui->renderer);
}

void UIPresent(UI* ui)
{
	SDL_RenderPresent(ui->renderer);
}

void UIDrawMessage(UI* ui, int x, int y, UIMessage msg) {
	char filepath[256];

	switch (msg) {
	case UIMessage::None:
		return;
	case UIMessage::DisplayDraw50:
		strcpy(filepath, "icons/50draw.bmp");
		break;
	case UIMessage::DisplayDraw75:
		strcpy(filepath, "icons/75draw.bmp");
		break;
	case UIMessage::DisplayDrawAgree:
		strcpy(filepath, "icons/agreedraw.bmp");
		break;
	case UIMessage::DisplayDrawDeadPosition:
		strcpy(filepath, "icons/deaddraw.bmp");
		break;
	case UIMessage::DisplayDrawRepetition:
		strcpy(filepath, "icons/repetitiondraw.bmp");
		break;
	case UIMessage::DisplayDrawStalemate:
		strcpy(filepath, "icons/stalemate.bmp");
		break;
	case UIMessage::DisplayDrawTime:
		strcpy(filepath, "icons/timedraw.bmp");
		break;
	case UIMessage::DisplayWinBlack:
		strcpy(filepath, "icons/blackwin.bmp");
		break;
	case UIMessage::DisplayWinBlackResign:
		strcpy(filepath, "icons/blackwinresgn.bmp");
		break;
	case UIMessage::DisplayWinBlackTime:
		strcpy(filepath, "icons/blackwintime.bmp");
		break;
	case UIMessage::DisplayWinWhite:
		strcpy(filepath, "icons/whitewin.bmp");
		break;
	case UIMessage::DisplayWinWhiteResign:
		strcpy(filepath, "icons/whitewinresgn.bmp");
		break;
	case UIMessage::DisplayWinWhiteTime:
		strcpy(filepath, "icons/whitewintime.bmp");
		break;
	default:
		assert(false);
		break;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(ui->renderer, SDL_LoadBMP(filepath));

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 350;
	rect.h = 50;

	SDL_RenderCopy(ui->renderer, texture, NULL, &rect);
}


void UIDrawRect(UI* ui, int x, int y, int w, int h, uint32_t col)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_Texture* texture = ui->squareIcon[col == 0 ? 1 : 0];
	SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
	SDL_SetTextureColorMod(texture, 255, 255, 255);
	SDL_RenderCopy(ui->renderer, texture, NULL, &rect);
}

void UIDrawPiece(UI* ui, int x, int y, char piece, int highlight, bool blackSquare)
{
	SDL_Texture* texture = nullptr;

	int black = 0;
	if (piece >= 'a') {
		black = 1;
		piece -= 32;
	}

	if (piece == 'P') texture = ui->pawnIcon[black];
	else if (piece == 'N') texture = ui->knightIcon[black];
	else if (piece == 'B') texture = ui->bishopIcon[black];
	else if (piece == 'R') texture = ui->rookIcon[black];
	else if (piece == 'Q') texture = ui->queenIcon[black];
	else if (piece == 'K') texture = ui->kingIcon[black];
	
	SDL_Texture* boardTexture = ui->squareIcon[blackSquare];
	if (highlight) {
		boardTexture = ui->squareIcon[0];
	}
	if (piece == 'Z' || piece == 'z') {
		texture = boardTexture;
	}

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 68;
	rect.h = 68;

	if (highlight == 1) {
		SDL_SetTextureColorMod(boardTexture, 255, 255, 0);
		SDL_SetTextureColorMod(texture, 255, 255, 0);
	} else {
		SDL_SetTextureColorMod(boardTexture, 255, 255, 255);
		SDL_SetTextureColorMod(texture, 255, 255, 255);
	}
	SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderCopy(ui->renderer, boardTexture, NULL, &rect);
	SDL_RenderCopy(ui->renderer, texture, NULL, &rect);
	SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
}

void UIDrawText(UI* ui, int x, int y, const char* text, uint32_t col, int size, bool centered)
{
	SDL_Color colour;
	colour.r = (col >> 16) & 0xFF;
	colour.g = (col >> 8) & 0xFF;
	colour.b = (col >> 0) & 0xFF;
	colour.a = 0xFF;

	TTF_Font* ttfFont = TTF_OpenFont(ROOT_FILEPATH "font/Inter-Regular.ttf", size);
	SDL_Surface* textSurface = TTF_RenderText_Blended(ttfFont, text, colour);
	SDL_Texture* mTexture = SDL_CreateTextureFromSurface(ui->renderer, textSurface);

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = textSurface->w;
	rect.h = textSurface->h;

	if (centered) {
		rect.x -= rect.w / 2;
	}

	SDL_RenderCopy(ui->renderer, mTexture, NULL, &rect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(mTexture);
	TTF_CloseFont(ttfFont);
}

void UICheckEvent(UI* ui)
{
	SDL_PumpEvents();

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			handleQuit(ui);
		} else if (e.type == SDL_MOUSEBUTTONDOWN) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			handleMouseDown(x, y);
		} else if (e.type == SDL_MOUSEBUTTONUP) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			handleMouseUp(x, y);

		} else if (e.type == SDL_MOUSEMOTION) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			handleMouseMove(x, y);

		} else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				handleEscapeKey();
			} else {
				
			}
		}
	}
}