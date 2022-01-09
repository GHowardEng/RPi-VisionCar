#pragma once

#define obj_area 1500

#include "wiringPi.h"

#include <opencv2/opencv.hpp>

#include <thread>

using namespace std;
using namespace cv;

enum colour {GREEN = 1, RED};

class CVision
{
public:
	CVision();
	~CVision();
	bool in_frame();
	int nav_vid();
	void get_frame();
	
	void pass();
	void reject();
	
	void update();
	void draw();
	void run();
	
	int result = 0; 

	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours_green;
	vector<vector<Point>> contours_red;
	vector<Point> contour;
	
	Mat frame, im;

	int pass_count = 0;
	int rej_count = 0;

};
