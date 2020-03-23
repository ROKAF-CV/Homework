#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <algorithm>
#include <iostream>
#define RED Scalar(0,0,255)
using namespace std;
using namespace cv;

void detect_face(Mat &img, VideoWriter &output) {
	Mat sticker = imread("images/rabbit_ear2.png");
	CascadeClassifier classifier("haarcascade_frontalface_default.xml");

	if (classifier.empty()) {
		cerr << "XML load failed\n";
		return;
	}

	vector<Rect>faces;
	classifier.detectMultiScale(img, faces);

	for (Rect rc : faces) {
		//rectangle(img, rc, RED, 2);
		Mat dst;
		resize(sticker, dst, Size(rc.width, 50), 0, 0, INTER_AREA);
		Mat imageROI(img, Rect(rc.x, rc.y - dst.rows, dst.cols, dst.rows));
		Mat mask(dst);
		dst.copyTo(imageROI, mask);
	}
	output << img;
	imshow("img", img);

}
int main() {

	VideoCapture cap(0);
	if (!cap.isOpened()) {
		cerr << "Camera open failed\n";
		return 1;
	}

	//****************************************
	//Video ÀúÀå
	int w = cvRound(cap.get(CAP_PROP_FRAME_WIDTH));
	int h = cvRound(cap.get(CAP_PROP_FRAME_HEIGHT));
	double fps = cap.get(CAP_PROP_FPS);
	//fps = 30;

	int fourcc = VideoWriter::fourcc('X', '2', '6', '4');
	int delay = cvRound(1000 / fps);

	VideoWriter outputVideo("output2.mp4", fourcc, fps, Size(w, h));
	if (!outputVideo.isOpened()) {
		cerr << "File open failed\n";
		return 1;
	}

	Mat frame;
	while (1) {
		cap >> frame;
		//imshow("frame", frame);
		if (frame.empty()) break;
		detect_face(frame, outputVideo);
		if (waitKey(50) == 27) break;
	}

	destroyAllWindows();
	return 0;
}