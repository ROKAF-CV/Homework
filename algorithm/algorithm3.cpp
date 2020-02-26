#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
using namespace std;
using namespace cv;
class Edge {
public:
	Edge(Mat img) {
		n = img.cols;
		m = img.rows;
		visited = new bool*[m];
		for (int i = 0; i < m; i++) {
			visited[i] = new bool[n];
			memset(visited[i], 0, sizeof(bool)*n);
		}
	}
	~Edge() {
		for (int i = 0; i < m; i++) {
			delete[] visited[i];
		}
		delete[]visited;
	}
	//기본 그레디언트 구하기
	void get_gradient(const Mat &img, Mat &dy, Mat &dx) {
		for (int j = 1; j < img.rows - 1; j++) {
			for (int i = 1; i < img.cols - 1; i++) {
				dy.at<uchar>(j, i) = img.at<uchar>(j + 1, i) - img.at<uchar>(j - 1, i);
				dx.at<uchar>(j, i) = img.at<uchar>(j, i + 1) - img.at<uchar>(j, i - 1);
			}
		}
	}
	void gradient_magnitude(const Mat &dy,const Mat &dx, Mat &magnitude) {
		/*for (int j = 1; j < dy.rows - 1; j++) {
			for (int i = 1; i < dy.cols - 1; i++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				magnitude.at<uchar>(j, i) = std::max(0.0, std::min(255.0, sqrt(pow(y, 2) + pow(x, 2))));
			}
		}*/
		for (int j = 1; j < dy.rows - 1; j++) {
			for (int i = 1; i < dy.cols - 1; i++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				magnitude.at<float>(j, i) = sqrt(pow(y, 2) + pow(x, 2));
			}
		}
	}

