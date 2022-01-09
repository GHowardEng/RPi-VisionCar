#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
	cv::VideoCapture vid;
	vid.open(0);
	
	while(1){
		if (vid.isOpened() == true)
		{
			vid >> frame;
		}
		imshow("Frame",frame);
		waitkey(1);
	}
}