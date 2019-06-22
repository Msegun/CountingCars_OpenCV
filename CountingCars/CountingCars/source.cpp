//  Krzysztof Borawski IT Year I Stage II 238152 
//	<* )	(^V^)    ( *> 
//	( <	)  <(   )>  ( > )
//	 <<	     W W     >>
// Happy penguin searching the perimeter

#include<opencv2/opencv.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"
#include "Vehicle.h"

#include<stdio.h>

#define EVER ;;
#define VIDEO_CLIP "C:\\Users\\kbora\\Desktop\\Project OpenCV\\CountingCars_OpenCV\\CountingCars\\Videos\\1.mp4"
#define WINDOW_NAME "Counting Cars - Little OpenCV project by Krzysztof Borawski"

using namespace cv;


double calc_distance(Point p1, Point p2) {

	int diff_x = abs(p1.x - p2.x);
	int diff_y = abs(p1.y - p2.y);
	return(sqrt(pow(diff_x, 2) + pow(diff_y, 2)));
}

double calc_diagonal(Rect bounding_rect) {
	return sqrt(pow(bounding_rect.width, 2) + pow(bounding_rect.height, 2));
}

void match_tracked_to_current_vehicles(std::vector<Vehicle> &tracked_vehicles, std::vector<Vehicle> &curent_vehicles) {
	for (Vehicle &v : tracked_vehicles) {
		v.found_match = false;
		v.prediction();
	}

	for (Vehicle &cv : curent_vehicles) {
		int ind = 0;
		// Mo¿liwie zmieniæ na wartoœæ
		double min_distance = INFINITY;
;

		for (int i = 0; i < tracked_vehicles.size(); i++) {
			if (tracked_vehicles[i].still_tracked) {
				double distance = calc_distance(cv.center_points.back(), tracked_vehicles[i].predicted_center);
				if (distance < min_distance) {
					min_distance = distance;
					ind = i;
				}
			}
		}

		if (min_distance < calc_diagonal(cv.bounding_rectangle) * 0.5) {
			// Update wartoœci
			tracked_vehicles[ind].contour = cv.contour;
			tracked_vehicles[ind].bounding_rectangle = cv.bounding_rectangle;
			tracked_vehicles[ind].center_points.push_back(cv.center_points.back());
			tracked_vehicles[ind].area = cv.area;
			tracked_vehicles[ind].still_tracked = true;
			tracked_vehicles[ind].found_match = true;
		}
		else {
			cv.found_match = true;
			tracked_vehicles.push_back(cv);
		}
	}

	for (int i = 0; i < tracked_vehicles.size(); i++) {

		if (!tracked_vehicles[i].found_match) {
			tracked_vehicles[i].disable_counter++;
		}

		if (tracked_vehicles[i].disable_counter >= 3) {
			tracked_vehicles.erase(tracked_vehicles.begin() + i);
		}

	}
}

bool did_vehicle_pass_the_line(Point p1, Vehicle v) {
	int size = v.center_points.size();
	if (size > 2)
		if(v.center_points[size -1].y <= p1.y && v.center_points[size - 2].y > p1.y && !v.counted)
			return true;
	else
		return false;
}
	

void count_cars() {

	Mat frame, next_frame;
	Mat grayscaled, next_grayscaled;
	Mat thresh;
	Mat convex_hulls;
	VideoCapture cap(VIDEO_CLIP);
	// cap.open(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cap >> frame;

	double thresh_value = 30;
	int width = 640;
	int heigth = 480;
	double min_value = 1000;
	int amount = 0;

	int x1 = 0;
	int x2 = frame.cols - 1;
	int y1 = 250, y2 = 250;

	resize(frame, frame, Size(width, heigth), 0, 0, INTER_CUBIC);
	Mat ui_mat(heigth+20, width+250, CV_8UC3);

	std::vector<std::vector<Point>> contours_vector;
	std::vector<Vehicle> tracked_vehicles;

	cvui::init(WINDOW_NAME);

	bool count = true;
	bool first_frame = true;

	for (EVER) {

		std::vector<Vehicle> curent_vehicles;

		cap >> frame;
		if (frame.empty()) {
			cap.open(VIDEO_CLIP);
			cap >> frame;
			amount = 0;
			first_frame = true;
		}
		else {
			cap >> next_frame;
			if (!next_frame.empty()) {
				try {
					resize(frame, frame, Size(width, heigth), 0, 0, INTER_CUBIC);
					resize(next_frame, next_frame, Size(width, heigth), 0, 0, INTER_CUBIC);

					cvui::window(ui_mat, 0, 0, width + 250, heigth + 20, "Clip & Menu");
					cvui::checkbox(ui_mat, width + 5, 25, "Count Cars", &count);
					cvui::text(ui_mat, width + 5, 40, std::to_string(amount));
					cvui::trackbar(ui_mat, width, 70, 250, &min_value, (double)0, (double)5000);
					cvui::trackbar(ui_mat, width, 100, 250, &thresh_value, (double) 10, (double) 150);

					if (count) {

						cvtColor(frame, grayscaled, CV_BGR2GRAY);
						cvtColor(next_frame, next_grayscaled, CV_BGR2GRAY);

						GaussianBlur(grayscaled, grayscaled, Size(3, 3), 0);
						GaussianBlur(next_grayscaled, next_grayscaled, Size(3, 3), 0);

						absdiff(grayscaled, next_grayscaled, thresh);
						threshold(thresh, thresh, thresh_value, 255, CV_THRESH_BINARY);

						Mat element = getStructuringElement(MORPH_RECT, cv::Size(5, 5));

						dilate(thresh, thresh, element, Point(0,0), 2);
						erode(thresh, thresh, element);

						findContours(thresh, contours_vector, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
						
						std::vector<std::vector<Point>> convex_hulls_vector(contours_vector.size());

						for (int i = 0; i < contours_vector.size(); i++) {
							convexHull(contours_vector[i], convex_hulls_vector[i]);
						}
						// To nie jest konieczne po debbugingu wykasowaæ 
						drawContours(thresh, convex_hulls_vector, -1, Scalar(255, 255, 255), -1);
						
						
						Point p1(x1, y2);
						Point p2(x2, y2);
						line(frame, p1, p2, Scalar(0, 0, 255), 1);

						for(int i = 0; i < convex_hulls_vector.size(); i++){
							Vehicle v(convex_hulls_vector[i]);
							if (v.area > min_value) {
								curent_vehicles.push_back(v);
							}
						}
						if (first_frame) {
							for (Vehicle &v : curent_vehicles) {
								tracked_vehicles.push_back(v);
							}
						}
						else {
							match_tracked_to_current_vehicles(tracked_vehicles, curent_vehicles);
						}

						for (Vehicle &v : tracked_vehicles) {
							if (v.still_tracked) {
								for (Point c : v.center_points)
									circle(frame, c, 1, Scalar(255, 255, 255), 2);
								rectangle(frame, v.bounding_rectangle, Scalar(0, 0, 255), 1);
								if (did_vehicle_pass_the_line(p1, v)) {
									amount++;
									v.counted = true;
								}
							}
						}

					}
					cvui::image(ui_mat, 0, 20, frame);
					first_frame = false;

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