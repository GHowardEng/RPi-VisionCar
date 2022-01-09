
#include "CBase4618.hpp"


void CBase4618::run()
{
	do
	{
		update();
		draw();
	} while (waitKey(1) != 'q');
}

void CBase4618::update()
{
	// To be overloaded in child class
}

void CBase4618::draw()
{
	// To be overloaded in child class
}
