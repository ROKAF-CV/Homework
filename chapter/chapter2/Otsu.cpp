#include "Otsu.h"



Otsu::Otsu()
{
}


Otsu::~Otsu()
{
}

int Otsu::otsu_objective_function(Mat &img) {
	double w0[256], mu0[256], mu1[256], between[256] = {};
	double mu = 0, v = 0;
	double normalized_hist[256];

	get_histogram(img, normalized_hist);
	//histogram 평균 및 분산 구하기
	for (int i = 0; i < 256; i++) {
		mu += i * normalized_hist[i];
	}
	for (int i = 0; i < 256; i++) {
		v += pow(i - mu, 2)*normalized_hist[i];
	}

	//initialize
	w0[0] = normalized_hist[0];
	mu0[0] = 0;
	for (int i = 1; i < 256; i++) {
		w0[i] = w0[i - 1] + normalized_hist[i];
		mu0[i] = (w0[i - 1] * mu0[i - 1] + i * normalized_hist[i]) / (w0[i] - 0.0000000001); //0으로 나누어지는 것 방지
		mu1[i] = (mu - w0[i] * mu0[i]) / (1 - w0[i]);
		between[i] = w0[i] * (1 - w0[i])*(mu0[i] - mu1[i])*(mu0[i] - mu1[i]);
	}

	return argmax(between, 256);
}

int Otsu::argmax(double val[], int t) {
	int max_index = 0;
	double max_val = 0;
	for (int i = 0; i < t; i++) {
		if (val[i] > max_val) {
			max_val = val[i];
			max_index = i;
		}
	}
	return max_index;
}


Mat Otsu::otsu(Mat &img) {
	Mat output(img.rows, img.cols, CV_8U);
	int thresh = otsu_objective_function(img);
	cout << thresh;
	int cnt = 0;
	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			if (img.at<uchar>(j, i) > thresh)
			{
				output.at<uchar>(j, i) = 255;
				cnt++;
			}

			else output.at<uchar>(j, i) = 0;
		}
	}
	cout << "\n" << cnt;
	return output;
}
