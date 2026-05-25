#include "Game.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include <conio.h>
#include <windows.h>
#include "Frame.h"

Game::Game(int window_columns, int window_rows) {
	m_columns = window_columns;
	m_rows = window_rows;

	int left = (m_columns - MAP_WIDTH) / 2 - 1;
	int right = (m_columns + MAP_WIDTH) / 2 - 1;
	int top = RENDER_START_POS_FRAME_Y;
	int bottom = RENDER_END_POS_FRAME_Y;

	int sidebarWidth = 15;
	int sideLeft = left - sidebarWidth;
	int sideRight = right + sidebarWidth;
	int div1 = top + MAP_HEIGHT / 3;
	int div2 = top + 2 * MAP_HEIGHT / 3;

	FrameBuilder fb;

	// top row
	fb.add(sideLeft, top, Junction::CORNER_TL);
	fb.add(left, top, Junction::T_DOWN);
	fb.add(right, top, Junction::T_DOWN);
	fb.add(sideRight, top, Junction::CORNER_TR);

	// left sidebar dividers (3 blocks)
	fb.add(sideLeft, div1, Junction::T_RIGHT);
	fb.add(left, div1, Junction::T_LEFT);
	fb.add(sideLeft, div2, Junction::T_RIGHT);
	fb.add(left, div2, Junction::T_LEFT);

	// bottom row
	fb.add(sideLeft, bottom, Junction::CORNER_BL);
	fb.add(left, bottom, Junction::T_UP);
	fb.add(right, bottom, Junction::T_UP);
	fb.add(sideRight, bottom, Junction::CORNER_BR);

	m_frame = fb.build();
}

void Game::run() {

	renderEmblem();

	while (running) {

		// input
		if (_kbhit()) {           // returns true if a keyinput is waiting to be processed
			int key = _getch();   // read one keypress, no Enter needed
			update(key);
		}

		if (state == MENU) {
			renderMenu();
			std::cin.get();

			startTime = std::chrono::steady_clock::now();
			state = GAME;
		}
		else if (state == GAME) {
			renderGame();
		}
		else if (state == WIN) {
			renderWinningScreen();

			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
			int key = _getch();

			if (key == 27) { // escape = quit game
				running = false;
			}
			reset();
		}
		else if (state == LOSE) {
			renderLosingScreen();

			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
			int key = _getch();

			if (key == 27) { // escape = quit game
				running = false;
			}
			reset();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(33)); // simple approach to get 30ish fps
	}
}

void Game::update(int inputKey) {

	int tmpPosX = 0;
	int tmpPosY = 0;

	switch (inputKey) {
	case 'w': tmpPosY = -1; break;
	case 's': tmpPosY = 1; break;
	case 'a': tmpPosX = -1; break;
	case 'd': tmpPosX = 1; break;
	}

	bool withinLeftBound = playerPosX + tmpPosX > -1;
	bool withinRightBound = playerPosX + tmpPosX < (int)map[0].size();
	bool withinUpperBound = playerPosY + tmpPosY > -1;
	bool withinLowerBound = playerPosY + tmpPosY < (int)map.size();

	if (!withinLeftBound || !withinRightBound || !withinLowerBound || !withinUpperBound) {
		return;
	}

	bool isWall = map[playerPosY + tmpPosY][playerPosX + tmpPosX] == '+';
	if (isWall) {
		return;
	}

	playerPosX += tmpPosX;
	playerPosY += tmpPosY;

	if (playerPosX == treasurePosX && playerPosY == treasurePosY) {
		state = WIN;
	}
}

void Game::renderGame() {
	frameBuffer.clear();
	frameBuffer += ansi::MOVE_TO(RENDER_POS_Y, 1);
	frameBuffer += ansi::CLEAR_ALL_AFTER;

	renderMap();
	renderFrame();
	renderTimeLeft();

	std::cout << frameBuffer << std::flush;
}

void Game::renderEmblem() {
	int centerPos_X = (m_columns - ansi::EMBLEM_WIDTH) / 2;

	// render emblem
	for (int i = 0; i < (int)std::size(ansi::EMBLEM); i++) {
		std::cout << ansi::MOVE_TO(i + 1, centerPos_X) << ansi::EMBLEM[i];
		std::this_thread::sleep_for(std::chrono::milliseconds(50)); // to make the rendering process visible, not necessary, but looks nice!
	}
}

void Game::renderMenu() {

	//TODO extend menu in the future
	std::string prompt = "Press [ENTER] to start the game.";
	std::cout << ansi::MOVE_TO(RENDER_POS_Y, (m_columns - (int)prompt.size()) / 2) << prompt << "\n" << ansi::GREEN;
}

