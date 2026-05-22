#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include "Ansi.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class Game {

public:

	Game(int window_columns, int window_rows) {
		m_columns = window_columns;
		m_rows = window_rows;
	}

private:

	static constexpr int TIME_LIMIT = 5;
	static constexpr int MAP_WIDTH = 54;
	static constexpr int MAP_HEIGHT = 19;
	static constexpr int RENDER_POS_Y = ansi::EMBLEM_HEIGHT;
	static constexpr int RENDER_START_POS_FRAME_Y = RENDER_POS_Y;
	static constexpr int RENDER_END_POS_FRAME_Y = RENDER_START_POS_FRAME_Y + MAP_HEIGHT;
	static constexpr int RENDER_START_POS_MAP_Y = RENDER_START_POS_FRAME_Y + 1;
	static constexpr int RENDER_START_POS_TIME_Y = RENDER_END_POS_FRAME_Y + 1;



	static constexpr int darkZoneRadius = 2;
	enum game_state {
		MENU, GAME, WIN, LOSE
	};

	game_state state = MENU;

	int m_columns = 0;
	int m_rows = 0;

	const int treasurePosX = 3; //52
	const int treasurePosY = 1; //16

	int playerPosX = 0;
	int playerPosY = 1;

	std::chrono::steady_clock::time_point startTime;

	std::string frameBuffer;

	std::vector<std::string> map = {
		"+++++++++++++++++++++++++++++++++++++++++++++++++++++",
		"    ++        ++++++++++++++     ++ +++++++++++++++++",
		"+++ ++++++ ++                + + ++                ++",
		"+++     ++ ++ ++++++++++++++ + + ++ ++++++++++++++ ++",
		"+ +++++ ++ ++ +      +++++++ + + ++ ++ ++      + + ++",
		"+ ++    ++ ++ + ++ ++++      + + ++ ++ ++ ++++ + + ++",
		"+ ++ ++++++++ + ++ +    +++++++++++ ++ ++ ++ + + + ++",
		"+ ++          + ++ + ++ +              ++ ++ + + + ++",
		"+ +  +++++++++++++ + ++ + ++ ++++ ++++ ++ ++ + + + ++",
		"+ + +++++++        + ++ + ++ +       + ++ ++ + + + ++",
		"+ + +        ++++++++++ + ++ + +++++ + ++    + + + ++",
		"+ + + ++++++++++        + ++ +   +++++ +++++++ + + ++",
		"+ + + ++++++++++ +++++++++++ +++++             + + ++",
		"+ + +            +++++++++++    ++ ++++++++ + ++ + ++",
		"+ ++++++++++++++ ++         ++++++ ++ +++++ +    + ++",
		"+                +++++++++++++++++ ++       ++++++ ++",
		"++++++++++++++++                   ++++++++      +   ",
		"+++++++++++++++++++++++++++++++++++++++++++++++++++++",
	};

	std::vector<std::vector<bool>> exploredMap{ map.size(), std::vector<bool>(map[0].size(), false) };

public:
	void update(int inputKey) {

		int tmpPosX = 0;
		int tmpPosY = 0;

		switch (inputKey) {
		case 'w':
			tmpPosY = -1;
			break;
		case 's':
			tmpPosY = 1;
			break;
		case 'a':
			tmpPosX = -1;
			break;
		case 'd':
			tmpPosX = 1;
			break;
		}

		bool withinLeftBound = playerPosX + tmpPosX > -1;
		bool withinRightBound = playerPosX + tmpPosX < (map[0].size());
		bool withinUpperBound = playerPosY + tmpPosY > -1;
		bool withinLowerBound = playerPosY + tmpPosY < (map.size());

		if (!withinLeftBound || !withinRightBound || !withinLowerBound || !withinUpperBound) {
			return;
		}

		bool isWall = map[playerPosY + tmpPosY][playerPosX + tmpPosX] == '+';
		if (isWall) {
			return;
		}

		playerPosX += tmpPosX;
		playerPosY += tmpPosY;

		bool reachedFinish = playerPosX == treasurePosX && playerPosY == treasurePosY;
		if (reachedFinish) {
			state = WIN;
		}
	}

	bool render() {
		frameBuffer.clear();
		frameBuffer += ansi::MOVE_TO(RENDER_POS_Y, 1);
		frameBuffer += ansi::CLEAR_ALL_AFTER;

		if (state == MENU) {
			renderMenu();
			return true;
		}

		if (state == WIN) {
			return renderWinningScreen();
		}

		if (state == LOSE) {
			return renderLosingScreen();
		}

		renderMap();
		renderFrame();
		renderTimeLeft();

		std::cout << frameBuffer << std::flush;
		return true;
	}

	bool isWon() {
		return state == WIN;
	}

	bool isLost() {
		return state == LOSE;
	}

	void reset() {
		playerPosX = 0;
		playerPosY = 1;
		startTime = std::chrono::steady_clock::now();

		for (auto& row : exploredMap) {
			std::fill(row.begin(), row.end(), false);
		}

		state = GAME;
	}

