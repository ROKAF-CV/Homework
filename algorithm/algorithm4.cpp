#include "Corner.h"
#include "Histogram.h"
void CallBackFunc(int event, int x, int y, int flags, void* param)
{
	cv::Mat *im = reinterpret_cast<cv::Mat*>(param);
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "왼쪽 마우스 버튼 클릭.. 좌표 = (" << x << ", " << y << ") :" << static_cast<int>(im->at<char>(cv::Point(x, y))) << "\n";
	}
}

void draw_corner(Mat &img, queue<pair<int, int>> &q) {

}

Mat asdf(Mat &img) {
	Histogram hist;
	return hist.otsu(img);
}
int main() {
	//moravec();
	Corner corner;

	//Mat origin = Mat::ones(Size(5,5), CV_8U);
	//for (int i = 0; i < 5; i++) {
	//	origin.at<uchar>(i, 0) = 0;
	//	origin.at<uchar>(i, 1) = 0;
	//}
	////origin.at<uchar>(1, 1) = 1;
	//origin.at<uchar>(0, 3) = origin.at<uchar>(0, 4) = origin.at<uchar>(1, 4) = 0;
	//Mat out2(origin.size(), origin.type());
	Mat origin = imread("Lenna.jpg", 0);
	queue<pair<int,int>> q=corner.Moravec(origin, 2);
	cout << "끝남\n";
	while (!q.empty()) {
		int y = q.front().first;
		int x = q.front().second;
		q.pop();
		circle(origin, Point(y, x), 20, Scalar(0), 5);
	}
	imshow("asdfadsf", origin);
	waitKey();
	return 0;
}

template<typename _T>
void print(Mat &img) {
	for (int i = 0; i < 5; i++) {
		for(int j=0;j<5;j++){
			cout << img.at<_T>(j, i) << " ";
		}
		cout << "\n";
	}
}

