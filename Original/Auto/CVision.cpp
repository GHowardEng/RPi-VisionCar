#include "CVision.hpp"

CVision::CVision()
{
	int dbug = 0;
	if(dbug)
	{
		namedWindow("HSV Green Adjust", 1);
		namedWindow("HSV Red Adjust (Orange Range)", 1);
		namedWindow("HSV Red Adjust (Violet Range)", 1);
		createTrackbar("Object Detection Area", "HSV Green Adjust", &obj_area, 1000);
		createTrackbar("H Green Upper","HSV Green Adjust", &h_g_u, 180);
		createTrackbar("S Green Upper","HSV Green Adjust", &s_g_u, 255);
		createTrackbar("V Green Upper","HSV Green Adjust", &v_g_u, 255);
		
		createTrackbar("H Green Lower","HSV Green Adjust", &h_g_l, 180);
		createTrackbar("S Green Lower","HSV Green Adjust", &s_g_l, 255);
		createTrackbar("V Green Lower","HSV Green Adjust", &v_g_l, 255);
		
		createTrackbar("H Red Upper","HSV Red Adjust (Orange Range)", &h_r_u1, 180);
		createTrackbar("S Red Upper","HSV Red Adjust (Orange Range)", &s_r_u1, 255);
		createTrackbar("V Red Upper","HSV Red Adjust (Orange Range)", &v_r_u1, 255);
		
		createTrackbar("H Red Lower","HSV Red Adjust (Orange Range)", &h_r_l1, 180);
		createTrackbar("S Red Lower","HSV Red Adjust (Orange Range)", &s_r_l1, 255);
		createTrackbar("V Red Lower","HSV Red Adjust (Orange Range)", &v_r_l1, 255);
		
		createTrackbar("H Red Upper","HSV Red Adjust (Violet Range)", &h_r_u2, 180);
		createTrackbar("S Red Upper","HSV Red Adjust (Violet Range)", &s_r_u2, 255);
		createTrackbar("V Red Upper","HSV Red Adjust (Violet Range)", &v_r_u2, 255);
		
		createTrackbar("H Red Lower","HSV Red Adjust (Violet Range)", &h_r_l2, 180);
		createTrackbar("S Red Lower","HSV Red Adjust (Violet Range)", &s_r_l2, 255);
		createTrackbar("V Red Lower","HSV Red Adjust (Violet Range)", &v_r_l2, 255);
	}
}

CVision::~CVision()
{
	
}

void CVision::get_frame()
{
	
}

bool CVision::in_frame()
{

	/*int area;

		cv::Mat gray, g_mask;
		contours_green.clear();
		contours_red.clear();
	

		if (frame.empty() == false)
		{
			cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
			cv::inRange(gray, 60, 255, g_mask);

			cv::erode(g_mask, g_mask, cv::Mat());
			cv::dilate(g_mask, g_mask, cv::Mat());

			findContours(g_mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
			
			for (int i = 0; i < contours.size(); i++)
			{
				area = contourArea(contours.at(i));
			
				if (area > obj_area)
				{
					//drawContours(frame, contours, i, Scalar (10, 10, 250), 4, 8, hierarchy);
					
					//cv::Rect bound (boundingRect(contours.at(i)));	
					//rectangle(frame, bound, Scalar (10, 10, 250), 1);
					
					//imshow("Object Detection", g_mask);
					
					return true;
				}
			}
		}*/
	

	return false;
}


