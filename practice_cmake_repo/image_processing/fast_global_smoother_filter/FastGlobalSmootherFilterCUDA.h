#pragma once

#include <opencv2/core/cuda.hpp>

namespace ni {
    namespace cuda {
        namespace algorithms {


            void processHorizontalWeightWrapper(const cv::cuda::GpuMat &input, cv::cuda::GpuMat &output, cv::cuda::Stream &stream);
            void processVerticalWeightWrapper(const cv::cuda::GpuMat &input, cv::cuda::GpuMat &output, cv::cuda::Stream &stream);

            void horizontalPass(cv::cuda::GpuMat &chorMat, cv::cuda::GpuMat &interDMat, cv::cuda::GpuMat &current, cv::cuda::Stream &stream, float lambda);
            void verticalPass(cv::cuda::GpuMat &cVertMat, cv::cuda::GpuMat &interDMat, cv::cuda::GpuMat &current, cv::cuda::Stream &stream, float lambda, const int height);

        }
    }
}