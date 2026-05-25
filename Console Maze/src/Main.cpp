#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <mmsystem.h>
#include "Game.h"
#pragma comment(lib, "winmm.lib")

int main() {

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int window_columns, window_rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	window_columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	window_rows    = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	system("chcp 65001 > nul");

	// start playing music
	PlaySound(L"soundtrack.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	// settings
	std::cout << ansi::CURSOR_OFF; // switch off cursor
	std::cout << ansi::GREEN;      // set color to green

	// start game
	Game game(window_columns, window_rows);
	game.run();
}
