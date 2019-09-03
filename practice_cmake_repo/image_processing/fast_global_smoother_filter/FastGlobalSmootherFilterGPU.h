#pragma once

#include <opencv2/core.hpp>
#include <opencv2/cudafilters.hpp>

namespace cv::cuda {

    class CV_EXPORTS_W FastGlobalSmootherFilter : public Filter {
    public:
        FastGlobalSmootherFilter(InputArray guide, double lambda, double sigmaColor, double lambdaAttenuation, int numIter, bool zeroNormalization);
        ~FastGlobalSmootherFilter() = default;

        virtual void apply(InputArray src, OutputArray dst, Stream& stream = Stream::Null()) override;
    private:
        struct Pimpl;
        cv::Ptr<Pimpl> pimpl_;
    };

    CV_EXPORTS_W Ptr<Filter> createFastGlobalSmoothFilter(InputArray guide, double lambda, double sigmaColor, double lambdaAttenuation, int numIter, bool zeroNormalization);

}