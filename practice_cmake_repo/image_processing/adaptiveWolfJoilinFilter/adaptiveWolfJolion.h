#pragma once

#include <opencv/opencv.hpp>

namespace image_processing {
    namespace algorithm {

        //INFO: 
        // all sources are got from the https://github.com/chriswolfvision/local_adaptive_binarization
        // and has been adopted.
        // 

        class LocalAdaptiveBinarization final {
        public:
            LocalAdaptiveBinarization(const cv::Mat &image, bool invert = false, const float valueK = 0.5);
            ~LocalAdaptiveBinarization() = default;

            LocalAdaptiveBinarization(const LocalAdaptiveBinarization&) = delete;
            LocalAdaptiveBinarization & operator = (const LocalAdaptiveBinarization&) = delete;

            cv::Mat process() const;
        private:
            double calcLocalStats(cv::Mat &im, cv::Mat &map_m, cv::Mat &map_s, int winx, int winy) const;
            void NiblackSauvolaWolfJolion(cv::Mat im, cv::Mat output, int winx, int winy, double k) const;
        private:
            const float optK_;
            cv::Mat imageData_;
            bool isInver_;
        };
	}
}