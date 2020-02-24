#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;
class Histogram {
public:
	void get_histogram(Mat &img, double normalized_hist[]) {
		int histogram[256];
		memset(histogram, 0, sizeof(histogram));
		for (int i = 0; i < img.cols; i++) {
			for (int j = 0; j < img.rows; j++) {
				histogram[img.at<uchar>(j, i)]++;
			}
		}

		for (int i = 0; i < 256; i++) {
			normalized_hist[i] = (double)histogram[i] / (img.cols*img.rows);
		}

	}

	Mat draw_histogram(double normalized_hist[]) {
		double vmax = -1.0;
		Mat himg(256, 256, CV_8U, Scalar(255));

		for (int i = 0; i < 256; i++) vmax = std::max(vmax, normalized_hist[i]);

		for (int i = 0; i < 256; i++) {
			Point p1(i, 256), p2(i, 256 - (int)((normalized_hist[i] / vmax)*(0.8 * 256)));
			line(himg, p1, p2, Scalar(0));
		}
		return himg;
	}

	void equalize_hist(Mat &img,double normalized_hist[],double equalized_hist[]) {
		double cv_Lout[256] = { normalized_hist[0] };
		equalized_hist[0] = cv_Lout[0];
		for (int i = 0; i < 256; i++) {
			//cout << normalized_hist[i] << " ";

		}
		for (int i = 1; i < 256; i++) {
			cv_Lout[i] = normalized_hist[i]+cv_Lout[i-1];
			equalized_hist[i]=cvRound(cv_Lout[i] * 255);
		}

		for (int i = 0; i < 256; i++) {
			cout << equalized_hist[i] << " ";

		}
		return;
	}
	Mat equalize_hist(Mat &img) {
		int row = img.rows;
		int col = img.cols;
		Mat equalized_img(row,col,CV_8U);
		for (int i = 0; i < img.cols; i++) {
			for (int j = 0; j < img.rows; j++) {
				equalized_img.at<uchar>(j,i)=img.at<uchar>(j, i);
			}
		}
		return equalized_img;
	}

	Mat otsu(Mat &img) {
		Mat output(img.rows, img.cols, CV_8U);
		int thresh=otsu_objective_function(img);
		cout << thresh;
		int cnt = 0;
		for (int i = 0; i < img.cols; i++) {
			for (int j = 0; j < img.rows; j++) {
				if (img.at<uchar>(j, i) > thresh)
				{
					output.at<uchar>(j, i) = 255;
					cnt++;
				}
				
				else output.at<uchar>(j, i) = 0;
			}
		}
		cout << "\n"<<cnt;
		return output;
	}
	

private:
	int otsu_objective_function(Mat &img) {
		double w0[256], mu0[256], mu1[256], between[256] = {};
		double mu=0, v=0;
		double normalized_hist[256];

		get_histogram(img, normalized_hist);
		//histogram 평균 및 분산 구하기
		for (int i = 0; i < 256; i++) {
			mu += i * normalized_hist[i];
		}
		for (int i = 0; i < 256; i++) {
			v += pow(i - mu, 2)*normalized_hist[i];
		}

		//initialize
		w0[0] = normalized_hist[0];
		mu0[0] = 0;
		for (int i = 1; i < 256; i++) {
			w0[i] = w0[i - 1] + normalized_hist[i];
			mu0[i] = (w0[i - 1] * mu0[i - 1] + i * normalized_hist[i])/(w0[i]-0.0000000001); //0으로 나누어지는 것 방지
			mu1[i] = (mu - w0[i] * mu0[i]) / (1 - w0[i]);
			between[i] = w0[i] * (1 - w0[i])*(mu0[i] - mu1[i])*(mu0[i] - mu1[i]);
		}

		return argmax(between, 256);
	}

	int argmax(double val[], int t) {
		int max_index = 0;
		double max_val = 0;
		for (int i = 0; i < t; i++) {
			if (val[i] > max_val) {
				max_val = val[i];
				max_index = i;
			}
		}
		return max_index;
	}
};

class Operation {
public:
	void addOp(Mat &img, Mat &out, int intensity) {
		for (int i = 0; i < img.cols; i++) {
			for (int j = 0; j < img.rows; j++) {
				out.at<uchar>(j, i) = min(img.at<uchar>(j, i) + intensity, 255);
			}
		}
	}
	void subOp(Mat &img, Mat &out, int intensity) {
		for (int i = 0; i < img.cols; i++) {
			for (int j = 0; j < img.rows; j++) {
				out.at<uchar>(j, i) = max(img.at<uchar>(j, i) - intensity, 0);
			}
		}
	}
	void invOp(Mat &img, Mat &out) {
		for (int i = 0; i < img.cols; i++) {
			for (int j = 0; j < img.rows; j++) {
				out.at<uchar>(j, i) = abs(img.at<uchar>(j, i) - 255);
			}
		}
	}
	void gammaOp(Mat &img, Mat &out, double gamma) {
		for (int i = 0; i < img.cols; i++) {
			for (int j = 0; j < img.rows; j++) {
				out.at<uchar>(j, i) = 255 * pow(img.at<uchar>(j, i) / 255.0, gamma);
			}
		}
	}
	
private:

};
void hist() {

	Mat origin = imread("Lenna.jpg", 0);
	//Mat origin = imread("Lenna.jpg", CV_RGB2GRAY);
	Histogram myhist;
	Mat hist;
	imshow("origin", origin);
	double normalized_hist[256], equalized_hist[256];
	myhist.get_histogram(origin, normalized_hist);
	myhist.equalize_hist(origin, normalized_hist, equalized_hist);
	Mat equalized_img = myhist.equalize_hist(origin);
	/*const int* ch_num = { 0 };
	float ch_range[] = { 0.0,255.0 };
	const float* ch_ranges = ch_range;
	int num_bins = 255;
	calcHist(&origin, 1,ch_num, Mat(), hist, 1,&num_bins, &ch_ranges);*/


	Mat my_hist_img = myhist.draw_histogram(normalized_hist);
	Mat my_equal_hist = myhist.draw_histogram(equalized_hist);

	imshow("myres", my_hist_img);
	imshow("myres2", my_equal_hist);
	imshow("equalized img", equalized_img);
	waitKey();
}
void print(Mat &img) {
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 50; j++) {
			printf("%d ", img.at<uchar>(j, i));
		}
		cout << "\n";
	}
}
void otsu() {
	
	Mat origin = imread("Lenna.jpg", 0);
	Histogram myhist;
	Mat output=myhist.otsu(origin);
	imshow("before", origin);
	imshow("after otsu", output);
	waitKey();
}

void op() {
	Operation op;
	Mat origin = imread("Lenna.jpg", 0);
	Mat output1(origin.size(), origin.type());
	Mat output2(origin.size(), origin.type());
	Mat output3(origin.size(), origin.type());
	Mat output4(origin.size(), origin.type());

	op.addOp(origin, output1, 50);
	op.subOp(origin, output2, 50);
	op.gammaOp(origin, output3,0.5);
	op.invOp(origin, output4);
	imshow("before",origin);
	imshow("add", output1);
	imshow("sub", output2);
	imshow("gamma", output3);
	imshow("inv", output4);
	waitKey();
}
int main() {
	/*hist();
	otsu();
	op();*/
	return 0;
}