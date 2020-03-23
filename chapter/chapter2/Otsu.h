#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "Histogram.h"
using namespace std;
using namespace cv;

class Otsu :public Histogram
{
public:
	Otsu();
	~Otsu();

	//in: image
	//out:binary image
	Mat otsu(Mat &img);

private:
	int otsu_objective_function(Mat &img);
	int argmax(double val[], int t);
};

