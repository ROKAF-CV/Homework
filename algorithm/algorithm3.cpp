#include "Edge.h"

void edge() {
	
	Mat origin=imread("Lenna.jpg", 0);
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
	Mat out(origin.size(), origin.type());
	Mat out2;
	edge.sobelOp(origin, out, 'y');
	Sobel(origin, out2, CV_8U, 1, 0);
	imshow("out", out);
	imshow("out2", out2);
	waitKey();
}
void LOG() {
	
	Mat origin = imread("Lenna.jpg", 0);
	Edge edge(origin);
	Mat out(origin.size(), origin.type());
	edge.zerocrossing_detection(origin, out,0.7,1.0);
	imshow("out", out);
	waitKey();
}

void canny() {
	
	Mat origin = imread("example.jpg", 0);
	Edge edge(origin);
	Mat out=Mat::zeros(origin.size(), origin.type());
	Mat out2(origin.size(), origin.type());

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	edge.canny_edge(origin, out,60.0,20.0);
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;
	
	start = std::chrono::system_clock::now();
	Canny(origin, out2, 20, 60);
	sec = std::chrono::system_clock::now() - start;
	std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;
	
	imshow("origin", origin);
	imshow("out", out);
	imshow("out2", out2);
	waitKey();
}

int main() {
	canny();
	//sobel();
	return 0;
}