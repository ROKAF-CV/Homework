#include "Edge.h"

Edge::Edge()
{
}

Edge::Edge(Mat img) {
	n = img.cols;
	m = img.rows;
	visited = new bool*[m];
	for (int i = 0; i < m; i++) {
		visited[i] = new bool[n];
		memset(visited[i], 0, sizeof(bool)*n);
	}
}
Edge::~Edge() {
	for (int i = 0; i < m; i++) {
		delete[] visited[i];
	}
	delete[]visited;
}

void Edge::get_gradient(const Mat &img, Mat &dy, Mat &dx, int type) {
	if (type == CV_8U) {
		for (int j = 1; j < img.rows - 1; j++) {
			for (int i = 1; i < img.cols - 1; i++) {
				dy.at<uchar>(j, i) = img.at<uchar>(j + 1, i) - img.at<uchar>(j - 1, i);
				dx.at<uchar>(j, i) = img.at<uchar>(j, i + 1) - img.at<uchar>(j, i - 1);
			}
		}
	}
	else
		for (int j = 1; j < img.rows - 1; j++) {
			for (int i = 1; i < img.cols - 1; i++) {
				dy.at<float>(j, i) = img.at<uchar>(j + 1, i) - img.at<uchar>(j - 1, i);
				dx.at<float>(j, i) = img.at<uchar>(j, i + 1) - img.at<uchar>(j, i - 1);
			}
		}
}
void Edge::gradient_magnitude(const Mat &dy, const Mat &dx, Mat &magnitude) {
	/*for (int j = 1; j < dy.rows - 1; j++) {
		for (int i = 1; i < dy.cols - 1; i++) {
			int y = dy.at<uchar>(j, i);
			int x = dx.at<uchar>(j, i);
			magnitude.at<uchar>(j, i) = std::max(0.0, std::min(255.0, sqrt(pow(y, 2) + pow(x, 2))));
		}
	}*/
	int type = dy.type();
	if (type == CV_8U)
		for (int j = 1; j < dy.rows - 1; j++) {
			for (int i = 1; i < dy.cols - 1; i++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				magnitude.at<float>(j, i) = sqrt(pow(y, 2) + pow(x, 2));
			}
		}
	else if (type == CV_16S)
		for (int j = 1; j < dy.rows - 1; j++) {
			for (int i = 1; i < dy.cols - 1; i++) {
				int y = dy.at<short>(j, i);
				int x = dx.at<short>(j, i);
				magnitude.at<float>(j, i) = sqrt(pow(y, 2) + pow(x, 2));
			}
		}
	else if (type == CV_32F)
		for (int j = 1; j < dy.rows - 1; j++) {
			for (int i = 1; i < dy.cols - 1; i++) {
				int y = dy.at<float>(j, i);
				int x = dx.at<float>(j, i);
				magnitude.at<float>(j, i) = sqrt(pow(y, 2) + pow(x, 2));
			}
		}
}

Mat Edge::gradient_direction(const Mat &dy, const Mat &dx) {
	Mat direction(dy.size(), 0);
	int type = dy.type();
	int height = dy.rows - 1;
	int width = dy.cols - 1;
	if (type == CV_8U)
		for (int j = 1; j < height; j++) {
			for (int i = 1; i < width; i++) {
				int y = dy.at<uchar>(j, i);
				int x = dx.at<uchar>(j, i);
				direction.at<uchar>(j, i) = quantize_direction(cvFastArctan(y, x));
			}
		}
	else if (type == CV_16S) {
		for (int j = 1; j < height; j++) {
			for (int i = 1; i < width; i++) {
				int y = dy.at<short>(j, i);
				int x = dx.at<short>(j, i);
				direction.at<uchar>(j, i) = quantize_direction(cvFastArctan(y, x));
			}
		}
	}
	else if (type == CV_32F) {
		for (int j = 1; j < height; j++) {
			for (int i = 1; i < width; i++) {
				int y = dy.at<float>(j, i);
				int x = dx.at<float>(j, i);
				direction.at<uchar>(j, i) = quantize_direction(cvFastArctan(y, x));
			}
		}
	}
	return direction;
}

