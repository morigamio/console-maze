#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "Ansi.h"

class Game {

public:
	Game(int window_columns, int window_rows);
	void run();

private:

	static constexpr int TIME_LIMIT = 120;
	static constexpr int MAP_WIDTH = 54;
	static constexpr int MAP_HEIGHT = 19;
	static constexpr int RENDER_POS_Y = ansi::EMBLEM_HEIGHT;
	static constexpr int RENDER_START_POS_FRAME_Y = RENDER_POS_Y;
	static constexpr int RENDER_END_POS_FRAME_Y = RENDER_START_POS_FRAME_Y + MAP_HEIGHT;
	static constexpr int RENDER_START_POS_MAP_Y = RENDER_START_POS_FRAME_Y + 1;
	static constexpr int RENDER_START_POS_TIME_Y = RENDER_END_POS_FRAME_Y + 1;

	static constexpr int darkZoneRadius = 2;

	static const enum game_state {
		MENU, GAME, WIN, LOSE
	};

	bool running = true;

	std::string m_frame;
	game_state state = MENU;

	int m_columns = 0;
	int m_rows = 0;

	const int treasurePosX = 52;
	const int treasurePosY = 16;

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

	void update(int inputKey);
	void renderEmblem();
	void renderGame();
	void renderMenu();
	void renderMap();
	void renderFrame();
	void renderTimeLeft();
	void renderWinningScreen();
	void renderLosingScreen();
	bool isWon();
	bool isLost();
	void reset();
};
