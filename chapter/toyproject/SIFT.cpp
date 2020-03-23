#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/flann/flann.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iostream>
#define RED Scalar(0,0,255)

using namespace cv;
void detect(const Mat &img1, const Mat &img2) {

}
int main() {
	Mat img1 = imread("model.jpg", 0);
	Mat img2 = imread("scene.jpg", 0);


	Ptr<xfeatures2d::SurfFeatureDetector> detector
		= xfeatures2d::SurfFeatureDetector::create();
	std::vector<KeyPoint> keypoint1, keypoint2;

	detector->detect(img1, keypoint1);
	detector->detect(img2, keypoint2);

	Mat disp;
	drawKeypoints(img1, keypoint1, disp, RED, DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("keypoint", disp);
	//waitKey();

	Ptr<xfeatures2d::SurfDescriptorExtractor>extractor
		= xfeatures2d::SurfDescriptorExtractor::create();

	Mat descriptor1, descriptor2;
	extractor->compute(img1, keypoint1, descriptor1);
	extractor->compute(img2, keypoint2, descriptor2);

	//매칭
	FlannBasedMatcher matcher;
	std::vector<DMatch> match;
	matcher.match(descriptor1, descriptor2, match);

	double maxd = 0, mind = 0;
	for (int i = 0; i < descriptor1.rows; i++) {
		double dist = match[i].distance;
		if (dist < mind) mind = dist;
		if (dist > maxd) maxd = dist;
	}

	std::vector<DMatch> good_match;
	for (int i = 0; i < descriptor1.rows; i++) {
		if (match[i].distance <= std::max(2 * mind, 0.05)) good_match.push_back(match[i]);
	}

	Mat img_match;
	drawMatches(img1, keypoint1, img2, keypoint2, good_match, img_match, Scalar::all(-1), Scalar::all(-1),
		std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("매칭결과", img_match);


	int sz = good_match.size();
	for (int i = 0; i < sz; i++) {
		std::cout << "키포인트 " << good_match[i].queryIdx << "~" << good_match[i].trainIdx << "\n";
	}


	//투영 변환 행렬 구하기

	std::vector<Point2f> model_pt;
	std::vector<Point2f> scene_pt;
	for (int i = 0; i < good_match.size(); i++) {
		model_pt.push_back(keypoint1[good_match[i].queryIdx].pt);
		scene_pt.push_back(keypoint2[good_match[i].trainIdx].pt);
	}

	Mat h = findHomography(model_pt, scene_pt, RANSAC);
	std::vector<Point2f> model_corner(4);
	model_corner[0] = Point(0, 0);
	model_corner[1] = Point(img1.cols, 0);
	model_corner[2] = Point(img1.cols, img2.rows);
	model_corner[3] = Point(0, img1.rows);

	std::vector<Point2f> scene_corner(4);
	perspectiveTransform(model_corner, scene_corner, h);

	Point2f p(img1.cols, 0);
	for (int i = 0; i < 4; i++)
		line(img_match, scene_corner[i] + p, scene_corner[i] + p, RED, 3);
	imshow("인식 결과", img_match);
	waitKey();
	return 0;
}