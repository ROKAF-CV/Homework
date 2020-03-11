#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;
class Histogram {
public:
	void get_histogram(Mat &img, double normalized_hist[]);

	Mat draw_histogram(double normalized_hist[]);
	void equalize_hist(Mat &img, double normalized_hist[], double equalized_hist[]);
	
	Mat equalize_hist(Mat &img);

	//in: image
	//out:binary image
	Mat otsu(Mat &img);


private:
	int otsu_objective_function(Mat &img);
	int argmax(double val[], int t);
};