#include "Corner.h"
void CallBackFunc(int event, int x, int y, int flags, void* param)
{
	cv::Mat *im = reinterpret_cast<cv::Mat*>(param);
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "왼쪽 마우스 버튼 클릭.. 좌표 = (" << x << ", " << y << ") :" << static_cast<int>(im->at<char>(cv::Point(x, y))) << "\n";
	}
}


void moravec() {

	Corner corner;
	Mat origin = imread("Lenna.jpg", 0);

	Mat out(origin.size(), 0);
	Mat out2 = Mat::zeros(origin.size(), CV_32F);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	//corner.Moravec(origin, out);
	corner.Harris(origin, out);
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	start = std::chrono::system_clock::now();
	//Canny(origin, out2, 20.0, 60.0);
	cornerHarris(origin, out2, 2, 3, 0.04);
	sec = std::chrono::system_clock::now() - start;
	std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;


	Mat dst_norm, dst_norm_scaled;
	normalize(out2, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);
	for (int i = 0; i < dst_norm.rows; i++)
	{
		for (int j = 0; j < dst_norm.cols; j++)
		{
			if ((int)dst_norm.at<float>(i, j) > 150)
			{
				circle(dst_norm_scaled, Point(j, i), 5, Scalar(0), 2, 8, 0);
			}
		}
	}

	imshow("corners_window", dst_norm_scaled);

	imshow("out", out);
	imshow("out2", out2);
	setMouseCallback("out", CallBackFunc, reinterpret_cast<void*>(&out));
	waitKey();
}
Mat src, src_gray;
int thresh = 200;
int max_thresh = 255;
void cornerHarris_demo()
{
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;
	Mat dst = Mat::zeros(src.size(), CV_32FC1);
	cornerHarris(src_gray, dst, blockSize, apertureSize, k);

	Mat dst_norm, dst_norm_scaled;
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);
	for (int i = 0; i < dst_norm.rows; i++)
	{
		for (int j = 0; j < dst_norm.cols; j++)
		{
			if ((int)dst_norm.at<float>(i, j) > thresh)
			{
				circle(dst_norm_scaled, Point(j, i), 5, Scalar(0), 2, 8, 0);
			}
		}
	}

	imshow("corners_window", dst_norm_scaled);
	waitKey();
}

int main() {
	//moravec();
	Corner corner;

	Mat origin = Mat::zeros(Size(7, 7), CV_8U);
	for (int i = 1; i <= 5; i++) {
		origin.at<uchar>(i, 3) = 1;
	}
	for (int i = 2; i <= 5; i++) {
		origin.at<uchar>(i, 4) = 1;
	}
	for (int i = 3; i <= 5; i++) {
		origin.at<uchar>(i, 5) = 1;
	}
	for (int i = 4; i <= 5; i++) {
		origin.at<uchar>(i, 6) = 1;
	}
	for (int i = 5; i <= 5; i++) {
		origin.at<uchar>(i, 3) = 1;
	}

	for (int j = 0; j < 7; j++) {
		for (int i = 0; i < 7; i++) {
			cout << (int)origin.at<uchar>(j, i) << " ";
		}
		cout << "\n";
	}
	Mat out2(origin.size(), origin.type());
	src = imread("example5.jpg");
	//cout << src.type();
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	queue<pair<int, int>> q = corner.Moravec(origin, 1);
	cout << q.size();
	while (!q.empty()) {
		int x = q.front().second;
		int y = q.front().first;

		circle(src_gray, Point(y, x), 5, Scalar(0));
	}
	cornerHarris_demo();
	imshow("out", src_gray);
	waitKey();
	//cornerHarris_demo();
	return 0;
}

template<typename _T>
void print(Mat &img) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			cout << img.at<_T>(j, i) << " ";
		}
		cout << "\n";
	}
}