#pragma once
#include <string_view>

namespace ansi {
	// colors
	inline constexpr std::string_view GREEN = "\033[1;32m";
	inline constexpr std::string_view DARK_GREEN = "\033[38;2;0;50;0m";
	inline constexpr std::string_view ORANGE = "\033[38;5;208m";
	inline constexpr std::string_view RED = "\033[1;31m";
	inline constexpr std::string_view YELLOW = "\033[1;33m";
	inline constexpr std::string_view DARK_YELLOW = "\033[38;2;50;50;0m";
	inline constexpr std::string_view CYAN = "\033[1;36m";

	inline constexpr std::string_view RESET = "\033[0m";
	inline constexpr std::string_view CURSOR_OFF = "\033[?25l";
	inline constexpr std::string_view CURSOR_ON = "\033[?25h";
	inline constexpr std::string_view SAVE_CURSOR_POS = "\033[s";
	inline constexpr std::string_view TO_SAVED_POS = "\033[u";
	inline constexpr std::string_view HOME = "\033[H";
	inline constexpr std::string_view CLEAR_ALL_AFTER = "\033[J";

	inline constexpr int EMBLEM_WIDTH = 72;
	inline constexpr int EMBLEM_HEIGHT = 10;
	inline std::string EMBLEM[] = {
		"",
		"",
		R"( ███╗   ███╗ ██████╗ ██████╗ ██╗ ██████╗  █████╗ ███╗   ███╗██╗ ██████╗ )",
		R"( ████╗ ████║██╔═══██╗██╔══██╗██║██╔════╝ ██╔══██╗████╗ ████║██║██╔═══██╗)",
		R"( ██╔████╔██║██║   ██║██████╔╝██║██║  ███╗███████║██╔████╔██║██║██║   ██║)",
		R"( ██║╚██╔╝██║██║   ██║██╔══██╗██║██║   ██║██╔══██║██║╚██╔╝██║██║██║   ██║)",
		R"( ██║ ╚═╝ ██║╚██████╔╝██║  ██║██║╚██████╔╝██║  ██║██║ ╚═╝ ██║██║╚██████╔╝)",
		R"( ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚═╝ ╚═════╝)",
		"",
		""
	};

	// "\033[<row>;<col>H" — move cursor to absolute position
	inline std::string MOVE_TO(int row, int col) {
		return "\033[" + std::to_string(row) + ";" + std::to_string(col) + "H";
	}

}