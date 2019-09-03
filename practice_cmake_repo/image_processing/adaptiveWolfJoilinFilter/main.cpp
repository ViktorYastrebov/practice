#include "adaptiveWolfJolion.h"

int maint(int argc, char *argv[]) {
	
	cv::Mat img = imread("image.png", IMREAD_GRAYSCALE);
	
	image_processing::algorithm::LocalAdaptiveBinarization localBinarization(img);
	cv::Mat result = localBinarization.process();
	
	imwrite("result.png", result);
	
	return 0;
}