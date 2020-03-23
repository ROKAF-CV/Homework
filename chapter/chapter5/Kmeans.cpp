#include "Kmeans.h"
#include <random>


Kmeans::Kmeans()
{
	k = 3;
}
Kmeans::Kmeans(int k_,int iter_)
{
	k = k_;
	iter = iter_;
}

Kmeans::~Kmeans()
{
}
void Kmeans::get_Kmeans(const Mat &img, Mat &out) {
	
	CV_Assert(img.type() == CV_8UC3);
	out = Mat(img.size(), img.type());
	Mat img_copy;
	resize(img, img_copy, Size(100, 100));
	//int n = img_copy.rows*img_copy.cols;
	int n = img.rows*img.cols;
	vector<Vec3b>v;
	//��� ���� ����
	for (int j = 0; j < img.rows; j++) {
		for (int i = 0; i < img.cols; i++) {
			uchar b = img.at<Vec3b>(j, i)[0];
			uchar g = img.at<Vec3b>(j, i)[1];
			uchar r = img.at<Vec3b>(j, i)[2];
			v.push_back({ b,g,r });
		}
	}
	/*����� ���� ����
	for (int j = 0; j < img_copy.rows; j++) {
		for (int i = 0; i < img_copy.cols; i++) {
			uchar r = img_copy.at<uchar>[0];
			uchar g = img_copy.at<uchar>[1];
			uchar b = img_copy.at<uchar>[2];
			v.push_back({ r,g,b });
		}
	}*/

	//������ ���÷� ������ ��� �ʱ�ȭ
	vector<vector<Vec3b>>cluster(k);
	vector<Vec3b>cluster_center; //Ŭ�������� ���
	cluster_center = init_cluster(v, k, n);

	bool end_flag = false;
	int iterator = 0; //�ݺ�Ƚ��
	while (iterator<iter) {
		iterator++;
		//��� ���� ���� ���� ����� ���� ã��
		for (int i = 0; i < n; i++) {
			int min_dist = 1e5;
			int min_idx = -1;
			for (int j = 0; j < k; j++) {
				float dist = get_distance_3b(v[i], cluster_center[j]);
				if (dist < min_dist) {
					min_idx = j;
					min_dist = dist;
				}
			}
			cluster[min_idx].push_back(v[i]);
		}


		
		int same_center_cnt = 0;
		//���� ���� ������ ������� ��ü
		for (int i = 0; i < k; i++) {
			int size_ = cluster[i].size();
			Vec3i after_center = { 0,0,0 };
			Vec3b before_center;
			for (int j = 0; j < size_; j++) {
				after_center[0] += cluster[i][j][0];
				after_center[1] += cluster[i][j][1];
				after_center[2] += cluster[i][j][2];
			}
			before_center = cluster_center[i];
			
			after_center /= size_;
			
			//���� ���� �� ���� ��հ� ���ϱ�
			int cnt = 0;
			for (int j = 0; j < 3; j++) {
				if (abs(before_center[j] - after_center[j])<=2) cnt++;
			}
			
			if (cnt == 3) same_center_cnt++;
			cluster_center[i] = after_center; //���� ��� ������Ʈ
		}

		//��� ������ ������ ��ȭ�� ������ ����ȭ ����
		if (same_center_cnt == k) end_flag= true;
		else {
			for (int i = 0; i < k; i++) cluster[i].clear();
		}

		if (end_flag)break;

	}


	//��� ȭ�Ҹ� �� ���� ��ȣ�� �ٲٱ�
	int pixel[256][256][256] = {};

	
	for (int i = 0; i < k; i++) {
		int size_ = cluster[i].size();
		for (int j = 0; j < size_; j++) {
			uchar b = cluster[i][j][0];
			uchar g = cluster[i][j][1];
			uchar r = cluster[i][j][2];
			pixel[b][g][r] = i + 1;	
		}
	}
	//�� �ű��
	for (int j = 0; j < img.cols-1; j++) {
		for (int i = 0; i < img.rows-1; i++) {
			uchar img_b = img.at<Vec3b>(i, j)[0];
			uchar img_g = img.at<Vec3b>(i, j)[1];
			uchar img_r = img.at<Vec3b>(i, j)[2];
			switch (pixel[img_b][img_g][img_r]) {
			case 1: out.at<Vec3b>(i, j) = { 0,0,255 }; break;
			case 2: out.at<Vec3b>(i, j) = { 255,0,0 }; break;
			case 3: out.at<Vec3b>(i, j) = { 0,255,0 }; break;
			case 4: out.at<Vec3b>(i, j) = { 0,123,255 }; break;
			default: break;
			}
		}
	}
}

float Kmeans::get_distance_3b(Vec3b a, Vec3b b) {
	float sum = 0.f;
	for (int i = 0; i < 2; i++)
		sum += get_distance_1b(a[i], b[i]);
	sum = sqrt(sum);
	return sum;
}
//get euclidian distance
float Kmeans::get_distance_1b(uchar a, uchar b) {
	return pow(a - b,2);
}

vector<Vec3b> Kmeans::init_cluster(vector<Vec3b>v,int k,int n) {
	//������ ���� �߽� ����
	std::random_device rd;
	std::mt19937 mersenne(rd());
	std::uniform_int_distribution<> randomNum(0, n);

	vector<Vec3b> cluster;
	for (int i = 0; i < k; i++) {
		int idx = randomNum(mersenne);
		cluster.push_back(v[idx]);
	}
	return cluster;
}