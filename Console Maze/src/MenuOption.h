#pragma once
enum class MenuOption { START_GAME, SETTINGS, EXIT, COUNT};
enum class PauseMenuOption { RESUME, RESTART, EXIT, COUNT};

constexpr const char* to_string(MenuOption opt) {
    switch (opt) {
    case MenuOption::START_GAME: return "Start Game";
    case MenuOption::SETTINGS:  return "Settings";
    case MenuOption::EXIT:      return "Exit";
    }
}

constexpr const char* to_string(PauseMenuOption opt) {
	switch (opt) {
	case PauseMenuOption::RESUME:  return "Resume";
	case PauseMenuOption::RESTART: return "Restart";
	case PauseMenuOption::EXIT:    return "Exit";
	}
}