void Edge::edge_direction(const Mat &direction, Mat &out) {
	for (int j = 1; j < direction.rows - 1; j++) {
		for (int i = 1; i < direction.cols - 1; i++) {
			out.at<uchar>(j, i) = (direction.at<uchar>(j, i) + 2) % 8;
		}
	}
}
//type: y-> y방향, x -> x방향
//no padding
void Edge::sobelOp(Mat &img, Mat &out, char type) {
	int sobel[3][3];
	if (type == 'y') memcpy(sobel, sobel_y, sizeof(sobel_y));
	else memcpy(sobel, sobel_x, sizeof(sobel_x));

	cout << out.type();
	cout << out.rows << " " << out.cols;
	for (int i = 1; i < img.cols - 1; i++) {
		for (int j = 1; j < img.rows - 1; j++) {
			int sum = 0;
			for (int k = -1; k <= 1; k++) { //x방향
				for (int l = -1; l <= 1; l++) { //y방향
					sum += sobel[l + 1][k + 1] * img.at<float>(j + l, i + k);
				}
			}
			//sum = max(sum, 0);
			//sum = min(sum, 255);
			out.at<float>(j, i) = sum;
		}
	}
}
void Edge::canny_edge(const Mat &img, Mat&out, double high, double low) {
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

void Edge::gaussian_blur2(const Mat &img, Mat &out, double sigma) {
	Mat gaussian = gaussian_mask(sigma);
	int mask_size = gaussian.rows / 2;

	for (int i = 0; i < gaussian.cols; i++) {
		for (int j = 0; j < gaussian.rows; j++) {
			cout << gaussian.at<float>(j, i) << " ";
		}
		cout << "\n";
	}
	for (int i = mask_size; i <= out.cols - mask_size - 1; i++) {
		for (int j = mask_size; j < out.rows - mask_size - 1; j++) {
			float sum = 0.f;
			for (int k = -mask_size; k <= mask_size; k++) { //x����
				for (int l = -mask_size; l <= mask_size; l++) { //y����
					sum += gaussian.at<float>(l + mask_size, k + mask_size) * img.at<uchar>(j + l, i + k);
				}
			}
			sum = max(sum, 0.0f);
			sum = min(sum, 255.0f);
			out.at<uchar>(j, i) = sum;
		}
	}
}
void Edge::log_blur(Mat &img, Mat &out, float sigma) {
	Mat log_filter = LOG_filter(sigma);
	int mask_size = log_filter.rows / 2;
	for (int i = mask_size; i <= out.cols - mask_size - 1; i++) {
		for (int j = mask_size; j < out.rows - mask_size - 1; j++) {
			float sum = 0.f;
			for (int k = -mask_size; k <= mask_size; k++) { //x����
				for (int l = -mask_size; l <= mask_size; l++) { //y����
					sum += log_filter.at<float>(l + mask_size, k + mask_size) * img.at<uchar>(j + l, i + k);
				}
			}
			sum = max(sum, -255.0f);
			sum = min(sum, 255.0f);
			out.at<float>(j, i) = sum;
		}
	}
}
void Edge::zerocrossing_detection(Mat &img, Mat &out, float sigma, int thresh) {

	Mat log_img(img.size(), CV_32F);
	log_blur(img, log_img, sigma);
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 20; j++) {
			cout << log_img.at<float>(j, i) << " ";
		}
		cout << "\n";
	}
	imshow("asdf", log_img);
	waitKey();
	for (int i = 1; i <= log_img.cols - 1; i++) {
		for (int j = 1; j <= log_img.rows - 1; j++) {
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

			if (cnt >= 2) out.at <uchar>(j, i) = 255;
			else out.at<uchar>(j, i) = 0;
		}
	}
}


