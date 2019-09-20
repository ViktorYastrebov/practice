#pragma once

#include <opencv/opencv.hpp>
#include <string>

namespace image_processing {
	
	struct RoughnessProcessor {
		RoughnessProcessor(const cv::Mat &input);

		RoughnessProcessor(const RoughnessProcessor &) = delete;
		RoughnessProcessor & operator = (const RoughnessProcessor &) = delete;
		RoughnessProcessor(RoughnessProcessor &&) = delete;
		RoughnessProcessor & operator = (RoughnessProcessor &&) = delete;

		double process() const;
	private:
		std::pair<cv::Mat, cv::Mat> getPeaks(const cv::Mat &source) const;
		cv::Mat prepareImage() const;
		cv::Mat getDerivation(const cv::Mat &in) const;

		void drawHist(const cv::Mat &in, const std::string &name) const;

	private:
		cv::Mat img_;
	};
}