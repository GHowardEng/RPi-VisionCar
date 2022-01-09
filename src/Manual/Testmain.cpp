
#include "CControlPi.hpp"
#include "pigpio.h"
#include "wiringPi.h"
#include <iostream>

int main()
{
	CControlPi ctrl;
	ctrl.setSpeed(90);

	delay(2000);

	ctrl.forward();	
			
	delay(2000);
	
	ctrl.reverse();

	delay(2000);

	//ctrl.setSpeed(45);
	//delay(1000);
	
	
	
	delay(10);
	ctrl.turn(RIGHT);
	ctrl.forward();
	delay(1000);
	ctrl.turn(LEFT);
	ctrl.turn(LEFT);
	
	
//	ctrl.turn(RIGHT);
//	ctrl.turn(LEFT);
	ctrl.stop();
	delay(500);
	
	return 0;
}
