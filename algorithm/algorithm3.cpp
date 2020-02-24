#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;
class Edge {
public:
	//기본 그레디언트 구하기
	void get_gradient(const Mat &img, Mat &dy, Mat &dx) {

		for (int i = 1; i < img.cols - 1; i++) {
			for (int j = 1; j < img.rows - 1; j++) {
				dy.at<uchar>(j, i) = img.at<uchar>(j + 1, i) - img.at<uchar>(j - 1, i);
				dx.at<uchar>(j, i) = img.at<uchar>(j, i + 1) - img.at<uchar>(j, i - 1);
			}
		}
	}

	void gradient_magnitude(const Mat &dy,const Mat &dx, Mat &magnitude) {
		for (int i = 1; i < dy.cols - 1; i++) {
			for (int j = 1; j < dy.rows - 1; j++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				magnitude.at<int>(j, i) = sqrt(pow(y, 2) + pow(x, 2));
			}
		}
	}

	Mat gradient_direction(const Mat &dy,const Mat &dx) {
		Mat direction(dy.size(), dy.type());
		for (int i = 1; i < dy.cols - 1; i++) {
			for (int j = 1; j < dy.rows - 1; j++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				direction.at<uchar>(j, i) = quantize_direction(cvFastArctan(y, x));
			}
		}
		return direction;
	}
	//에지방향 보고 섰을때 왼쪽이 밝고 오른쪽이 어두움
	void edge_direction(const Mat &direction,Mat &out) {
		for (int i = 1; i < direction.cols - 1; i++) {
			for (int j = 1; j < direction.rows - 1; j++) {
				out.at<uchar>(j, i) = (direction.at<uchar>(j, i) + 2) % 8;
			}
		}
	}
	//type: y-> y소벨, x -> x소벨
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
				for (int k = -1; k <= 1; k++) { //x방향
					for (int l = -1; l <= 1; l++) { //y방향
						sum += sobel[k + 1][l + 1] * img.at<uchar>(j + l, i + k);
					}
				}
				sum = max(sum, 0);
				sum = min(sum, 255);
				out.at<uchar>(j, i) = sum;
			}
		}
	}
	void canny_edge(Mat &img,Mat&out,double sigma,double high,double low) {
		//3채널일 경우 1채널로 바꾸기
		if (img.channels() == 3) {
			
		}
		Mat dy(out.size(), out.type());
		Mat dx(out.size(), out.type());
		Mat edge_direct(out.size(), out.type());
		Mat edge_mag(out.size(), out.type());
		gaussian_blur(img, out, sigma);
		sobelOp(out, dy, 'y');
		sobelOp(out, dx, 'x');
		
		//소벨 연산자를 통해 에지 방향과 크기 얻기
		edge_direction(gradient_direction(dy, dx) , edge_direct);
		gradient_magnitude(dy, dx, edge_mag);

		
	}

	void gaussian_blur(Mat &img,Mat &out,double sigma) {
		Mat gaussian = gaussian_mask(sigma);
		int mask_size = gaussian.rows/2;

		for (int i = 0; i < gaussian.cols; i++) {
			for (int j = 0; j < gaussian.rows; j++) {
				cout << gaussian.at<float>(j, i)<<" ";
			}
			cout << "\n";
		}
		for (int i = mask_size; i <= out.cols- mask_size-1; i++) {
			for (int j = mask_size; j < out.rows- mask_size-1; j++) {
				float sum = 0.f;
				for (int k = -mask_size; k <= mask_size; k++) { //x방향
					for (int l = -mask_size; l <= mask_size; l++) { //y방향
						sum+= gaussian.at<float>(l+mask_size,k+mask_size) * img.at<uchar>(j + l, i + k);
					}
				}
				sum = max(sum, 0.0f);
				sum = min(sum, 255.0f);
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
	//padding 처리 필요
	Mat gaussian_mask(float sigma) {
		int row, col;
		int sig = cvRound(6 * sigma);
		if (sig % 2 == 0) row = col = sig +1;
		else row = col = sig;
		Mat out(row, col, CV_32F);
		row = row / 2, col = col / 2;

		for (int i = -col; i <= col; i++) {
			for (int j = -row; j <= row; j++) {
				out.at<float>(j+row, i+col) = (1 / (CV_2PI*pow(sigma,2)))*exp(-(pow(i,2) + (pow(j,2))) / (2 * pow(sigma, 2)));
			}
		}
		return out;
	}

	Mat LOG_filter(double sigma) {
		Mat gaussian= gaussian_mask(sigma);
		Mat out(gaussian.rows, gaussian.cols, CV_32F);
		int row = gaussian.rows/2, col = gaussian.cols/2;
		for (int i = -col; i <= col; i++) {
			for (int j = -row; j <= row; j++) {
				out.at<float>(j + row, i + col) = (pow(i + col, 2) + pow(j + row, 2) - 2 * pow(sigma, 2)) / pow(sigma, 4)*gaussian.at<uchar>(j + row, i + col);
			}
		}
		return out;
	}

	void NMSalgorithm(Mat &mag,const Mat &direct) {

	}
};

void edge() {
	Edge edge;
	Mat origin=imread("Lenna.jpg", 0);
	Mat out(origin.size(), origin.type());
	Mat out2(origin.size(), origin.type());
	edge.gaussian_blur(origin, out, 1);
	GaussianBlur(origin, out2, Size(7, 7), 1.0);
	imshow("out", out);
	imshow("out2", out2);
	waitKey();
}

int main() {
	edge();
	return 0;
}