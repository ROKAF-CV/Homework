#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
using namespace std;
using namespace cv;
class Corner {
public:
	void Moravec(const Mat &img, Mat &out) {
		int mask_size = 3;
		for (int j = 3; j < img.rows-3; j++) {
			for (int i = 3; i < img.cols-3; i++) {
				int sum = 0;
				for (int l = 0; l < mask_size; l++) { //y방향
					for (int k = 0; k < mask_size; k++) { //x방향
						sum+=mask[l][k]*pow(img.at<uchar>(l + j, k + i) - img.at<uchar>(l, k), 2);
					}
				}
				out.at<uchar>(j, i) = sum;
			}
		}
	}

	void Harris(const Mat &img, Mat &out) {

	}

private:
	int mask[3][3] = {  {1,1,1},
						{1,1,1},
						{1,1,1} };
};

void moravec() {
	Corner corner;
	Mat origin = imread("example.jpg", 0);
	
	Mat out(origin.size(), origin.type());
	Mat out2(origin.size(), origin.type());

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	corner.Moravec(origin, out);
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	start = std::chrono::system_clock::now();
	//Canny(origin, out2, 20.0, 60.0);
	sec = std::chrono::system_clock::now() - start;
	std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;


	imshow("out", out);
	imshow("out2", out2);
	waitKey();
}
int main() {
	moravec();
	return 0;
}