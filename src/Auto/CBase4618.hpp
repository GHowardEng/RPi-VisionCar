#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//#define WIN4618 0
#define PI4618 1

#ifdef WIN4618
	#include "CControl.h"
	#include "stdafx.h"
#else 
	#include "CControlPi.hpp"
#endif

/**
*
* @brief Acts as a base class for further projects.
*
*
* This class is meant to be used as a base class for graphics related projects. Empty functions are to be overloaded into a derived class for further development.
*
*
* @author Grant Howard
*
*/
class CBase4618
{
public:
	#ifdef WIN4618
	CControl controller;	///< CControl object to interface with microcontroller
	#else 
	CControlPi controller;
	#endif
	
	Mat _canvas;		///< Mat image to be drawn on
	
	/** @brief Will read inputs and update locations once implemented.
	*
	* @return No return value
	*
	*/
	void update();

	/** @brief Will display updated image once implemented.
	*
	* @return No return value
	*
	*/
	void draw();

	/** @brief Runs program by calling update() and draw() in a loop.
	*
	* @return No return value
	*
	*/
	void run();
};
