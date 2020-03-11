#include "Kmeans.h"

void kmeans() {
	Kmeans seg(3,10);
	Mat origin = imread("example2.jpg",IMREAD_COLOR);
	Mat out;
	seg.get_Kmeans(origin,out);
	imshow("out", out);
	imshow("origin", origin);
	waitKey();
}

int main() {
	kmeans();
	return 0;
}