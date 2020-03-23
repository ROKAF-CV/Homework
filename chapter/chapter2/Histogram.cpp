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






