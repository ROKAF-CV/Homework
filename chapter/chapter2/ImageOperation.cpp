#include "ImageOperation.h"

void ImageOperation::addOp(Mat &img, Mat &out, int intensity) {
	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			out.at<uchar>(j, i) = min(img.at<uchar>(j, i) + intensity, 255);
		}
	}
}

void ImageOperation::subOp(Mat &img, Mat &out, int intensity) {
	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			out.at<uchar>(j, i) = max(img.at<uchar>(j, i) - intensity, 0);
		}
	}
}

void ImageOperation::invOp(Mat &img, Mat &out) {
	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			out.at<uchar>(j, i) = abs(img.at<uchar>(j, i) - 255);
		}
	}
}

void ImageOperation::gammaOp(Mat &img, Mat &out, double gamma) {
	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			out.at<uchar>(j, i) = 255 * pow(img.at<uchar>(j, i) / 255.0, gamma);
		}
	}
}