private:

	void renderFrame() {

		frameBuffer += ansi::GREEN;

		std::string horizontalBar;
		for (int i = 0; i < map[0].size(); i++)
			horizontalBar += "═";

		// upper frame border
		frameBuffer += ansi::MOVE_TO(RENDER_START_POS_FRAME_Y, (m_columns - MAP_WIDTH) / 2 - 1);
		frameBuffer += "╔" + horizontalBar + "╗";

		// lower frame border
		frameBuffer += ansi::MOVE_TO(RENDER_END_POS_FRAME_Y, (m_columns - MAP_WIDTH) / 2 - 1);
		frameBuffer += "╚" + horizontalBar + "╝";

		// left and right frame border
		for (int i = 0; i < map.size(); i++)
		{
			// left
			frameBuffer += ansi::MOVE_TO(RENDER_START_POS_FRAME_Y + 1 + i, (m_columns - MAP_WIDTH) / 2 - 1);
			frameBuffer += "║";

			// right
			frameBuffer += ansi::MOVE_TO(RENDER_START_POS_FRAME_Y + 1 + i, (m_columns + MAP_WIDTH) / 2 - 1);
			frameBuffer += "║";

		}
	}

	bool renderWinningScreen() {
		std::string prompt = "Congratulations - you have found the treasure!";
		int centerPos_X = (m_columns - prompt.size()) / 2;
		frameBuffer += ansi::MOVE_TO(RENDER_POS_Y, centerPos_X);
		frameBuffer += ansi::CLEAR_ALL_AFTER;
		frameBuffer += ansi::YELLOW;
		frameBuffer += prompt;
		std::cout << frameBuffer << std::flush;

		for (int i = 5; i > 0; i--)
		{
			frameBuffer.clear();
			frameBuffer += ansi::MOVE_TO(RENDER_POS_Y + 1, m_columns / 2);
			frameBuffer += std::to_string(i);
			std::cout << frameBuffer << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

		frameBuffer.clear();
		prompt = "Press any key to restart or [ESC] to quit the game";
		centerPos_X = (m_columns - prompt.size()) / 2;
		frameBuffer += ansi::MOVE_TO(RENDER_POS_Y + 1, centerPos_X);
		frameBuffer += prompt;
		std::cout << frameBuffer << std::flush;

		int key = _getch();


		// escape = quit game
		if (key == 27) {
			return false;
		}
		reset();
		return true;
	}

	bool renderLosingScreen() {
		std::string prompt = "Time's up! You lost.";
		int centerPos_X = (m_columns - prompt.size()) / 2;
		frameBuffer += ansi::MOVE_TO(RENDER_POS_Y, centerPos_X);
		frameBuffer += ansi::CLEAR_ALL_AFTER;
		frameBuffer += ansi::RED;
		frameBuffer += prompt;
		std::cout << frameBuffer << std::flush;

		for (int i = 5; i > 0; i--)
		{
			frameBuffer.clear();
			frameBuffer += ansi::MOVE_TO(RENDER_POS_Y + 1, m_columns / 2);
			frameBuffer += std::to_string(i);
			std::cout << frameBuffer << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

		frameBuffer.clear();
		prompt = "Press any key to restart or [ESC] to quit the game";
		centerPos_X = (m_columns - prompt.size()) / 2;
		frameBuffer += ansi::MOVE_TO(RENDER_POS_Y + 1, centerPos_X);
		frameBuffer += prompt;
		std::cout << frameBuffer << std::flush;


		int key = _getch();
		// escape = quit game
		if (key == 27) {
			return false;
		}
		reset();
		return true;
	}

	void renderMenu() {
		PlaySound(L"soundtrack.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		int centerPos_X = (m_columns - ansi::EMBLEM_WIDTH) / 2;

		// render emblem
		for (int i = 0; i < std::size(ansi::EMBLEM); i++)
		{
			std::cout << ansi::MOVE_TO(i + 1, centerPos_X) << ansi::EMBLEM[i];
			std::this_thread::sleep_for(std::chrono::milliseconds(35));
		}
		std::string  prompt = "Press [ENTER] to start";
		std::cout << ansi::MOVE_TO(RENDER_POS_Y, (m_columns - prompt.size()) / 2) << prompt << "\n" << ansi::GREEN;

		std::cin.get();

		startTime = std::chrono::steady_clock::now();
		state = GAME;
	}

	void renderMap() {

		int centerPos_X = (m_columns - MAP_WIDTH) / 2;

		for (int y = 0; y < map.size(); y++)
		{
			frameBuffer += ansi::MOVE_TO(RENDER_START_POS_MAP_Y + y, centerPos_X);
			for (int x = 0; x < map[0].size(); x++)
			{
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

				// print symbols near by in normal color
				frameBuffer += colorNear;
				frameBuffer += symbol;
				exploredMap[y][x] = true;
			}
		}
	}

	void renderTimeLeft() {
		int timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime).count();
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
};


int main() {

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int window_columns, window_rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	window_columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	window_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	Game game(window_columns, window_rows);

	system("chcp 65001 > nul");

	// settings
	std::cout << ansi::CURSOR_OFF; // switch off cursor
	std::cout << ansi::GREEN; // set color to green

	bool running = true;
	while (running) {

		std::cout << ansi::CURSOR_OFF; // switch off cursor, in case it got switched back on (for example when resizing)

		if (_kbhit()) {           // returns true if a keyinput is waiting to be processed
			int key = _getch();   // read one keypress, no Enter needed
			game.update(key);
		}
		running = game.render();

		std::this_thread::sleep_for(std::chrono::milliseconds(33));// simple approach to get 30ish fps
	}
}
