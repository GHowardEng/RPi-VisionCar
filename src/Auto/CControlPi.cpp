
#include "CControlPi.hpp"
#include "pigpio.h"
#include "wiringPi.h"
#include "softPwm.h"

#include <iostream>

#define max 65
using namespace std;

CControlPi::CControlPi()
{
	wiringPiSetup();
	softPwmCreate (PWMa, 100, 100);
	softPwmCreate (PWMb, 100, 100);
	
	gpioInitialise();
	gpioSetMode(Ai1, PI_OUTPUT);
	gpioSetMode(LED, PI_OUTPUT);
	gpioSetMode(SW,	 PI_INPUT);
	gpioSetMode(Ai2, PI_OUTPUT);
	gpioSetMode(Bi1, PI_OUTPUT);
	gpioSetMode(Bi2, PI_OUTPUT);
	gpioSetMode(STBY, PI_OUTPUT);
	
	//CControlPi::stop();
}
bool CControlPi::get_data(int type, int channel, int &value)
{
	if (type == DIGITAL)
	{
		value = gpioRead (channel);
		return true;
	}
	
	else if (type == ANALOG)
	{
		unsigned char inBuf[3];
		char cmd[] = { 1, 128, 0 };
		
		int handle = spiOpen(channel, 200000, 3);
		
		spiXfer(handle, cmd, (char*) inBuf, 3);
		value = ((inBuf[1] & 3) << 8) | inBuf[2];
		
		spiClose(handle);
		
		return true;
	}
	return false;
}

bool CControlPi::set_data(int type, int channel, int value)
{
	if (type == DIGITAL)
	{
		gpioWrite(channel, value);
		return true;
	}
	
	else if (type == SERVO)
	{
		gpioServo(channel, value);
		return true;
	}
	
	return false;
}

int CControlPi::get_analog(int channel, int &value)
{
	if (get_data(ANALOG, channel, value))
	{
		return (100 * (value)) / 1023;
	}
	return false;

}

int CControlPi::get_button(int channel, int index)
{
	int value;
	get_data(DIGITAL, channel, value);

	if (value != lastval[index])
	{
		lastval[index] = value;
		delay(2);
		
		get_data(DIGITAL, channel, value);
		if (value == 0)
		{
			return 1;
		}
	}
	

	return 0;
}

void CControlPi::print_menu(void)
{

	cout << "\n\n******************************";
	cout << "\n\n(B)utton Count";
	cout << "\n(D)igital Input/Output";
	cout << "\n(A)nalog Inputs";
	cout << "\n(S)ervo Test";
	cout << "\n(C)ontrol Servo";
	cout << "\n(Q)uit";
	cout << "\n\nCommand> ";

}

void CControlPi::forward(void)
{	
	CControlPi::setSpeed(0);
	CControlPi::set_data(DIGITAL, STBY, 1);		// Take out of standby
	
	CControlPi::set_data(DIGITAL, Ai1, 0);		// Activate motor (CCW)
	CControlPi::set_data(DIGITAL, Ai2, 1);
	
	CControlPi::set_data(DIGITAL, Bi1, 1);		// Activate motor (CW)
	CControlPi::set_data(DIGITAL, Bi2, 0);		// Directions reversed due to motor placement
	
	if (CControlPi::drive != FORWARD){
		cout << "accel\n";
		for (int i = 5; i < max; i+=5){
			 softPwmWrite(PWMa, i );
			softPwmWrite(PWMb, i - 3);
			delay(10);
		}
	}
	softPwmWrite(PWMa, max );
	softPwmWrite(PWMb, max - 3);
	
}

void CControlPi::reverse(void)
{
	//CControlPi::stop();
	
	softPwmWrite(PWMa, max - 15 );
	softPwmWrite(PWMb, max - 21);
	
	CControlPi::set_data(DIGITAL, STBY, 1);		// Take out of standby
			
	CControlPi::set_data(DIGITAL, Ai1, 1);		// Activate motor (CW)
	CControlPi::set_data(DIGITAL, Ai2, 0);
	
	CControlPi::set_data(DIGITAL, Bi1, 0);		// Activate motor (CCW)
	CControlPi::set_data(DIGITAL, Bi2, 1);		// Directions reversed due to motor placement
}