void Game::renderMap() {

	int centerPos_X = (m_columns - MAP_WIDTH) / 2;

	for (int y = 0; y < (int)map.size(); y++) {
		frameBuffer += ansi::MOVE_TO(RENDER_START_POS_MAP_Y + y, centerPos_X);

		for (int x = 0; x < (int)map[0].size(); x++) {
			char symbol = map[y][x];
			std::string_view colorNear = ansi::GREEN;
			std::string_view colorFar = ansi::DARK_GREEN;

			// player
			if (x == playerPosX && y == playerPosY) {
				symbol = '@';
				colorNear = ansi::ORANGE;
			}
			// treasure
			if (x == treasurePosX && y == treasurePosY) {
				symbol = '$';
				colorNear = ansi::YELLOW;
				colorFar = ansi::DARK_YELLOW;
			}

			int dx = x - playerPosX;
			int dy = y - playerPosY;

			// replace symbols out of view with blank space to make them invisible
			bool inDarkZone = (std::max)(std::abs(dx), std::abs(dy * 2)) > darkZoneRadius;
			if (inDarkZone) {
				if (exploredMap[y][x]) {
					frameBuffer += colorFar;
					frameBuffer += symbol;
				}
				else {
					frameBuffer += ' ';
				}
				continue;
			}

			// print nearby symbols in normal color
			frameBuffer += colorNear;
			frameBuffer += symbol;
			exploredMap[y][x] = true;
		}
	}
}

void Game::renderFrame() {
	frameBuffer += ansi::GREEN;
	frameBuffer += m_frame;
}

void Game::renderTimeLeft() {
	int timeElapsed = (int)std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::steady_clock::now() - startTime).count();

	if (timeElapsed > TIME_LIMIT) {
		state = LOSE;
		return;
	}

	std::string minutesLeft = std::to_string((TIME_LIMIT - timeElapsed) / 60);
	std::string secondsLeft = std::to_string((TIME_LIMIT - timeElapsed) % 60);
	if (secondsLeft.size() == 1) {
		secondsLeft = "0" + secondsLeft;
	}

	frameBuffer += ansi::MOVE_TO(RENDER_START_POS_TIME_Y, (m_columns - MAP_WIDTH) / 2 - 1);
	frameBuffer += ansi::RED;
	frameBuffer += "Time left: " + minutesLeft + ":" + secondsLeft;
}

void Game::renderWinningScreen() {
	frameBuffer.clear();

	std::string prompt = "Congratulations - you have found the treasure!";
	int centerPos_X = (m_columns - (int)prompt.size()) / 2;
	frameBuffer += ansi::MOVE_TO(RENDER_POS_Y, centerPos_X);
	frameBuffer += ansi::CLEAR_ALL_AFTER;
	frameBuffer += ansi::YELLOW;
	frameBuffer += prompt;
	std::cout << frameBuffer << std::flush;

	centerPos_X = m_columns / 2; // reassign (not redeclare) for countdown centering
	for (int i = 5; i > 0; i--) {
		frameBuffer.clear();
		frameBuffer += ansi::MOVE_TO(RENDER_POS_Y + 1, centerPos_X);
		frameBuffer += std::to_string(i);
		std::cout << frameBuffer << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	frameBuffer.clear();
	prompt = "Press any key to restart or [ESC] to quit the game";
	centerPos_X = (m_columns - (int)prompt.size()) / 2;
	frameBuffer += ansi::MOVE_TO(RENDER_POS_Y + 1, centerPos_X);
	frameBuffer += prompt;
	std::cout << frameBuffer << std::flush;
}

void Game::renderLosingScreen() {
	frameBuffer.clear();

	std::string prompt = "Time's up! You lost.";
	int centerPos_X = (m_columns - (int)prompt.size()) / 2;
	frameBuffer += ansi::MOVE_TO(RENDER_POS_Y, centerPos_X);
	frameBuffer += ansi::CLEAR_ALL_AFTER;
	frameBuffer += ansi::RED;
	frameBuffer += prompt;
	std::cout << frameBuffer << std::flush;

	centerPos_X = m_columns / 2; // reassign (not redeclare) for countdown centering
	for (int i = 5; i > 0; i--) {
		frameBuffer.clear();
		frameBuffer += ansi::MOVE_TO(RENDER_POS_Y + 1, centerPos_X);
		frameBuffer += std::to_string(i);
		std::cout << frameBuffer << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	frameBuffer.clear();
	prompt = "Press any key to restart or [ESC] to quit the game";
	centerPos_X = (m_columns - (int)prompt.size()) / 2;
	frameBuffer += ansi::MOVE_TO(RENDER_POS_Y + 1, centerPos_X);
	frameBuffer += prompt;
	std::cout << frameBuffer << std::flush;
}

bool Game::isWon() {
	return state == WIN;
}

bool Game::isLost() {
	return state == LOSE;
}

void Game::reset() {
	playerPosX = 0;
	playerPosY = 1;
	startTime = std::chrono::steady_clock::now();

	for (auto& row : exploredMap) {
		std::fill(row.begin(), row.end(), false);
	}

	state = GAME;
}
