#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <utility>
using namespace std;
using namespace cv;
#define NUM_MAX 1e9
class Corner{
public:
	queue<pair<int,int>> Moravec(const Mat &img, double thresh) {
		//for (int j = 3; j < img.rows-3; j++) {
		//	for (int i = 3; i < img.cols-3; i++) {
		//		int sum = 0;
		//		for (int l = 0; l < mask_size; l++) { //y방향
		//			for (int k = 0; k < mask_size; k++) { //x방향
		//				sum+=mask[l][k]*pow(img.at<uchar>(l + j, k + i) - img.at<uchar>(l, k), 2);
		//			}
		//		}
		//		out.at<uchar>(j, i) = max(0,min(sum,255));
		//	}
		//}
		queue<pair<int,int>>q;
		for (int j = 1; j < img.rows - 1; j++) {
			for (int i = 1; i < img.cols - 1; i++) {
				int min_s = NUM_MAX;
				//S(0,1) S(1,0) S(-1,0) S(0,-1)만 계산하는데, 그 중 제일 작은 값
				for (int ii = 0; ii < 4; ii++) {
					min_s = std::min(min_s, feature(img, dx[ii], dy[ii], i, j));
				}
				if (min_s > thresh) q.push({ i,j});
			}
		}

		return q;
	}

	void Harris(const Mat &img, Mat &out) {
		float mask[3][3] = {
			{0.0116601, 0.0861571, 0.0116601},
		{0.0861571, 0.63662, 0.0861571},
		{0.0116601, 0.0861571, 0.0116601}
		};
		
		int mask_size = 3;
		for (int j = 3; j < img.rows - 3; j++) {
			for (int i = 3; i < img.cols - 3; i++) {
				float sum = 0.f;
				for (int l = 0; l < mask_size; l++) { //y방향
					for (int k = 0; k < mask_size; k++) { //x방향
						sum += mask[l][k] * pow(img.at<uchar>(l + j, k + i) - img.at<uchar>(l, k), 2);
					}
				}
				out.at<uchar>(j, i) = max(0.f,min(sum,255.f));
			}
		}
	}

private:
	int mask[3][3] = {  {1,1,1},
						{1,1,1},
						{1,1,1} };
	int dx[4] = { 0,0,1,-1 }; //하, 상,좌,우
	int dy[4] = { 1,-1,0,0 };
	int nx[3] = { 0,1,-1 };
	int ny[3] = {0,1,-1 };

	//특징 가능성 값 찾기
	//cx,cy 중심좌표 
	//x,y S(x,y)
	int feature(const Mat &img,int x,int y,int cx,int cy) {
		int S=0;//sum of squared difference
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				int nx = i + cx;
				int ny = j + cy;
				if(isRange(ny+y,nx+x)&& isRange(ny,nx))
					S += pow(img.at<uchar>(ny+y,nx+x)-img.at<uchar>(ny,nx),2);
			}
		}
		return S;
	}
	
	bool isRange(int y, int x) {
		return y >= 0 && y < 5 && x >= 0 && x < 5;
	}
};

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
	
	Mat out(origin.size(),0);
	Mat out2=Mat::zeros(origin.size(), CV_32F);

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
	queue<pair<int,int>> q=corner.Moravec(origin, 3);
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
