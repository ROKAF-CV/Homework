#include "Edge.h"
void edge() {

	Mat origin = imread("Lenna.jpg", 0);
	Edge edge(origin);
	Mat out(origin.size(), origin.type());
	Mat out2(origin.size(), origin.type());
	edge.gaussian_blur(origin, out, 0.5);
	GaussianBlur(origin, out2, Size(7, 7), 1.0);
	imshow("out", out);
	imshow("out2", out2);
	waitKey();
}

void sobel() {

	Mat origin = imread("Lenna.jpg", 0);
	Edge edge(origin);
	Mat out(origin.size(), CV_32F);
	Mat out_x ,out_y;
	edge.sobelOp(origin, out, 'y');
	//edge.sobelOp(origin, out, 'x');
	Sobel(origin, out_x, CV_32F, 1,0);
	Sobel(origin, out_y, CV_32F, 0,1);
	imshow("out", out);
	imshow("x", out_x);
	imshow("y", out_y);
	waitKey();
}
void LOG() {

	Mat origin = imread("Lenna.jpg", 0);
	Edge edge(origin);
	Mat out(origin.size(), origin.type());
	edge.zerocrossing_detection(origin, out, 0.7, 1.0);
	imshow("out", out);
	waitKey();
}

void canny() {

	Mat origin = imread("example.jpg", 0);
	Mat origin2;
	for (int j = 50; j < 80; j++) {
		for (int i = 50; i < 80; i++) {
			cout << static_cast<int>(origin.at<uchar>(j, i)) << " ";
		}
		cout << "\n";
	}

	origin.convertTo(origin2, CV_32F);
	for (int j = 50; j < 80; j++) {
		for (int i = 50; i < 80; i++) {
			cout << (origin2.at<float>(j, i)) << " ";
		}
		cout << "\n";
	}
	imshow("origin11", origin);
	Edge edge(origin);
	Mat out = Mat::zeros(origin.size(), CV_32F);
	Mat out2(origin.size(), origin.type());
	
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	edge.canny_edge(origin2, out, 60.0, 20.0);
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	start = std::chrono::system_clock::now();
	//Canny(origin, out2, 20, 60);
	sec = std::chrono::system_clock::now() - start;
	std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	imshow("origin", origin);
	imshow("out", out);
	imshow("out2", out2);
	waitKey();
}
void canny2() {
	Mat origin = imread("example.jpg", 0);
	Edge edge(origin);
	Mat out = Mat::zeros(origin.size(), origin.type());
	Mat out2(origin.size(), origin.type());


	Canny(origin, out2, 20, 60);

	imshow("origin", origin);
	imshow("out", out);
	imshow("out2", out2);
	waitKey();
}
int main() {
	//sobel();
	canny();
	//canny2();
	return 0;
}