void CControlPi::stop(void)
{	
	if(CControlPi::drive != STOP)
	{
		for(int i = 40; i > 0; i-=5){
			CControlPi::setSpeed(i);
			delay(5);
		}
	}
	
		CControlPi::set_data(DIGITAL, Bi1, 0);		// De-activate motor
		CControlPi::set_data(DIGITAL, Bi2, 0);
		delay(80);
		
		CControlPi::set_data(DIGITAL, Ai1, 0);		// De-activate motor
		CControlPi::set_data(DIGITAL, Ai2, 0);
		
		CControlPi::set_data(DIGITAL, STBY, 0);		// Put in standby
		delay(40);
}

void CControlPi::turn(int dir)
{
	
	CControlPi::set_data(DIGITAL, STBY, 1);		// Take out of standby
		
	if (dir == RIGHT)
	{
		CControlPi::setSpeed(61);
		CControlPi::set_data(DIGITAL, Ai1, 0);		// Activate motor (CW)
		CControlPi::set_data(DIGITAL, Ai2, 0);
		
		CControlPi::set_data(DIGITAL, Bi1, 1);		// Activate motor (CW)
		CControlPi::set_data(DIGITAL, Bi2, 0);
		delay(548);
	}
	
	else if (dir == LEFT)
	{
		CControlPi::setSpeed(61);
		CControlPi::set_data(DIGITAL, Ai1, 0);		// Activate motor (CW)
		CControlPi::set_data(DIGITAL, Ai2, 1);
		
		CControlPi::set_data(DIGITAL, Bi1, 0);		// Activate motor (CW)
		CControlPi::set_data(DIGITAL, Bi2, 0);
		delay(563);
	}
	
	CControlPi::set_data(DIGITAL, Bi1, 0);		// De-activate motor
	CControlPi::set_data(DIGITAL, Bi2, 0);
	
	
	CControlPi::set_data(DIGITAL, Ai1, 0);		// De-activate motor
	CControlPi::set_data(DIGITAL, Ai2, 0);
	
	CControlPi::set_data(DIGITAL, STBY, 0);		// Put in standby
}

void CControlPi::correct(int dir)		// Reduce motor speed on one side for corrective turn
{
	if (dir == RIGHT)
	{
		softPwmWrite(PWMa, max - 38);
		softPwmWrite(PWMb, max + 5);
	}
	
	else if (dir == LEFT)
	{
		softPwmWrite(PWMb, max - 38);
		softPwmWrite(PWMa, max + 5);
	}
	
	delay(65);
	
	CControlPi::forward();
}
void CControlPi::setSpeed(int val)
{
	if ((val >= 0) && (val < 70))
	{
		softPwmWrite(PWMa, val);
		softPwmWrite(PWMb, val -3);
	}
	else 
	{
		softPwmWrite(PWMa, val);
		softPwmWrite(PWMb, val -5);
	}
		
}

// Read input string and activate controls
// w - move forward
// s - stop / reverse
// a - turn left
// d - turn right
// x - stop
void CControlPi::readInput(string input)
{
		
	if (input[0] == 'x')
			{
				CControlPi::stop();
				drive = STOP;
			}
			
			else if (input[0] == 'w')
			{
				if (drive == REVERSE)
				{
					
					CControlPi::stop();
					drive = STOP;
					delay(400);
				}
				
				else
				{
					
					CControlPi::forward();
					drive = FORWARD;
				}
			}
			
			else if (input[0] == 's')
			{
				if (drive == FORWARD)
				{
					CControlPi::stop();
					drive = STOP;
					delay(400);
				}
				
				else
				{
					drive = REVERSE;
					CControlPi::reverse();
				}
			}
			
			else if (input[0] == 'd')
			{
				if (drive == FORWARD)
				{
					CControlPi::correct(RIGHT);
				}
				else
				{
					drive = STOP;
					CControlPi::turn(RIGHT);
				}
			}
			
			else if (input[0] == 'a')
			{
				if (drive == FORWARD)
				{
					CControlPi::correct(LEFT);
				}
				
				else
				{
					drive = STOP;
					CControlPi::turn(LEFT);
				}
			}
			
}
