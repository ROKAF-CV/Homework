#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;
class Edge {
public:
	void get_gradient(Mat &img, Mat &dy, Mat &dx) {

		for (int i = 1; i < img.cols - 1; i++) {
			for (int j = 1; j < img.rows - 1; j++) {
				dy.at<uchar>(j, i) = img.at<uchar>(j + 1, i) - img.at<uchar>(j - 1, i);
				dx.at<uchar>(j, i) = img.at<uchar>(j, i + 1) - img.at<uchar>(j, i - 1);
			}
		}
	}

	void gradient_magnitude(Mat &dy, Mat &dx, Mat &magnitude) {
		for (int i = 1; i < dy.cols - 1; i++) {
			for (int j = 1; j < dy.rows - 1; j++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				magnitude.at<int>(j, i) = sqrt(pow(y, 2) + pow(x, 2));
			}
		}
	}

	void gradient_direction(Mat &dy, Mat &dx, Mat &direction) {
		for (int i = 1; i < dy.cols - 1; i++) {
			for (int j = 1; j < dy.rows - 1; j++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				direction.at<uchar>(j, i) = quantize_direction(cvFastArctan(y, x));
			}
		}
	}
	//tpye: y-> y소벨, x -> x소벨
	//no padding
	void sobelOp(Mat &img, Mat &out, char type) {
		int sobel[3][3];
		if (type == 'y') memcpy(sobel, sobel_y, sizeof(sobel_y));
		else memcpy(sobel, sobel_x, sizeof(sobel_x));
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << sobel[j][i] << " ";
			}
			cout << "\n";
		}

		for (int i = 1; i < img.cols - 1; i++) {
			for (int j = 1; j < img.rows - 1; j++) {
				int sum = 0;
				for (int k = -1; k < 2; k++) { //x방향
					for (int l = -1; l < 2; l++) { //y방향
						sum += sobel[k + 1][l + 1] * img.at<uchar>(j + l, i + k);
					}
				}
				sum = max(sum, 0);
				sum = min(sum, 255);
				out.at<uchar>(j, i) = sum;
			}
		}
	}
private:
	int sobel_y[3][3] = {	{-1,-2,-1},
							{0,0,0},
							{1,2,1} };
	int sobel_x[3][3] = {	{-1,0,1},
							{-2,0,2},
							{-1,0,1} };
	int laplacian_mask[3][3] = { {0,1,0},
							{1,-4,1},
							{0,1,0} };

	//8-quantization && 3시방향이 0도
	int quantize_direction(int val) {
		int direction;
		if (val > 337.5 || val <= 22.5)			direction = 0;
		else if (val > 22.5 && val <= 67.5)		direction = 1;
		else if (val > 67.5 && val <= 112.5)	direction = 2;
		else if (val > 112.5 && val <= 157.5)	direction = 3;
		else if (val > 157.5 && val <= 202.5)	direction = 4;
		else if (val > 202.5 && val <= 247.5)	direction = 5;
		else if (val > 247.5 && val <= 292.5)	direction = 6;
		else direction = 7;
		return direction;
	}
};

int main() {
	cout << cvFastArctan(1, 1);
	return 0;
}