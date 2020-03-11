#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;

class ImageOperation {
public:
	//영상에 화소 더하기
	void addOp(Mat &img, Mat &out, int intensity);

	//영상에 화소 빼기
	void subOp(Mat &img, Mat &out, int intensity);

	//영상 반전
	void invOp(Mat &img, Mat &out);

	//영상 감마 변환
	void gammaOp(Mat &img, Mat &out, double gamma);
};