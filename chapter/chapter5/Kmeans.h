#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
using namespace std;
using namespace cv;
class Kmeans
{
public:
	Kmeans();
	Kmeans(int k_, int iter_);
	~Kmeans();
	//3컬러 이미지 ,기본 3개로 나누기
	void get_Kmeans(const Mat &img, Mat &out);
private:
	int k, iter;
	float get_distance_3b(Vec3b a, Vec3b b);
	float get_distance_1b(uchar a, uchar b);
	vector<Vec3b>init_cluster(vector<Vec3b>v, int k,int n);
};

