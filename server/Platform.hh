#pragma once

class Platform
{
public:
	Platform(double x_pos, double y_pos, double w, double h);

	bool isInside(double x, double y);

	inline double getY() const { return y_pos; }
	inline double getH() const { return h; }

private:

	double x_pos, y_pos, w, h;
};
