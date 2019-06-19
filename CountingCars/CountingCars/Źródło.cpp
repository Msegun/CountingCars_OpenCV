#include<opencv2/opencv.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include<stdio.h>

#define ever ;;
#define WINDOW_NAME "Counting Cars - Little OpenCV project by Krzysztof Borawski"

using namespace cv;

void count_cars() {
	
	Mat frame;
	Mat greyscaled;
	Mat thresh;
	VideoCapture cap;
	cap.open(0);
	/*cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);*/
	cap >> frame;
	int width = frame.cols;
	int heigth = frame.rows;
	Mat ui_mat(heigth+20, width+150, CV_8UC3);

	cvui::init(WINDOW_NAME);

	bool count = true;

	while (true) {
		cap >> frame;
		try {
			cvui::window(ui_mat, 0, 0, width + 150, heigth + 20, "Clip & Menu");
			cvui::checkbox(ui_mat, width + 5, 25, "Count Cars", &count);
			if (cvui::button(ui_mat, width + 5, 40, 100, 30, "Button")) {
				std::cout << width << " " << heigth << std::endl;
			}
			if (count) {
				cvtColor(frame, greyscaled, CV_BGR2GRAY);
				threshold(greyscaled, thresh, 80, 255, 1);

				circle(frame, Point(50, 50), 1, Scalar(255, 255, 1));
				// TODO ca³e gunwo zwi¹zane z ui buttonami checkboxami wyjebaæ do innej funkcjiœ
				// Ogó³em zrobienie tutaj wykrywania samochodów kurwa maæ musze znaleŸæ film
			}
			cvui::image(ui_mat, 0, 20, frame);
		}
		catch(Exception){
			cap.release();
			return;
		}

		cvui::update();
		imshow(WINDOW_NAME, ui_mat);
		imshow("thresh", thresh);
		if (cv::waitKey(20) == 27) {
			cap.release();
			return;
		}
	}
}

int main() {
	count_cars();
	destroyAllWindows();
	return 0;
}