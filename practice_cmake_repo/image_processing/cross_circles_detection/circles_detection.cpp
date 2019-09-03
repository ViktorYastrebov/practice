#include <functional>

namespace image_processing {
	namespace circle_detection {
		
		
        using Contour = std::vector<cv::Point>;

        using ContoursProcessorFunc = std::function<void(const std::vector<Contour> &contours)>;
        using FilterFunctionType = std::function<bool(const Contour &contour)>;
        using ContourModiferFunc = std::function<void(cv::Mat source, const Contour &contour)>;

        void processContoursWith(cv::Mat img, FilterFunctionType filter, ContourModiferFunc modiferFunc);
        void processContoursWith(cv::Mat img, ContourModiferFunc modiferFunc);

        using ControurFunc = std::function<void(const Contour &contour)>;
        void processContoursWith(cv::Mat img, ControurFunc func);

        using ContourFuncWithVector = std::function<void(const Contour &contour, const std::vector<Contour> &contoursVector, const unsigned int idx)>;
        void processContoursWith(cv::Mat img, ContourFuncWithVector func);
        void processContoursWith(cv::Mat img, FilterFunctionType filter, ContourFuncWithVector func);
        void processContoursWith(cv::Mat img, FilterFunctionType filter, ContourFuncWithVector func, ContourFuncWithVector elseFunc);
		
		
        using namespace std;
        using namespace cv;
		
