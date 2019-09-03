#pragma once

#include <opencv/opencv.hpp>
#include <vector>

namespace image_processing {
	namespace circle_detection {
		
		std::vector<cv::Vec3i> detectCircles(const cv::Mat &input, const int minRadius, const int maxRadius);
	}
}