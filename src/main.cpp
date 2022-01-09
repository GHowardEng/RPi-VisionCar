
#include <opencv2/opencv.hpp>
#include "CControlPi.hpp"
#include "pigpio.h"
#include "wiringPi.h"
#include "server.h"
#include "CVision.hpp"
#include "libSonar.h"
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

float distFront;
float distRight;
float distLeft;

int result = 0;

int run = 0;

bool thread_pause;
mutex mtx;

enum mode_state { MANUAL = 0, AUTO };
int mode = MANUAL;

cv::Mat image;
cv::Mat crop;

int main()
{
	CVision vision;
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
	
	cout << "\nServer Running. Send 'q' from Client to exit.\n";
	
    while (serv.str[0] != 'q')
	{
		crop.copyTo(vision.frame); 		// Input new camera image
		//vision.frame = image;
		ctrl.get_data(DIGITAL, SW, input);
		
	
		if (serv.str[0] == 'm')		// Switch modes when 'm' received
		{
			if (mode == MANUAL)
			{
				mode = AUTO;
				
				serv.reply = "AUTO MODE";
				cout << "\n" << serv.reply << "\n";
				send(serv.clientsock, serv.reply.c_str(), serv.reply.length(), 0);
			}
			else
			{
				mode = MANUAL;
				
				serv.reply = "MANUAL MODE";
				cout << "\n" << serv.reply << "\n";
				send(serv.clientsock, serv.reply.c_str(), serv.reply.length(), 0);
				ctrl.drive = STOP;
			}
			delay(1000);
		}
		
		if (mode == MANUAL)				// If in manual mode
		{
						
			thread_pause = false;		// Run image transmission thread
			delay(1);
			
			if(distFront < 10.5 && ctrl.drive == FORWARD)
			{
				ctrl.stop();
				ctrl.drive = STOP;
			}
			
			if (input)
				ctrl.stop();
			else
				ctrl.readInput(serv.str);	// Read manual inputs and control car
		}
		
		else 						// Else run in automatic mode
		{
			thread_pause = true; 	// Pause image transmission thread
		
			
			delay(100);
			cout << "Front: " << distFront << " cm. Left: " << distLeft << " cm. Right: " << distRight << "\n";
			
			
			if (serv.str[0] == 'r' || (run == 0 && input))
			{
				run = 1;
				ctrl.set_data(DIGITAL, LED, 1);
				
			}
			
			if (serv.str[0] == 'x' || (run == 1 && input))
			{
				run = 0;
				ctrl.stop();
				ctrl.drive = STOP;
				ctrl.set_data(DIGITAL, LED, 0);
				
			}
			
			if (run == 1)
			{
				if (distFront < 20)
					result = vision.nav_vid(); // Process image and detect colour
					serv.set_txim(vision.im);	// Transmit processed image	
					//imshow("vision", vision.im);
					//waitKey(1);
				if (ctrl.drive == STOP)
				{

					

				}
				
					
			}
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
	box.x = 100;
	box.y = 200;	// Set box starting points
	box.width = image.size().width - 2*box.x;
	box.height = image.size().height - 1.15*box.y;	// Set box dimensions
	
	do
	{	
		Camera.grab();
		Camera.retrieve ( image );	// Take new image from camera
		crop = image(box);
		flip(crop, crop, 0);		// Mirror vertically
		flip(crop, crop, 1);
		
		
		if(thread_pause == false)	// If not paused, set image for transmission
		{
			serv.set_txim(crop);
		}
		
	} while (serv._exit != true);
}

void sonarPing()
{
	Sonar sonar1, sonar_left, sonar_right;
	
	sonar1.init(trigger1, echo1);
	sonar_left.init(trigger_left, echo_left);
	sonar_right.init(trigger_right, echo_right);
	
    float sample[2], sample_R[2], sample_L[2];
    
    do 
    {
		//delay(5);
		
		/*sample[1] = sample[0];
		sample[0] = sonar1.distance(25000);		// Shift samples
		
		sample_R[1] = sample_R[0];
		sample_R[0] = sonar_right.distance(25000);		// Shift samples

		sample_L[1] = sample_L[0];
		sample_L[0] = sonar_left.distance(25000);		// Shift samples*/
		
		mtx.lock();
		/*distFront = (sample[0] + sample[1]) / 2; // Apply moving to average samples
		distLeft = (sample_L[0] + sample_L[1]) / 2; // Apply moving to average samples
		distRight = (sample_R[0] + sample_R[1]) / 2; // Apply moving to average samples
		*/
		distFront = sonar1.distance(25000);
		distRight = sonar_right.distance(25000);
		distLeft = sonar_left.distance(25000);
	
		if(distFront > 600)		// Sensor reads large distance when actually at zero
			distFront = 0;
		if(distRight > 600)		// Sensor reads large distance when actually at zero
			distRight = 0;
		if(distLeft > 600)		// Sensor reads large distance when actually at zero
			distLeft = 0;	
		mtx.unlock();
		
		if ((ctrl.drive == FORWARD) && distLeft <= 2.2)
		{
			mtx.lock();
			ctrl.correct(RIGHT);
			cout << "RIGHT, distLeft: " << distLeft << "\n";
			delay(100);
			mtx.unlock();
		}
		else if ((ctrl.drive == FORWARD) && distRight <= 2.2)
		{
			mtx.lock();
			ctrl.correct(LEFT);
			cout << "LEFT, distRight: " << distRight << "\n";
			delay(100);
			mtx.unlock();
		}
		
		if(distFront < 11.5 && ctrl.drive == FORWARD)
		{
			mtx.lock();
			ctrl.stop();
			ctrl.drive = STOP;
			mtx.unlock();
	

			if (result == GREEN)	// Turn based on navigation colour in picture
			{
				ctrl.drive = STOP;
				cout << "\nGreen\n";
				ctrl.turn(RIGHT);
			}
			
			else if (result == RED)
			{
				ctrl.drive = STOP;
				cout << "\nRed\n";
				ctrl.turn(LEFT);
			}
		}
			
		else if (distFront > 25.0 && mode == AUTO && run )
		{
			mtx.lock();
			ctrl.forward();
			ctrl.drive = FORWARD;
			mtx.unlock();
			
		}
	
	} while(serv._exit != true);
}

void status()
{
	do
	{
		while(mode != MANUAL){}
		delay(10);
		gpioWrite(LED, gpioRead (STBY) );
	} while(serv._exit != true);
}