	Mat gradient_direction(const Mat &dy,const Mat &dx) {
		Mat direction(dy.size(), dy.type());
		for (int j = 1; j < dy.rows - 1; j++) {
			for (int i = 1; i < dy.cols - 1; i++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				direction.at<uchar>(j, i) = quantize_direction(cvFastArctan(y, x));
			}
		}
		return direction;
	}
	//에지방향 보고 섰을때 왼쪽이 밝고 오른쪽이 어두움
	void edge_direction(const Mat &direction,Mat &out) {
		for (int j = 1; j < direction.rows - 1; j++) {
			for (int i = 1; i < direction.cols - 1; i++) {
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
		/*for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << sobel[j][i] << " ";
			}
			cout << "\n";
		}*/
		
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
	void canny_edge(const Mat &img,Mat&out,double high,double low) {
		//3채널일 경우 1채널로 바꾸기
		if (img.channels() == 3) {
			
		}
		Mat dy(out.size(), out.type());
		Mat dx(out.size(), out.type());
		Mat edge_direct(out.size(), out.type());
		Mat edge_mag(out.size(), CV_32F);
		//gaussian_blur(img, out, 0.5);
		GaussianBlur(img, out, Size(3, 3), 0.5);
		sobelOp(out, dy, 'y');
		sobelOp(out, dx, 'x');
		
		//소벨 연산자를 통해 에지 방향과 크기 얻기
		edge_direction(gradient_direction(dy, dx) , edge_direct);
		gradient_magnitude(dy, dx, edge_mag);

		//NMS 알고리즘으로 거짓 에지 제거
		NMSalgorithm(edge_mag, edge_direct);
		thresholding(out, edge_mag, high, low);
	}

	void gaussian_blur(const Mat &img,Mat &out,double sigma) {
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
	void log_blur(Mat &img,Mat &out,float sigma) {
		Mat log_filter = LOG_filter(sigma);
		int mask_size = log_filter.rows/2;
		for (int i = mask_size; i <= out.cols - mask_size - 1; i++) {
			for (int j = mask_size; j < out.rows - mask_size - 1; j++) {
				float sum = 0.f;
				for (int k = -mask_size; k <= mask_size; k++) { //x방향
					for (int l = -mask_size; l <= mask_size; l++) { //y방향
						sum += log_filter.at<float>(l + mask_size, k + mask_size) * img.at<uchar>(j + l, i + k);
					}
				}
				sum = max(sum, -255.0f);
				sum = min(sum, 255.0f);
				out.at<float>(j, i) = sum;
			}
		}
	}
	void zerocrossing_detection(Mat &img,Mat &out ,float sigma,int thresh) {
		
		Mat log_img(img.size(),CV_32F);
		log_blur(img, log_img,sigma);
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
				cout << log_img.at<float>(j, i) << " ";
			}
			cout << "\n";
		}
		imshow("asdf", log_img);
		waitKey();
		for (int i = 1; i <= log_img.cols-1; i++) {
			for (int j = 1; j <= log_img.rows-1; j++) {
				int cnt = 0;
				if (log_img.at<float>(j - 1, i - 1)*log_img.at<float>(j + 1, i + 1) < 0) {
					if (abs(log_img.at<float>(j - 1, i - 1) - log_img.at<float>(j + 1, i + 1) > thresh)) {
						cnt++;
					}
				}
				if (log_img.at<float>(j - 1, i)*log_img.at<float>(j + 1, i) < 0) {
					if (abs(log_img.at<float>(j - 1, i) - log_img.at<float>(j + 1, i) > thresh)) {
						cnt++;
					}
				}
				if (log_img.at<float>(j - 1, i + 1)*log_img.at<float>(j + 1, i - 1) < 0) {
					if (abs(log_img.at<float>(j - 1, i + 1) - log_img.at<float>(j + 1, i - 1) > thresh)) {
						cnt++;
					}
				}
				if (log_img.at<float>(j, i + 1)*log_img.at<float>(j, i - 1) < 0) {
					if (abs(log_img.at<float>(j, i + 1) - log_img.at<float>(j, i - 1) > thresh)) {
						cnt++;
					}
				}

				if (cnt >= 2) out.at <uchar>(j,i)= 255;
				else out.at<uchar>(j, i) = 0;
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

	//캐니에지용 variable
	int n, m;
	bool **visited;

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

	Mat LOG_filter(float sigma) {
		Mat gaussian= gaussian_mask(sigma);
		Mat out(gaussian.rows, gaussian.cols, CV_32F);
		int row = gaussian.rows/2, col = gaussian.cols/2;
		for (int i = -col; i <= col; i++) {
			for (int j = -row; j <= row; j++) {
				out.at<float>(j + row, i + col) = (pow(i, 2) + pow(j, 2) - 2 * pow(sigma, 2)) / pow(sigma, 4)*gaussian.at<float>(j+row, i+col);
			}
		}
		return out;
	}
	void NMSalgorithm(Mat &mag, const Mat &direct) {
		for (int j = 1; j < mag.rows - 2; j++) {
			for (int i = 1; i < mag.cols - 2; i++) {
				//이웃화소 2개
				int x1, y1, x2, y2;
				get_neighbor(direct, j, i, x1, y1, x2, y2);
				/*if (mag.at<uchar>(j, i) <= mag.at<uchar>(y1, x1) || mag.at<uchar>(j, i) <= mag.at<uchar>(y2, x2))
					mag.at<uchar>(j, i) = 0;*/
				if (mag.at<float>(j, i) <= mag.at<float>(y1, x1) || mag.at<float>(j, i) <= mag.at<float>(y2, x2))
					mag.at<float>(j, i) = 0;
			}
		}
	}

	void get_neighbor(const Mat &img,int j,int i,int &x1,int &y1,int &x2,int &y2) {
		if (img.at<uchar>(j, i) == 0 || img.at<uchar>(j, i) == 4) {
			y1 = j - 1, x1 = i;
			y2 = j + 1, x2 = i;
		}
		if (img.at<uchar>(j, i) == 1 || img.at<uchar>(j, i) == 5) {
			y1 = j - 1, x1 = i + 1;
			y2 = j + 1, x2 = i - 1;
		}
		if (img.at<uchar>(j, i) == 2 || img.at<uchar>(j, i) == 6) {
			y1 = j, x1 = i - 1;
			y2 = j, x2 = i + 1;
		}
		if (img.at<uchar>(j, i) == 3 || img.at<uchar>(j, i) == 7) {
			y1 = j + 1, x1 = i - 1;
			y2 = j - 1, x2 = i + 1;
		}
	}

	void thresholding(Mat &out,const Mat &mag,const double T_high,const double T_low) {
		/*bool **visited = new bool*[m];
		for (int i = 0; i < m; i++) {
			visited[i] = new bool[n];
			memset(visited[i], 0, sizeof(bool)*n);
		}
*/	
		for (int j = 0; j < m; j++) {
			for (int i = 0; i < n; i++) {

				out.at<uchar>(j, i) = 0;
			}
		}


		for (int j = 1; j < m - 2; j++) {
			for (int i = 1; i < n - 2; i++) {
				if (mag.at<float>(j, i) > T_high && !visited[j][i]) follow_edge(out, mag, j, i, T_low);
			}
		}/*
		for (int i = 0; i < m; i++) {
			delete[] visited[i];
		}
		delete []visited;*/
	}
	
	void follow_edge(Mat &out,const Mat&mag, int y, int x, const double T_low) {
		visited[y][x] = true;
		out.at<uchar>(y, x) = 255;
		int dx[8] = {0,0,1,-1,1,-1,1,-1};
		int dy[8] = {1,-1,0,0,-1,-1,1,1};
		for (int i = 0; i < 8; i++) {
			int nx = x + dx[i];
			int ny = y + dy[i];
			if (mag.at<float>(ny, nx) > T_low && !visited[ny][nx]) follow_edge(out, mag, ny, nx, T_low);
		}

	}
	bool isRange(int j, int i) {
		return j >= 0 && j < m && i >= 0 && i < n;
	}
};

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