#include "Histogram.h"

void Histogram::get_histogram(Mat &img, double normalized_hist[]) {
	int histogram[256];
	memset(histogram, 0, sizeof(histogram));
	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			histogram[img.at<uchar>(j, i)]++;
		}
	}

	for (int i = 0; i < 256; i++) {
		normalized_hist[i] = (double)histogram[i] / (img.cols*img.rows);
	}

}

Mat Histogram::draw_histogram(double normalized_hist[]) {
	double vmax = -1.0;
	Mat himg(256, 256, CV_8U, Scalar(255));

	for (int i = 0; i < 256; i++) vmax = std::max(vmax, normalized_hist[i]);

	for (int i = 0; i < 256; i++) {
		Point p1(i, 256), p2(i, 256 - (int)((normalized_hist[i] / vmax)*(0.8 * 256)));
		line(himg, p1, p2, Scalar(0));
	}
	return himg;
}

void Histogram::equalize_hist(Mat &img, double normalized_hist[], double equalized_hist[]) {
	double cv_Lout[256] = { normalized_hist[0] };
	equalized_hist[0] = cv_Lout[0];
	for (int i = 0; i < 256; i++) {
		//cout << normalized_hist[i] << " ";

	}
	for (int i = 1; i < 256; i++) {
		cv_Lout[i] = normalized_hist[i] + cv_Lout[i - 1];
		equalized_hist[i] = cvRound(cv_Lout[i] * 255);
	}

	for (int i = 0; i < 256; i++) {
		cout << equalized_hist[i] << " ";

	}
	return;
}
Mat Histogram::equalize_hist(Mat &img) {
	int row = img.rows;
	int col = img.cols;
	Mat equalized_img(row, col, CV_8U);
	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			equalized_img.at<uchar>(j, i) = img.at<uchar>(j, i);
		}
	}
	return equalized_img;
}

Mat Histogram::otsu(Mat &img) {
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



int Histogram::otsu_objective_function(Mat &img) {
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

int Histogram::argmax(double val[], int t) {
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


