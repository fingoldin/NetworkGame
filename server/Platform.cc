#include "Platform.hh"

Platform::Platform(double x_pos, double y_pos, double w, double h) : x_pos(x_pos), y_pos(y_pos), w(w), h(h)
{
}

bool Platform::isInside(double x, double y)
{
	return (x < (x_pos + w / 2.0) && x > (x_pos - w / 2.0) && y < (y_pos + h / 2.0) && y > (y_pos - h / 2.0));
}
