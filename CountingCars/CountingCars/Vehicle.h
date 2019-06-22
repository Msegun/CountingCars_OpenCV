#include<opencv2/opencv.hpp>

using namespace cv;

class Vehicle
{
public:
	std::vector<cv::Point> contour; // In constructor we pass hull

	std::vector<cv::Point> center_points;
	Point predicted_center;
	double area;

	Rect bounding_rectangle;

	Vehicle(std::vector<Point> contour);
	void prediction();

	bool counted;

	bool found_match;

	bool still_tracked;

	int disable_counter;

};

