#include "circles_detection.h"

int main(int argc, char * argv[]) {
	
	cv::Mat img = imread("image.png", IMREAD_GRAYSCALE);
	
	cv::Mat outImg(img.size(), CV_32SC3);
	
	auto circles = image_processing::circle_detection::detectCircles(img, 10, 60);
	
	img.convertTo(outImg, CV_32SC3);
	
	
	for(const auto & c: circles) {
		cv::circle(outImg, Point(c[0], c[1]), c[2], Scalar(255, 0, 0));
	}
	imwrite("out.png", outImg);
	
	return 0;
}