#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;

class ImageOperation {
public:
	//���� ȭ�� ���ϱ�
	void addOp(Mat &img, Mat &out, int intensity);

	//���� ȭ�� ����
	void subOp(Mat &img, Mat &out, int intensity);

	//���� ����
	void invOp(Mat &img, Mat &out);

	//���� ���� ��ȯ
	void gammaOp(Mat &img, Mat &out, double gamma);
};