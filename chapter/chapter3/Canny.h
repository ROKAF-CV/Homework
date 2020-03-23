#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Edge.h"
using namespace cv;

class Canny:public Edge
{
public:
	Canny(const Mat &img);
	Canny();
	~Canny();
	void canny_edge(const Mat &img, Mat&out, double high, double low);
private:

	int n, m;
	bool **visited;
	int dy[8] = { 0,0,1,-1,-1,1,-1,1 };
	int dx[8] = { 1,-1,0,0,-1,-1 };

	void NMSalgorithm(Mat &mag, const Mat &direct);
	void get_neighbor(const Mat &img, int j, int i, int &x1, int &y1, int &x2, int &y2);
	Mat thresholding(const Mat &mag, const double T_high, const double T_low);
	void follow_edge(Mat &out, const Mat&mag, int y, int x, const double T_low);
	bool isRange(int j, int i);

};

