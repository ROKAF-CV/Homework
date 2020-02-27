#include "Histogram.h"
#include "ImageOperation.h"
void hist() {

	Mat origin = imread("Lenna.jpg", 0);
	//Mat origin = imread("Lenna.jpg", CV_RGB2GRAY);
	Histogram myhist;
	Mat hist;
	imshow("origin", origin);
	double normalized_hist[256], equalized_hist[256];
	myhist.get_histogram(origin, normalized_hist);
	myhist.equalize_hist(origin, normalized_hist, equalized_hist);
	Mat equalized_img = myhist.equalize_hist(origin);
	/*const int* ch_num = { 0 };
	float ch_range[] = { 0.0,255.0 };
	const float* ch_ranges = ch_range;
	int num_bins = 255;
	calcHist(&origin, 1,ch_num, Mat(), hist, 1,&num_bins, &ch_ranges);*/


	Mat my_hist_img = myhist.draw_histogram(normalized_hist);
	Mat my_equal_hist = myhist.draw_histogram(equalized_hist);

	imshow("myres", my_hist_img);
	imshow("myres2", my_equal_hist);
	imshow("equalized img", equalized_img);
	waitKey();
}
void print(Mat &img) {
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 50; j++) {
			printf("%d ", img.at<uchar>(j, i));
		}
		cout << "\n";
	}
}
void otsu() {

	Mat origin = imread("Lenna.jpg", 0);
	Histogram myhist;
	Mat output = myhist.otsu(origin);
	imshow("before", origin);
	imshow("after otsu", output);
	waitKey();
}

void op() {
	ImageOperation op;
	Mat origin = imread("Lenna.jpg", 0);
	Mat output1(origin.size(), origin.type());
	Mat output2(origin.size(), origin.type());
	Mat output3(origin.size(), origin.type());
	Mat output4(origin.size(), origin.type());

	op.addOp(origin, output1, 50);
	op.subOp(origin, output2, 50);
	op.gammaOp(origin, output3, 0.5);
	op.invOp(origin, output4);
	imshow("before", origin);
	imshow("add", output1);
	imshow("sub", output2);
	imshow("gamma", output3);
	imshow("inv", output4);
	waitKey();
}


int main() {
	hist();
	otsu();
	op();
	return 0;
}