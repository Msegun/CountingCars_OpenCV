#include "Vehicle.h"

Vehicle::Vehicle(std::vector<Point> contour)
{
	this->contour = contour;
	this->bounding_rectangle = boundingRect(contour);
	
	this->area = contourArea(contour);


	Point current_center;

	auto moment = moments(contour, true);

	current_center.x= int(moment.m10 / moment.m00);
	current_center.y = int(moment.m01 / moment.m00);
	//current_center.x = (this->bounding_rectangle.x * 2 + this->bounding_rectangle.width) / 2;
	//current_center.y = (this->bounding_rectangle.y * 2 + this->bounding_rectangle.height) / 2;

	this->center_points.push_back(current_center);

	this->still_tracked = true;
	this->found_match = true;
	this->counted = false;
	this->disable_counter = 0;
}

void Vehicle::prediction() {

	int delta_x = 0;
	int delta_y = 0;
	int sum_x = 0;
	int sum_y = 0;
	int i = center_points.size();

	switch (i)
	{
		case 1:
			this->predicted_center.x = this->center_points.back().x;
			this->predicted_center.y = this->center_points.back().y;
			break;

		case 2:
			delta_x = this->center_points[1].x - center_points[0].x;
			delta_y = this->center_points[1].y - center_points[0].y;
			this->predicted_center.x = this->center_points.back().x + delta_x;
			this->predicted_center.y = this->center_points.back().y + delta_y;
			break;
			 
		default:
			// we consider last 3 points
			sum_x = ((this->center_points[i - 1].x - this->center_points[i - 2].x) * 2) +
				((this->center_points[i - 2].x - this->center_points[i - 3].x) * 1);
			sum_y = ((this->center_points[i - 1].y - this->center_points[i - 2].y) * 2) +
				((this->center_points[i - 2].y - this->center_points[i - 3].y) * 1);
			delta_x = (int)std::round((float)sum_x / 3.0);
			delta_y = (int)std::round((float)sum_y / 3.0);
			this->predicted_center.x = this->center_points.back().x + delta_x;
			this->predicted_center.y = this->center_points.back().y + delta_y;
			break;
	}
}

