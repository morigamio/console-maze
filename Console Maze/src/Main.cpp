#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include "Ansi.h"

class Game {

	enum game_state {
		MENU, GAME, WIN, LOSE
	};

	game_state state = GAME;

public:


	static constexpr int darkZoneRadius = 2;

	static constexpr int PADDING = 5;

	static constexpr int EMBLEM_H = 10;

	static constexpr int STAT_PANEL_WIDTH = 20;
	static constexpr int INSTRUCTION_PANEL_H = 5;
	static constexpr int BUFF_PANEL_H = 5;
	static constexpr int ITEM_PANEL_H = 5;

	static constexpr int MAP_H = 18;
	static constexpr int MAP_W = 53;

	const int treasurePosX = 3; //52
	const int treasurePosY = 10; //16

	int playerPosX = 0;
	int playerPosY = 1;

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

		bool reachedFinish = playerPosX + tmpPosX == treasurePosX && playerPosY + tmpPosY == treasurePosY;
		if (reachedFinish) {
			state = WIN;
		}

		playerPosX += tmpPosX;
		playerPosY += tmpPosY;
	}

	void renderTest() {
		ansi::MOVE_TO(1, 1);
		frameBuffer += R"(╔═╗)";
	}

	void render() {
		frameBuffer.clear();
		renderMap();
		//renderTest();

		// print current frame
		std::cout << ansi::TO_SAVED_POS << frameBuffer << std::flush;
	}

	void renderMap() {
		frameBuffer += ansi::MOVE_TO(1, 1);
		frameBuffer += R"(╔═════════════════════════════════════════════════════╗)";
		for (int y = 0; y < map.size(); y++)
		{
			frameBuffer += ansi::MOVE_TO(y + 2, 1);
			frameBuffer += "║";
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
				bool inDarkZone = std::max(std::abs(dx), std::abs(dy * 2)) > darkZoneRadius;
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
			frameBuffer += "║";
		}
		frameBuffer += ansi::MOVE_TO(MAP_H + 2, 1);
		frameBuffer += R"(╚═════════════════════════════════════════════════════╝)";
	}

	bool isWon() {
		return state == WIN;
	}

	void reset() {
		playerPosX = 0;
		playerPosY = 1;

		for (auto& row : exploredMap) {
			std::fill(row.begin(), row.end(), false);
		}

		state = GAME;
	}
};


int main() {

	Game game;

	system("chcp 65001 > nul");

	// settings
	std::cout << ansi::CURSOR_OFF; // switch off cursor
	std::cout << ansi::GREEN; // set color to green

	// render emblem
	for (const auto& line : ansi::EMBLEM) {
		std::cout << line << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(35));
	}

	std::cout << ansi::SAVE_CURSOR_POS; // save render location
	std::cout << "Press [ENTER] to start" << "\n" << ansi::GREEN;

	std::cin.get();

	std::cout << ansi::TO_SAVED_POS << ansi::CLEAR_BELOW;

	bool running = true;
	while (running) {

		std::cout << ansi::CURSOR_OFF; // switch off cursor, in case it got switched back on (for example when resizing)

		if (_kbhit()) {           // returns true if a keyinput is waiting to be processed
			int key = _getch();   // read one keypress, no Enter needed
			game.update(key);
		}
		game.render();

		if (game.isWon()) {
			std::cout << ansi::TO_SAVED_POS << ansi::CLEAR_BELOW;
			std::cout << ansi::GREEN << "Congratulations - you have found the treasure! Press [ENTER] to restart or [ESC] to quit the game";
			int key = _getch();

			// enter = restart game
			if (key == 13) {
				std::cout << ansi::TO_SAVED_POS << ansi::CLEAR_BELOW;
				game.reset();
			}

			// escape = quit game
			if (key == 27) {
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(33));// simple approach to get 30ish fps
	}
}
