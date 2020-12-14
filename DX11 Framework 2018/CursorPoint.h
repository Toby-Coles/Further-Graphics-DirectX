#pragma once

class CursorPoint {

public:
	CursorPoint() : m_x(0), m_y(0)
	{

	}
	CursorPoint(int x, int y) :m_x(x), m_y(y) {
	}

	

public:
	int m_x = 0, m_y = 0;
};