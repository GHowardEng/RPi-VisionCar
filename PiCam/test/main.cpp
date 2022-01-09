#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include <opencv2/opencv.hpp>

#include "raspicam_cv.h"

int main ( ) 
{
    raspicam::RaspiCam_Cv Camera;

    Camera.open();
	
    cv::Mat image;
    
    
	while (1)
	{
	  Camera.grab();
      Camera.retrieve ( image );
	  cv::imshow("asdf", image);
	  cv::waitKey(100);
	}
	
	Camera.release();

}
