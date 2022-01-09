#pragma once
#include <string>

using namespace std;

enum TYPE { DIGITAL = 0, ANALOG, SERVO };
enum DIR { LEFT = 0, RIGHT};

#define Ai1 6
#define Ai2 5
#define PWMa 30		// Control pins for motor A

#define Bi1	19
#define Bi2 26
#define PWMb 29		// Control pins for motor B

#define STBY 13		// Motor controller standby pin

#define LED 4 		// BCM pin for PiHAT LED
#define SW	17		// BCM pin for PiHAT pushbutton

#define SERVO_MAX 2500
#define SERVO_MIN 700

enum drive_state { STOP = 0, FORWARD, REVERSE };

class CControlPi
{
private:
	int lastval[5];  ///< Stores previous button state for de-bouncing
	
public:
	CControlPi();
	int drive = STOP;
	bool get_data(int type, int channel, int &value);
	bool set_data(int type, int channel, int value);
	int get_button(int channel, int index);
	int get_analog(int channel, int &value);
	void print_menu(void);
	
	void turn(int dir);
	void small_turn(int dir);
	void forward(void);
	void reverse(void);
	void stop(void);
	void correct(int dir);
	void setSpeed(int val);
	void readInput(string input);
	
};
