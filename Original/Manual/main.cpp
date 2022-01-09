
#include <opencv2/opencv.hpp>
#include "CControlPi.hpp"
#include "pigpio.h"
#include "wiringPi.h"
#include "server.h"
#include "libSonar.h"
#include "CVision.hpp"
#include <thread>
#include <iostream>
#include "raspicam_cv.h"

#define trigger1 15
#define echo1 3

#define trigger_left	2
#define echo_left		12

#define trigger_right	13
#define echo_right		14

using namespace std;
using namespace cv;

Server serv;
void serverfunc();
void serverimagefunc();

CControlPi ctrl;

void sonarPing();
void status();
void corr();
void input_ch();

float distFront;
float distRight;
float distLeft;

int result = 0;
string ch;
int run = 0;
int go = 0;
CVision vision;
bool thread_pause;
mutex mtx;

enum mode_state { MANUAL = 0, AUTO };

cv::Mat image;
cv::Mat crop;

int main()
{
	Mat frame;
	string tx;
	int input = 0;
	
	serv._exit = false;
	std::thread t1(&serverfunc);
	t1.detach();	// Start server thread and detach
	
	// Wait until server starts up
	delay(3000);

	std::thread t2(&serverimagefunc);
	t2.detach();	// Start image thread and detach
	
	delay(2000);
	
	std::thread t3(&sonarPing);
	t3.detach();	// Start sonar measurement thread and detach
	
	std::thread t4(&status);
	t4.detach();
	
	std::thread t5(&input_ch);
	t5.detach();
	
	std::thread t6(&corr);
	t6.detach();
	
	cout << "\nServer Running. Send '`' from Client to exit.\n";
	
    while (serv.str[0] != '`' && ch != "`")
	{
		
		ctrl.get_data(DIGITAL, SW, input);
					
		thread_pause = false;		// Run image transmission thread
		
				
		if (input)
			ctrl.stop();
		else if (serv.str[0] == 't')
			go = 1;
		else
		{
			if (serv.str[0] == 'x')
				go = 0;
			
			ctrl.readInput(serv.str);	// Read manual inputs and control car
			
		}
	}

	ctrl.stop();
	
	//serv._exit = true;
	delay(500);
	return 0;
}


// Start TCP server
void serverfunc()
{
	serv.start(4618);
}

// Send image to TCP server
void serverimagefunc()
{
	raspicam::RaspiCam_Cv Camera;
    Camera.open();
	delay(1000);		// Open camera and give time for setup
	
	Camera.grab();
	Camera.retrieve ( image );	// Take new image from camera
	
	// Create rectangle object for cropping
	Rect box;
	box.x = 0;
	box.y = 400;	// Set box starting points
	box.width = image.size().width;
	box.height = image.size().height - box.y;	// Set box dimensions
	
	do
	{	
		Camera.grab();
		Camera.retrieve ( image );	// Take new image from camera
		flip(image, image, 0);		// Mirror vertically
		flip(image, image, 1);
		resize(image(box), crop, Size(), 0.45, 0.45);
		
		
		if(thread_pause == false)	// If not paused, set image for transmission
		{
			serv.set_txim(crop);
		}
		
	} while (serv._exit != true);
	Camera.release();
	return;
}

void sonarPing()
{
	Sonar sonar1, sonar_left, sonar_right;
	
	sonar1.init(trigger1, echo1);
	sonar_left.init(trigger_left, echo_left);
	sonar_right.init(trigger_right, echo_right);
	
    float sample[3], sample_R[2], sample_L[2];
    
    do 
    {
		
		//delay(5);
		
		sample[2] = sample[1];
		sample[1] = sample[0];
		sample[0] = sonar1.distance(20000);		// Shift samples
		
		if (sample[0] < sample[1] && sample[0] > sample[2])
		{
			distFront = sample[0];
		}
		else if(sample[0] > sample[1] && sample[0] < sample[2])
		{
			distFront = sample[0];
		}
		else if(sample[1] > sample[0] && sample[1] < sample[2])
		{
			distFront = sample[1];
		}
		else if(sample[1] > sample[2] && sample[1] < sample[0])
		{
			distFront = sample[1];
		}
		else if(sample[2] > sample[0] && sample[2] < sample[1])
		{
			distFront = sample[2];
		}
		else if(sample[2] > sample[1] && sample[2] < sample[0])
		{
			distFront = sample[2];
		}
	
		if(distFront > 600)		// Sensor reads large distance when actually at zero
			distFront = 0;
		if(distRight > 600)		// Sensor reads large distance when actually at zero
			distRight = 0;
		if(distLeft > 600)		// Sensor reads large distance when actually at zero
			distLeft = 0;	
		mtx.unlock();
		
		/*if ((ctrl.drive == FORWARD) && distLeft <= 2.5)
		{
			mtx.lock();
			ctrl.correct(RIGHT);
			cout << "RIGHT, distLeft: " << distLeft << "\n";
			delay(100);
			mtx.unlock();
		}
		else if ((ctrl.drive == FORWARD) && distRight <= 2.5)
		{
			mtx.lock();
			ctrl.correct(LEFT);
			cout << "LEFT, distRight: " << distRight << "\n";
			delay(100);
			mtx.unlock();
		}*/
		
		
		if(distFront < 12.5 && ctrl.drive == FORWARD && go == 0)
		{
			mtx.lock();
			ctrl.stop();
			ctrl.drive = STOP;
			mtx.unlock();
		}
		
		if(go)
		{
			ctrl.drive = FORWARD;
			ctrl.forward();
			ctrl.setSpeed(85);
		}
		
		if(ctrl.drive == FORWARD && distFront < 25)
		{
			ctrl.setSpeed(65);
		}
		else if (ctrl.drive == FORWARD && distFront > 30)
		{
			ctrl.setSpeed(85);
		}
		
		
			
	
	} while(serv._exit != true);
	return;
}

void status()
{
	do
	{
		delay(10);
		gpioWrite(LED, gpioRead (STBY) );
	} while(serv._exit != true);
	return;
}

void input_ch()
{
	cout << "Manual Mode.";
	ch = "0";
	do
	{
		cin >> ch;
	}while(serv._exit != true);
	return;	
}

void corr()
{
	do
	{
		delay(1);
		vision.frame = crop.clone();
			
		result = vision.nav_vid();
			
		if (distFront > 15 && ctrl.drive == FORWARD)
		{
			if (result != 0)
			{
				if((vision.bound.x - (vision.frame.size().width - (vision.bound.x + vision.bound.width))) > 40)
				{
					cout << "\nRIGHT";
					ctrl.correct(RIGHT);
				}
					
				else if (((vision.frame.size().width - (vision.bound.x + vision.bound.width)) - vision.bound.x) > 40)
				{
					cout << "\nLEFT";
					ctrl.correct(LEFT);
				}
			}
		}
	}while(serv._exit != true);
	return;
}
