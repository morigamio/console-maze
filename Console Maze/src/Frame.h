#pragma once
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "Ansi.h"

enum class Junction {
	CROSS, T_DOWN, T_UP, T_LEFT, T_RIGHT, CORNER_TL,
	CORNER_TR, CORNER_BL, CORNER_BR
};

static const std::map<Junction, std::string_view> symbol = {
	 { Junction::CORNER_TL, "╔" },
	 { Junction::CORNER_TR, "╗" },
	 { Junction::CORNER_BL, "╚" },
	 { Junction::CORNER_BR, "╝" },
	 { Junction::T_DOWN,    "╦" },
	 { Junction::T_UP,      "╩" },
	 { Junction::T_LEFT,    "╣" },
	 { Junction::T_RIGHT,   "╠" },
	 { Junction::CROSS,     "╬" },
};

struct JunctionPoint { int x, y; Junction type; };

class FrameBuilder {
private:
	std::vector<JunctionPoint> m_points;
public:
	void add(int x, int y, Junction type) {
		m_points.push_back({ x, y, type });
	}

	// draws ═ between same-Y points, ║ between same-X points according to the junction points provided and returns the resulting string to be printed to the console
	std::string build() {
		std::string frame;

		std::map<int, std::vector<JunctionPoint>> sort_by_x; // conjunction points to vertically connect, sorted by X coordinate
		std::map<int, std::vector<JunctionPoint>> sort_by_y; // conjunction points to horiztonally connect, sorted by Y coordinate

		bool connect = true; // to toggle connection between points, so that gaps can be created in the frame 

		for (const auto& point : m_points) {
			sort_by_x[point.x].push_back(point);
			sort_by_y[point.y].push_back(point);
		}

		// connect points in the same column with ║ or leave a gap according to the junction types
		for (auto& [x, points] : sort_by_x) {
			std::sort(points.begin(), points.end(), compY); // sort by Y coordinate

			// connect points in the same column with ║ or leave a gap according to the junction types
			int j = 0; // index for the current conjunction point
			for (int i = points[0].y; i <= points.back().y; i++)
			{
				JunctionPoint& point = points[j];
				frame += ansi::MOVE_TO(i, x);
				if (i == point.y) {
					frame += symbol.at(point.type);
					connect = (point.type == Junction::CROSS || point.type == Junction::T_DOWN || point.type == Junction::T_RIGHT || point.type == Junction::T_LEFT || point.type == Junction::CORNER_TL || point.type == Junction::CORNER_TR);
					j++;
				}
				else {
					frame += connect ? "║" : " ";
				}
			}
		}

		for (auto& [y, points] : sort_by_y) {
			std::sort(points.begin(), points.end(), compX); // sort by X coordinate

			// connect points in the same row with ═ or leave a gap according to the junction types
			int j = 0; // index for the current conjunction point
			for (int i = points[0].x; i <= points.back().x; i++)
			{
				JunctionPoint& point = points[j];
				frame += ansi::MOVE_TO(y, i);
				if (i == point.x) {
					frame += symbol.at(point.type);
					connect = (point.type == Junction::CROSS || point.type == Junction::T_DOWN || point.type == Junction::T_UP || point.type == Junction::T_RIGHT || point.type == Junction::CORNER_TL || point.type == Junction::CORNER_BL);
					j++;
				}
				else {
					frame += connect ? "═" : " ";
				}
			}
		}
		return frame;
	};

private:
	static bool compX(JunctionPoint& a, JunctionPoint& b)
	{
		return a.x < b.x;
	}

	static bool compY(JunctionPoint& a, JunctionPoint& b)
	{
		return a.y < b.y;
	}

};