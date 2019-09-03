#include <opencv/opencv.hpp>


namespace image_processing {
	namespace algorithm {
		
		struct GroupLinesBuilder {
            GroupLinesBuilder(const double maxDistanceBetweenLines, const double maxSameLineDistance);

            std::vector<cv::Vec4i> operator()(const std::vector<cv::Vec4i> &inputs) const;
        private:
            const double COS_10_DEGREE_ = 0.98480775;
            const double MAX_DISTANCE_BETWEEN_LINES_;
            const double MAX_SAME_LINE_DISTANCE_;

            struct LineParams {
                int A, B, C;
            };
            using MapType = std::vector < std::pair<LineParams, std::vector<cv::Vec4i > > >;

            LineParams getLineParams(const cv::Vec4i &line) const;
            bool acceptFunc(const LineParams &params1, const LineParams &params2, const cv::Vec4i &line1, const cv::Vec4i &line2) const;
            LineParams generalizeLineParams(const LineParams &p1, const LineParams & p2) const;
            void addToVector(MapType &accum, const LineParams &params, const cv::Vec4i &line) const;
        };
		
	}
}