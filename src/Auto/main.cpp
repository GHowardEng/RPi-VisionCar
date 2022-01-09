
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "CControlPi.hpp"
#include "pigpio.h"
#include "wiringPi.h"
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


void imagefunc();

CControlPi ctrl;

void sonarPing();
void input_ch();


float distFront = 0;
float distRight = 0;
float distLeft = 0;

int result = 0;
int run = 0;
int turn = 0;
int right_count = 0;
string ch;

bool thread_pause;
bool thread_exit;

enum mode_state { MANUAL = 0, AUTO };

cv::Mat image;
cv::Mat crop;

int main()
{
	CVision vision;
	Mat frame;
	string tx;
	int input = 0;

	thread_exit = false;
	
	std::thread t1(&input_ch);
	t1.detach();
	
	std::thread t2(&imagefunc);
	t2.detach();	// Start image thread and detach
	
	waitKey(10);
	delay(2000);
	
	std::thread t3(&sonarPing);
	t3.detach();	// Start sonar measurement thread and detach
		
    while (ch != "q")
	{
		//vision.frame = crop;		// Input new camera image
		vision.frame = crop.clone();
		ctrl.get_data(DIGITAL, SW, input);
		
		thread_pause = true; 	// Pause image transmission thread
	
		result = vision.nav_vid(); // Process image and detect colour
		//cout << "Front: " << distFront << " cm. Left: " << distLeft << " cm. Right: " << distRight << "\n";

		imshow("result", vision.frame);
		waitKey(1);
	
		
		if (ch == "r")
		{
			run = 1;
			ctrl.set_data(DIGITAL, LED, 1);
		}
		
		if (ch == "x" || (run == 1 && input))
		{
			ch = "0";
			run = 0;
			right_count = 0;
			ctrl.stop();
			ctrl.drive = STOP;
			ctrl.set_data(DIGITAL, LED, 0);
		}
		


		
		//cout << "NAV\n";
		if (distFront > 15 && ctrl.drive == FORWARD)
		{
			if (result != 0)
			{
				if((vision.bound.x - (vision.frame.size().width - (vision.bound.x + vision.bound.width))) > 25)
				{
					cout << "\nRIGHT";
					ctrl.correct(RIGHT);
				}
					
				else if (((vision.frame.size().width - (vision.bound.x + vision.bound.width)) - vision.bound.x) > 25)
				{
					cout << "\nLEFT";
					ctrl.correct(LEFT);
				}
			}
		}
		

		if (result == GREEN)	// Turn based on navigation colour in picture
		{
			//cout << "\nGreen\n";
			//imshow("result", vision.frame);
			//waitKey(2);
			
			if(run && distFront < 13 && ctrl.drive == STOP)
			{
				turn = 1;
				ctrl.turn(RIGHT);
				right_count += 1;
				cout << right_count << "\n";
				while(distFront < 15){}
				turn = 0;
			}
			
			//cout << "\n" << vision.bound.x << "\n";
			//cout << vision.frame.size().width - (vision.bound.x + vision.bound.width) << "\n";
			
		}
		
		else if (result == RED)
		{
			
			//cout << "\nRed\n";
			//imshow("result", vision.frame);
			//waitKey(1);
			
			if(run && distFront < 13 && ctrl.drive == STOP)
			{
				turn = 1;
				ctrl.turn(LEFT);
				//cout << "\nRed\n";
				while(distFront < 15){}
				turn = 0;
			}
			
		}
					
		
	}

	ctrl.stop();
	cout << "Exit\n";
	
	delay(100);
	thread_exit = true;
	delay(500);
	ctrl.set_data(DIGITAL, LED, 0);
	return 0;
}



void imagefunc()
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

		//imshow("Camera", image);
		//waitKey(10);
	} while (thread_exit != true);
	
	Camera.release();
	return;
}

void sonarPing()
{
	Sonar sonar1, sonar_left, sonar_right;
	
	sonar1.init(trigger1, echo1);
	sonar_left.init(trigger_left, echo_left);
	sonar_right.init(trigger_right, echo_right);
	
    float sample[4], sample_R[2], sample_L[2];
    
    do 
    {
		
		//delay(5);
		sample[3] = sample[2];
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
		//cout << distFront << "\n";
			
		
		/*sample_R[1] = sample_R[0];
		sample_R[0] = sonar_right.distance(20000);		// Shift samples

		sample_L[1] = sample_L[0];
		sample_L[0] = sonar_left.distance(20000);		// Shift samples*/
		
		
		//distFront = (sample[0] + sample[1] + sample[2] + sample[3]) / 4; // Apply moving to average samples
		//distLeft = (sample_L[0] + sample_L[1]) / 2; // Apply moving to average samples
		//distRight = (sample_R[0] + sample_R[1]) / 2; // Apply moving to average samples 
		
		//distFront = sonar1.distance(25000);
		//distRight = sonar_right.distance(25000);
		//distLeft = sonar_left.distance(25000);
	
		if(distFront > 600)		// Sensor reads large distance when actually at zero
			distFront = 0;
		if(distRight > 600)		// Sensor reads large distance when actually at zero
			distRight = 0;
		if(distLeft > 600)		// Sensor reads large distance when actually at zero
			distLeft = 0;	
		
	if (distFront > 16 && run && !(turn))
	{
		//cout << "FWD\n";
		//ctrl.stop();
		ctrl.forward();
		ctrl.drive = FORWARD;	
	}
	else if(distFront < 11.5 && ctrl.drive == FORWARD && !(turn) && right_count < 6)
	{
		ctrl.stop();
		ctrl.drive = STOP;
		//delay(10);
	}
	if(ctrl.drive == FORWARD && distFront < 25 && right_count < 6)
	{
		ctrl.setSpeed(65);
	}
	else if (ctrl.drive == FORWARD && (distFront > 30 || right_count >= 6))
	{
		ctrl.setSpeed(85);
	}
	
	if (right_count >= 6 && distFront < 5)
	{
		ctrl.stop();
		ctrl.drive = STOP;
	}
		

	
					
	if ((ctrl.drive == FORWARD) && distLeft <= 2.5)
	{

	//ctrl.correct(RIGHT);
	//cout << "RIGHT, distLeft: " << distLeft << "\n";
	//delay(150);

	}
	else if ((ctrl.drive == FORWARD) && distRight <= 2.5)
	{
	
		//ctrl.correct(LEFT);
		//cout << "LEFT, distRight: " << distRight << "\n";
		//delay(150);
	
	}
	
		
	
	} while(thread_exit != true);
	
	return;
}

void input_ch()
{
	cout << "Auto mode running.";
	ch = "0";
	do
	{
		cin >> ch;
	}while(thread_exit != true);
	return;	
}