int CVision::nav_vid()
{
						// H	S	V
	cv::Scalar lower_green(h_g_l, s_g_l, v_g_l);
	cv::Scalar upper_green(h_g_u, s_g_u, v_g_u);	// Green HSV thresholds
	
	cv::Scalar lower_red(h_r_l1, s_r_l1, v_r_l1);
	cv::Scalar upper_red(h_r_u1, s_r_u1, v_r_u1);		// Red HSV thresholds
	
	cv::Mat hsv, mask_green, mask_red, red_low, red_high, res_green, res_red;

	contours_green.clear();
	contours_red.clear();
	
	int area;

	if (frame.empty() == false)
	{	
		cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
		// Test for red
		// Threshold for lower (orange) values and wrap around to higher (purple) values
		cv::inRange(hsv, lower_red, upper_red, red_low);
		
		cv::inRange(hsv, cv::Scalar(h_r_l2, s_r_l2, v_r_l2), cv::Scalar(h_r_u2, s_r_u2, v_r_l1), red_high);
		
		// Combine two thresholded images
		cv::addWeighted(red_low, 1.0, red_high, 1.0, 0.0, mask_red);
		
		cv::erode(mask_red, mask_red, cv::Mat());
		cv::erode(mask_red, mask_red, cv::Mat());
		cv::dilate(mask_red, mask_red, cv::Mat());
		cv::dilate(mask_red, mask_red, cv::Mat());
		
		findContours(mask_red, contours_red, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	
		if (contours_red.size())
		{
			//cout << "\nRed Run";
			for (int i = 0; i < contours_red.size(); i++)
			{
				drawContours(mask_red, contours_red, i, Scalar::all(255), CV_FILLED, 8, hierarchy);
			}
			cv::bitwise_and(frame, frame, res_red, mask_red);
			
			for (int i = 0; i < contours_red.size(); i++)
			{
				area = contourArea(contours_red.at(i));
				
				if (area > obj_area)
				{
					//drawContours(frame, contours_red, i, Scalar::all(255), 2, 8, hierarchy);
					bound =(boundingRect(contours_red.at(i)));	
					rectangle(frame, bound, Scalar::all(255), 3);
						
					i = contours_red.size();
					im = res_red;
					return RED;
				}
				
			}
		}
		
		
		// Test for green in frame
		
		cv::inRange(hsv, lower_green, upper_green, mask_green);

		cv::erode(mask_green, mask_green, cv::Mat());
		cv::erode(mask_green, mask_green, cv::Mat());
		
		cv::dilate(mask_green, mask_green, cv::Mat());
		cv::dilate(mask_green, mask_green, cv::Mat());
	
		findContours(mask_green, contours_green, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		if (contours_green.size())
		{
			//cout << "\nGreen Run";
			for (int i = 0; i < contours_green.size(); i++)
			{
				drawContours(mask_green, contours_green, i, Scalar::all(255), CV_FILLED, 8, hierarchy);
			}
			cv::bitwise_and(frame, frame, res_green, mask_green);
		
			for (int i = 0; i < contours_green.size(); i++)
			{
				area = contourArea(contours_green.at(i));
				
				if (area > obj_area)
				{
					//drawContours(frame, contours_green, i, Scalar::all(255), 2, 8, hierarchy);
					bound = (boundingRect(contours_green.at(i)));	
					rectangle(frame, bound, Scalar::all(255), 2);
						
					i = contours_green.size();
					im = res_green;
					return GREEN;
				}
				
			}
		}
		
		
		
	}
	
	im = frame;
	// Return 0 if no colour found
	return 0;
}
int status = 0;



void CVision::update()
{
	//get_frame();
	
	if (result == 's')
	{
		if(status)
		{
			status = 0;
		}
		
		else
		{
			status = 1;
		}
	}
	
	if (status)
	{
		
		
		if (in_frame())
		{
			
			if (nav_vid() || (status == 0 && result == 'p'))
			{
				pass();
			}
			
			else 
			{
				reject();
			}
		}
	}
	
	else if (result == 'p')
		pass();
		
	else if (result == 'r')
		reject();
}

void CVision::draw()
{

	putText(frame, to_string(pass_count), Point(140,40), FONT_HERSHEY_DUPLEX, 1, Scalar(20, 255, 10), 2);
	putText(frame, "/", Point(180,40), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2);
	putText(frame, to_string(rej_count), Point(220,40), FONT_HERSHEY_DUPLEX, 1, Scalar(20, 10, 255), 2);
	
	if (status)
	{
		putText(frame, "Status: ON", Point(20, 80), FONT_HERSHEY_DUPLEX, 0.7, Scalar (20, 255, 10), 1);
	}
	
	else
	{
		putText(frame, "Status: OFF", Point(20, 80), FONT_HERSHEY_DUPLEX, 0.7, Scalar (20, 10, 255), 1);
	}
	
	
	//cv::imshow("Frame", frame);
}

void CVision::run()
{
	pass_count = 0;
	rej_count = 0;
	
	do
	{
		update();
		draw();
		
		result = waitKey(10);
		
	} while (result != ' ');
}

void CVision::pass()
{	
	putText(frame, "Pass", Point(20,40), FONT_HERSHEY_DUPLEX, 1, Scalar(20, 255, 10), 2);
	pass_count++;	
}

void CVision::reject()
{	
	putText(frame, "Reject", Point(20,40), FONT_HERSHEY_DUPLEX, 1, Scalar(20, 10, 255), 2);
	rej_count += 1;
}
