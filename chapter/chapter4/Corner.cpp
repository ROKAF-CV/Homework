#include "Corner.h"

#define NUM_MAX 1e9

Corner::Corner()
{
}

queue<pair<int, int>> Corner::Moravec(const Mat &img, double thresh) {
	//for (int j = 3; j < img.rows-3; j++) {
	//	for (int i = 3; i < img.cols-3; i++) {
	//		int sum = 0;
	//		for (int l = 0; l < mask_size; l++) { //y����
	//			for (int k = 0; k < mask_size; k++) { //x����
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
			//S(0,1) S(1,0) S(-1,0) S(0,-1)�� ����ϴµ�, �� �� ���� ���� ��
			for (int ii = 0; ii < 4; ii++) {
				min_s = std::min(min_s, feature(img, dx[ii], dy[ii], i, j));
			}
			if (min_s > thresh) q.push({ i,j });
		}
	}

	return q;
}

Mat Corner::Harris(const Mat &img) {

	Mat dy(img.size(), CV_32F);
	Mat dx(img.size(), CV_32F);
	Mat dy_2, dx_2, dydx;
	Mat dst_dy_2, dst_dx_2, dst_dydx;
	Mat moment(img.size(),CV_32F); //���Ʈ ��� A
	float k = 0.04; //Ư¡���� ���ɼ�
	get_gradient(img, dy, dx,CV_32F);
	dy_2 = dy.mul(dy);
	dx_2 = dx.mul(dx);
	dydx = dy.mul(dx);

	GaussianBlur(dy_2, dst_dy_2, Size(3, 3), 1.0);
	GaussianBlur(dx_2, dst_dx_2, Size(3, 3), 1.0);
	GaussianBlur(dydx, dst_dydx, Size(3, 3), 1.0);

	//�� ��� ���� ���� ������ ���ϱ�
	for (int j = 0; j < moment.rows; j++) {
		for (int i = 0; i < moment.cols; i++) {
			float a11, a12, a21, a22;
			a11 = dst_dy_2.at<float>(j, i);
			a12 = a21= dst_dx_2.at<float>(j, i);
			a22 = dst_dydx.at<float>(j, i);
			
			//������ ���ؼ� Ư¡ ���ɼ� �� ã��
			/*float matrix[] = { a11,a12,a21,a22 };
			Mat A(Size(2, 2), CV_32F,matrix);
			Mat eigenval;
			eigen(A, eigenval);
			float eigenval1=eigenval.at<float>(0,0),eigenval2=eigenval.at<float>(0,1);
			float C=eigenval1*eigenval2-k*pow(eigenval1+eigenval2,2);
			*/

			//Ư¡ ���ɼ� ��
			float C = (a11*a22 - a12 * a21) - k * pow(a11 + a22, 2);
			moment.at<float>(j, i) = C;
		}
	}

	return moment;
}



//Moravec
//Ư¡ ���ɼ� �� ã��
//cx,cy �߽���ǥ 
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

queue<pair<int, int>> Corner::localization(const Mat &feature, double thresh) {
	//NMS ��ģ Ư¡�� ��ǥ (x,y)
	queue<pair<int, int>>q;

	for (int j = 0; j < feature.rows; j++) {
		for (int i = 0; i < feature.cols; i++) {
			float c = feature.at<float>(j, i);
			if (c > thresh && c > feature.at<float>(j + 1, i) && c > feature.at<float>(j, i + 1) && c > feature.at<float>(j - 1, i) && c > feature.at<float>(j, i - 1))
				q.push({ i,j });
		}
	}
	return q;
}