//8-quantization
uchar Edge::quantize_direction(float val) {
	uchar direction;
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
//padding ó�� �ʿ�
Mat Edge::gaussian_mask(float sigma) {
	int row, col;
	int sig = cvRound(6 * sigma);
	if (sig % 2 == 0) row = col = sig + 1;
	else row = col = sig;
	Mat out(row, col, CV_32F);
	row = row / 2, col = col / 2;

	for (int i = -col; i <= col; i++) {
		for (int j = -row; j <= row; j++) {
			out.at<float>(j + row, i + col) = (1 / (CV_2PI*pow(sigma, 2)))*exp(-(pow(i, 2) + (pow(j, 2))) / (2 * pow(sigma, 2)));
		}
	}
	return out;
}

Mat Edge::LOG_filter(float sigma) {
	Mat gaussian = gaussian_mask(sigma);
	Mat out(gaussian.rows, gaussian.cols, CV_32F);
	int row = gaussian.rows / 2, col = gaussian.cols / 2;
	for (int i = -col; i <= col; i++) {
		for (int j = -row; j <= row; j++) {
			out.at<float>(j + row, i + col) = (pow(i, 2) + pow(j, 2) - 2 * pow(sigma, 2)) / pow(sigma, 4)*gaussian.at<float>(j + row, i + col);
		}
	}
	return out;
}
void Edge::NMSalgorithm(Mat &mag, const Mat &direct) {
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

void Edge::get_neighbor(const Mat &direction, int j, int i, int &x1, int &y1, int &x2, int &y2) {
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

Mat Edge::thresholding(const Mat &mag, const double T_high, const double T_low) {
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

void Edge::follow_edge(Mat &out, const Mat&mag, int y, int x, const double T_low) {
	visited[y][x] = true;

	out.at<uchar>(y, x) = 255;

	for (int i = 0; i < 8; i++) {
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (mag.at<float>(ny, nx) > T_low && !visited[ny][nx]) follow_edge(out, mag, ny, nx, T_low);
	}

}
bool Edge::isRange(int j, int i) {
	return j >= 0 && j < m && i >= 0 && i < n;
}
void Edge::gaussian_blur(const Mat &img, Mat &out, float sigma) {
	register int i, j, k, x;

	int w = img.cols;
	int h = img.rows;

	out = Mat(img.size(), CV_32F);
	uchar *pData = img.data;
	//uchar *out_pData = out.data;
	//////////////////////////////////////////////////////////////////////////
	//가우시안 마스크
	//////////////////////////////////////////////////////////////////////////

	int dim = (int)max(3.0, 6 * sigma + 1.0); 
	if (dim % 2 == 0) dim++; 
	int dim2 = (int)dim / 2;

	float* pMask = new float[dim];
	for (i = 0; i < dim; i++)
	{
		x = i - dim2;
		pMask[i] = exp(-(x*x) / (2 * sigma*sigma)) / (sqrt(2 * CV_PI)*sigma);
	}

	//////////////////////////////////////////////////////////////////////////
	//임시버퍼
	//////////////////////////////////////////////////////////////////////////

	float** buf = new float*[h];
	for (i = 0; i < h; i++)
	{
		buf[i] = new float[w];
		memset(buf[i], 0, sizeof(float)*w);
	}

	//////////////////////////////////////////////////////////////////////////
	//세로
	//////////////////////////////////////////////////////////////////////////
	float sum1, sum2;
	for (i = 0; i < w; i++)
		for (j = 0; j < h; j++)
		{
			sum1 = sum2 = 0.0;

			for (k = 0; k < dim; k++)
			{
				x = k - dim2 + j;

				if (x >= 0 && x < h)
				{
					sum1 += pMask[k];
					sum2 += (pMask[k] * pData[x*w + i]);
				}
			}
			buf[j][i] = (sum2 / sum1);
		}

	//////////////////////////////////////////////////////////////////////////
	//가로
	//////////////////////////////////////////////////////////////////////////

	for (j = 0; j < h; j++)
		for (i = 0; i < w; i++)
		{
			sum1 = sum2 = 0.0;
			for (k = 0; k < dim; k++)
			{
				x = k - dim2 + i;
				if (x >= 0 && x < w)
				{
					sum1 += pMask[k];
					sum2 += (pMask[k] * buf[j][x]);
				}
			}
			out.at<float>(j, i) = limit(sum2 / sum1);
		}

	//////////////////////////////////////////////////////////////////////////
	//메모리해제
	//////////////////////////////////////////////////////////////////////////

	delete[] pMask;
	for (i = 0; i < h; i++)	delete[] buf[i];
	delete[] buf;
}

float Edge::limit(float a) {
	if (a > 255)return 255.f;
	else if (a < 0)return 0.f;
	else return (uchar)a;
}