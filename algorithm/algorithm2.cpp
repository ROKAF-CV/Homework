#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;
class Histogram {
public:
	void get_histogram(Mat &img) {
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
		return;
	}

	Mat draw_histogram() {
		double vmax = -1.0;
		Mat himg(256, 256, CV_8U, Scalar(255));

		for (int i = 0; i < 256; i++) vmax = std::max(vmax, normalized_hist[i]);

		for (int i = 0; i < 256; i++) {
			Point p1(i, 256), p2(i, 256 - (int)((normalized_hist[i] / vmax)*(0.8 * 256)));
			line(himg, p1, p2, Scalar(0));
		}
		return himg;
	}

private:
	double normalized_hist[256];
};


int main() {
	
	Mat origin = imread("Lenna.jpg", 0);
	//Mat origin = imread("Lenna.jpg", CV_RGB2GRAY);
	Histogram myhist;
	Mat hist;
	imshow("origin", origin);
	myhist.get_histogram(origin);

	const int* ch_num = { 0 };
	float ch_range[] = { 0.0,255.0 };
	const float* ch_ranges = ch_range;
	int num_bins = 255;
	calcHist(&origin, 1,ch_num, Mat(), hist, 1,&num_bins, &ch_ranges);


	Mat my_output_img=myhist.draw_histogram();

	imshow("myres", my_output_img);

	waitKey();
	return 0;
}