		void processContoursWith(cv::Mat img, FilterFunctionType filter, ContourModiferFunc modiferFunc) {
            vector<Contour> contours;
            vector<Vec4i> hierarchy;
            cv::findContours(img, contours, hierarchy, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

            for (const auto &c : contours) {
                if (filter(c)) {
                    modiferFunc(img,c);
                }
            }
        }

        void processContoursWith(cv::Mat img, ContourModiferFunc modiferFunc) {
            vector<Contour> contours;
            vector<Vec4i> hierarchy;
            cv::findContours(img, contours, hierarchy, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

            for (const auto &c : contours) {
                modiferFunc(img, c);
            }
        }

        void processContoursWith(cv::Mat img, ControurFunc func) {
            vector<Contour> contours;
            vector<Vec4i> hierarchy;
            cv::findContours(img, contours, hierarchy, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

            for (const auto &c : contours) {
                func(c);
            }
        }

        void processContoursWith(cv::Mat img, ContourFuncWithVector func) {
            vector<Contour> contours;
            vector<Vec4i> hierarchy;
            cv::findContours(img, contours, hierarchy, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

            //simplify the drawContour usage
            unsigned int idx = 0;
            for (const auto &c : contours) {
                func(c, contours, idx);
                ++idx;
            }
        }

        void processContoursWith(cv::Mat img, FilterFunctionType filter, ContourFuncWithVector func) {
            vector<Contour> contours;
            vector<Vec4i> hierarchy;
            cv::findContours(img, contours, hierarchy, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

            //simplify the drawContour usage
            unsigned int idx = 0;
            for (const auto &c : contours) {
                if (filter(c)) {
                    func(c, contours, idx);
                }
                ++idx;
            }
        }

        void processContoursWith(cv::Mat img, FilterFunctionType filter, ContourFuncWithVector func, ContourFuncWithVector elseFunc) {
            vector<Contour> contours;
            vector<Vec4i> hierarchy;
            cv::findContours(img, contours, hierarchy, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

            //simplify the drawContour usage
            unsigned int idx = 0;
            for (const auto &c : contours) {
                if (filter(c)) {
                    func(c, contours, idx);
                } else {
                    elseFunc(c, contours, idx);
                }
                ++idx;
            }
        }

		std::vector<cv::Vec3i> detectCircles(const cv::Mat &input, const int minRadius, const int maxRadius) {
			Mat img = input.clone();

			cv::Mat binary;
			cv::threshold(img, binary, 255 / 4, 255, THRESH_BINARY_INV);

			cv::Mat othersBlobs(input.size(), CV_8UC1, Scalar(0));
			cv::Mat dumpFiltered(input.size(), CV_8UC1, Scalar(0));
		
			std::vector<Vec3i> cirlces;
			
			processContoursWith(binary,
				[=](const Contour &c) -> bool {
					constexpr const int MINIM_POINT_REQUIRED_FOR_AREA_CALC = 5;
					if (c.size() > MINIM_POINT_REQUIRED_FOR_AREA_CALC) {
						auto area = cv::contourArea(c);
						return area > (CV_PI * minRadius * minRadius);
					}
					return false;
				},
				[&](const Contour &contour, const std::vector<Contour> &/*sourceVector*/, const unsigned int /*idx */) -> void {
					auto rotatedRect = cv::fitEllipse(contour);
					double eccentricy = rotatedRect.size.height / rotatedRect.size.width;

				cv::Moments info = cv::moments(contour);
				float x = static_cast<float>(info.m10 / info.m00);
				float y = static_cast<float>(info.m01 / info.m00);

				if (eccentricy > 0.7 && eccentricy < 1.3) {
					//approximation
					float r = (rotatedRect.size.height + rotatedRect.size.width) / 2;

					double xUm = r * params.micronsPP.X;
					double yUm = r * params.micronsPP.Y;

					cirlces.push_back({static_cast<int>(std::ceil(x)),
									   static_cast<int>(std::ceil(y))
									   static_cast<int>(std::ceil(r))}
									   );

				} else {
					constexpr const float BORDER_LENGTH = 5;
					if (x + BORDER_LENGTH < img.size().width && x - BORDER_LENGTH > 0 &&
						y + BORDER_LENGTH < img.size().height && y - BORDER_LENGTH > 0) {

						Contour resultContour;
						cv::Rect rect = rotatedRect.boundingRect();
						Scalar imageOffset(-rect.x, -rect.y);
						cv::add(contour, imageOffset, resultContour);

						cv::Mat data(rect.size(), CV_8UC1, Scalar(0));
						std::vector<Contour> contourWrapper{ resultContour };
						cv::drawContours(data, contourWrapper, 0, Scalar(255), cv::FILLED);

						constexpr const int MASK_SIZE = 3;

						//float Mat, store for getting the Radius
						Mat dist;
						distanceTransform(data, dist, DIST_L2, MASK_SIZE);

						Mat normalizedDistance;
						normalize(dist, normalizedDistance, 0, 255, NORM_MINMAX);

						// if it's normalized in the range [0.0, 1.0] we would need 0.95 threshold => 242 = 255/100 * 95
						constexpr const int THRESHOLD_95_PERCENTAGE = 242;
						Mat pointsMatBinary;
						cv::threshold(normalizedDistance, pointsMatBinary, THRESHOLD_95_PERCENTAGE, 255, THRESH_BINARY);

						pointsMatBinary.convertTo(pointsMatBinary, CV_8U, 255);

						ni::matlab_functions::processContoursWith(pointsMatBinary,
							[&](const Contour &contour) -> void {
								//we match exact point
								if (contour.size() < 5) {
									base_defect_t base(0, imageNumber, params.analysisID, false);

									float r = dist.at<float>(contour[0].y, contour[0].x);

									//cv::circle(normalizedDistance, Point(contour[0].x, contour[0].y), static_cast<int>( std::ceil(r)), Scalar(255));

									int xCenter = contour[0].x + rect.x;
									int yCenter = contour[0].y + rect.y;
									//might be not the exact precision : distanceTransform(..., 3)
									// here: Radius = MASK_SIZE
									double xUm = MASK_SIZE * params.micronsPP.X;
									double yUm = MASK_SIZE * params.micronsPP.Y;

									cirlces.push_back({xCenter, yCenter, static_cast<int>(std::ceil(r))});
								} else {
									//here is quite hard to use fitEllipse because contour.size() will be < 5 for sure
									// use this apporximation
									cv::Rect approxRect = boundingRect(contour);
									double relation = static_cast<double>(approxRect.height) / static_cast<double>(approxRect.width);
									if (relation > 0.7 && relation < 1.3) {

										//DOES NOT WORK FOR SMALL GROUP OF PIXELS
										//Moments centerInfo = cv::moments(contour);

										int xPos = static_cast<int>(std::ceil(approxRect.x + approxRect.width / 2) );
										int yPos = static_cast<int>(std::ceil(approxRect.y + approxRect.height / 2) );
										float r = dist.at<float>(yPos, xPos);

										//cv::circle(normalizedDistance, Point(xPos, yPos), static_cast<int>(std::ceil(r)) , Scalar(255));

										int xOrigin = xPos + rect.x;
										int yOrigin = yPos + rect.y;
										
										cirlces.push_back({xOrigin, yOrigin, static_cast<int>(std::ceil(r))});
									}
								}
						});
					}
				}
			});
			return cirlces;
		}
	}
}