//  Krzysztof Borawski IT Year I Stage II 238152 
//	<* )	(^V^)    ( *> 
//	( <	)  <(   )>  ( > )
//	 <<	     W W     >>
// Happy penguin searching the perimeter

#include<opencv2/opencv.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <filesystem>
#include<stdio.h>

#define EVER ;;
#define VIDEO_CLIP "C:\\Users\\kbora\\Desktop\\Project OpenCV\\CountingCars_OpenCV\\CountingCars\\Videos\\Cars1.mp4"
#define WINDOW_NAME "Counting Cars - Little OpenCV project by Krzysztof Borawski"

using namespace cv;

std::vector<std::vector<Point>> erase_contours_out_of_range(std::vector<std::vector<Point>> contours, double min_size) {
	for (int i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i], false);
		if (area < min_size) {
			contours.erase(contours.begin() + i);
		}
	}
	return contours;
}

void count_cars() {

	Mat frame, next_frame;
	Mat grayscaled, next_grayscaled;
	Mat thresh;
	VideoCapture cap(VIDEO_CLIP);
	// cap.open(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cap >> frame;
	int width = 640;
	int heigth = 480;
	double min_value = 3000;
	resize(frame, frame, Size(width, heigth), 0, 0, INTER_CUBIC);
	Mat ui_mat(heigth+20, width+250, CV_8UC3);

	std::vector<std::vector<Point>> contours_vector;

	cvui::init(WINDOW_NAME);

	bool count = true;

	for (EVER) {
		cap >> frame;
		if (frame.empty()) {
			// I'm not calling cap.release()
			// Documentation says that before opening new clip VideoCapture::release() is called
			cap.open(VIDEO_CLIP);
			cap >> frame;
		}
		else {
			cap >> next_frame;
			if (!next_frame.empty()) {
				try {
					resize(frame, frame, Size(width, heigth), 0, 0, INTER_CUBIC);
					resize(next_frame, next_frame, Size(width, heigth), 0, 0, INTER_CUBIC);

					cvui::window(ui_mat, 0, 0, width + 250, heigth + 20, "Clip & Menu");
					cvui::checkbox(ui_mat, width + 5, 25, "Count Cars", &count);
					cvui::trackbar(ui_mat, width, 70, 250, &min_value, (double)0, (double)10000);

					if (cvui::button(ui_mat, width + 5, 40, 100, 30, "Button")) {
						std::cout << width << " " << heigth << std::endl;
					}

					if (count) {

						cvtColor(frame, grayscaled, CV_BGR2GRAY);
						threshold(grayscaled, thresh, 30, 255, CV_THRESH_BINARY);

						cvtColor(next_frame, next_grayscaled, CV_BGR2GRAY);

						GaussianBlur(grayscaled, grayscaled, Size(3, 3), 0);
						GaussianBlur(next_grayscaled, next_grayscaled, Size(3, 3), 0);

						absdiff(grayscaled, next_grayscaled, thresh);
						threshold(thresh, thresh, 30, 255, CV_THRESH_BINARY);

						Mat structuringElement5x5 = getStructuringElement(MORPH_RECT, cv::Size(5, 5));

						dilate(thresh, thresh, structuringElement5x5, Point(0,0), 2);
						erode(thresh, thresh, structuringElement5x5);

						findContours(thresh, contours_vector, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
						
						contours_vector = erase_contours_out_of_range(contours_vector, min_value);

						std::vector<std::vector<Point>> convex_hulls_vector(contours_vector.size());

						for (int i = 0; i < contours_vector.size(); i++) {
							convexHull(contours_vector[i], convex_hulls_vector[i]);
						}
						// To nie jest konieczne po debbugingu wykasowaæ 
						drawContours(thresh, convex_hulls_vector, -1, Scalar(255, 255, 255), -1);

						// Wyznaczenie tylko tych wiêkszych


						// Sprawdzanie dupereli
						// circle(frame, Point(50, 50), 1, Scalar(255, 255, 1));
						//rectangle(frame, Rect(-10, -10, 60, 20), Scalar(255, 255, 1), 1, 8);
					}
					cvui::image(ui_mat, 0, 20, frame);
				}
				catch (Exception) {
					cap.release();
					return;
				}
			}
		}

		cvui::update();
		imshow(WINDOW_NAME, ui_mat);
		imshow("thresh", thresh);
		if (waitKey(30) == 27) {
			cap.release();
			return;
		}
	}
}

int main() {
	count_cars();
	destroyAllWindows();

	system("pause");
	return 0;
}