#include "Corner.h"

#define NUM_MAX 1e9

queue<pair<int, int>> Corner::Moravec(const Mat &img, double thresh) {
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
	queue<pair<int, int>>q;
	for (int j = 1; j < img.rows - 1; j++) {
		for (int i = 1; i < img.cols - 1; i++) {
			int min_s = NUM_MAX;
			//S(0,1) S(1,0) S(-1,0) S(0,-1)만 계산하는데, 그 중 제일 작은 값
			for (int ii = 0; ii < 4; ii++) {
				min_s = std::min(min_s, feature(img, dx[ii], dy[ii], i, j));
			}
			if (min_s > thresh) q.push({ i,j });
		}
	}

	return q;
}

void Corner::Harris(const Mat &img, Mat &out) {
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
			out.at<uchar>(j, i) = max(0.f, min(sum, 255.f));
		}
	}
}



//특징 가능성 값 찾기
//cx,cy 중심좌표 
//x,y S(x,y)
int Corner::feature(const Mat &img, int x, int y, int cx, int cy) {
	int S = 0;//sum of squared difference
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			int nx = i + cx;
			int ny = j + cy;
			if (isRange(ny + y, nx + x) && isRange(ny, nx))
				S += pow(img.at<uchar>(ny + y, nx + x) - img.at<uchar>(ny, nx), 2);
		}
	}
	return S;
}

bool Corner::isRange(int y, int x) {
	return y >= 0 && y < 5 && x >= 0 && x < 5;
}
