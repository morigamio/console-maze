#pragma once
#include <string>
#include <vector>

class Map {
public:
private:
	std::vector<std::string> m_tiles;
	std::vector<std::vector<bool>> m_explored;
	bool m_dirty;

public:
	int width()  const { return (int)m_tiles[0].size(); }
	int height() const { return (int)m_tiles.size(); }
	char tile(int x, int y)     const { return m_tiles[y][x]; }
	bool explored(int x, int y) const { return m_explored[y][x]; }
	void markExplored(int x, int y) {
		if (!m_explored[y][x]) {
			m_explored[y][x] = true;
			m_dirty = true;       // something changed → signal re-render
		}
	}

	bool isDirty()    const { return m_dirty; }
	void clearDirty() { m_dirty = false; }

	void reset() {
		for (auto& row : m_explored)
			std::fill(row.begin(), row.end(), false);
		m_dirty = true;
	}
};