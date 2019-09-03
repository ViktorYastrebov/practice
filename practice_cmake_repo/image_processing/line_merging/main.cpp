#include "line_merging.h"

int maint(int argc, char * argv[]) {
	
	cv::Mat img = imread("image.png", IMREAD_GRAYSCALE);
	
	std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(thinLines, lines, 3, CV_PI / 360, 50, minLineLength_, 10);
	
	GroupLinesBuilder groupLines(maxDistanceBetweenLine_, maxSameLineSegmentDistance_);
    std::vector<cv::Vec4i> grouppedLines = groupLines(lines);
	
	
	cv::Mat result(img.size(), CV_32SC1);
	
	img.convertTo(result, CV_32SC1);
	
	for (const auto &line : grouppedLines) {		
		cv::line(result, Point(line[0], line[1]), Point(line[2], line[3]), Scalar(255, 0,0) );
    }
	imwrite("result.png", result);
	
	return 0;
}