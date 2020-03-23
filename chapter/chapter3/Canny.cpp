#include "Canny.h"


Canny::Canny(const Mat &img) {
	n = img.cols;
	m = img.rows;
	visited = new bool*[m];
	for (int i = 0; i < m; i++) {
		visited[i] = new bool[n];
		memset(visited[i], 0, sizeof(bool)*n);
	}
}
Canny::~Canny() {
	for (int i = 0; i < m; i++) {
		delete[] visited[i];
	}
	delete[]visited;
}


void Canny::canny_edge(const Mat &img, Mat&out, double high, double low) {
	Mat after_gaussian;
	Mat dy(img.size(), CV_32F);
	Mat dx(img.size(), CV_32F);
	Mat edge_direct(img.size(), 0);
	Mat edge_mag(img.size(), CV_32F);
	//gaussian_blur2(img, out, 0.5);
	gaussian_blur(img, after_gaussian, 0.5);
	//GaussianBlur(img, after_gaussian, Size(3, 3), 0.5);
	sobelOp(after_gaussian, dy, 'y');
	sobelOp(after_gaussian, dx, 'x');

	edge_direction(gradient_direction(dy, dx), edge_direct);
	gradient_magnitude(dy, dx, edge_mag);

	NMSalgorithm(edge_mag, edge_direct);
	out = thresholding(edge_mag, high, low);

}





void Canny::NMSalgorithm(Mat &mag, const Mat &direct) {
	for (int j = 1; j < mag.rows - 2; j++) {
		for (int i = 1; i < mag.cols - 2; i++) {
			//neighbor 2 pixels
			int x1, y1, x2, y2;
			get_neighbor(direct, j, i, x1, y1, x2, y2);
			float magnitude = mag.at<float>(j, i);
			/*if (mag.at<uchar>(j, i) <= mag.at<uchar>(y1, x1) || mag.at<uchar>(j, i) <= mag.at<uchar>(y2, x2))
				mag.at<uchar>(j, i) = 0;*/
			if (magnitude <= mag.at<float>(y1, x1) || magnitude <= mag.at<float>(y2, x2))
				mag.at<float>(j, i) = 0;
		}
	}
}

void Canny::get_neighbor(const Mat &direction, int j, int i, int &x1, int &y1, int &x2, int &y2) {
	if (direction.at<uchar>(j, i) == 0 || direction.at<uchar>(j, i) == 4) {
		y1 = j - 1, x1 = i;
		y2 = j + 1, x2 = i;
	}
	if (direction.at<uchar>(j, i) == 1 || direction.at<uchar>(j, i) == 5) {
		y1 = j - 1, x1 = i + 1;
		y2 = j + 1, x2 = i - 1;
	}
	if (direction.at<uchar>(j, i) == 2 || direction.at<uchar>(j, i) == 6) {
		y1 = j, x1 = i - 1;
		y2 = j, x2 = i + 1;
	}
	if (direction.at<uchar>(j, i) == 3 || direction.at<uchar>(j, i) == 7) {
		y1 = j + 1, x1 = i + 1;
		y2 = j - 1, x2 = i - 1;
	}
}

Mat Canny::thresholding(const Mat &mag, const double T_high, const double T_low) {
	/*bool **visited = new bool*[m];
	for (int i = 0; i < m; i++) {
		visited[i] = new bool[n];
		memset(visited[i], 0, sizeof(bool)*n);
	}
*/
	Mat	out = Mat::zeros(mag.size(), 0);


	for (int j = 1; j < m - 2; j++) {
		for (int i = 1; i < n - 2; i++) {
			if (mag.at<float>(j, i) > T_high && !visited[j][i]) follow_edge(out, mag, j, i, T_low);
		}
	}/*
	for (int i = 0; i < m; i++) {
		delete[] visited[i];
	}
	delete []visited;*/
	return out;
}

void Canny::follow_edge(Mat &out, const Mat&mag, int y, int x, const double T_low) {
	visited[y][x] = true;

	out.at<uchar>(y, x) = 255;

	for (int i = 0; i < 8; i++) {
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (mag.at<float>(ny, nx) > T_low && !visited[ny][nx]) follow_edge(out, mag, ny, nx, T_low);
	}

}
bool Canny::isRange(int j, int i) {
	return j >= 0 && j < m && i >= 0 && i < n;
}
