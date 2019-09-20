#include "surface_roughness.h"

#include <iostream>

auto main(int argc, char *argv[]) -> int {
	
	using namespace image_processing;
	using namespace cv;
	
	const std::string imagePath("imageTest.png");
	
	Mat image = imread(imagePath, IMREAD_GRAYSCALE);
    if (!image.empty()) {
		RoughnessProcessor roughness(image);
		double percentage = roughness.process();
		
		std::cout << "surface roughness percentage : " << percentage << ", acceptable is > 50 =>" << percentage > 0.5 ? "True" : "False" << std::endl;
	} else {
		std::cerr << "image loading is failed" << std::endl;
	}
	return 0;
}