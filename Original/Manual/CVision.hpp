#pragma once


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
	Rect bound;
	
	int h_g_u = 56;
	int s_g_u = 255;
	int v_g_u = 255;
	
	int h_g_l = 33;
	int s_g_l = 120;
	int v_g_l = 40;
	
	int h_r_u1 = 12;
	int s_r_u1 = 255;
	int v_r_u1 = 255;
	
	int h_r_l1 = 0;
	int s_r_l1 = 120;
	int v_r_l1 = 50;
	
	int h_r_u2 = 180;
	int s_r_u2 = 255;
	int v_r_u2 = 255;
	
	int h_r_l2 = 150;
	int s_r_l2 = 120;
	int v_r_l2 = 50;
	
	
	
	int obj_area = 140;
	
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours_green;
	vector<vector<Point>> contours_red;
	vector<Point> contour;
	
	Mat frame, im;

	int pass_count = 0;
	int rej_count = 0;

};
