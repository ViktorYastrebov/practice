#include "stdafx.h"
#include "FastGlobalSmootherFilterGPU.h"
#include "FastGlobalSmootherFilterCUDA.h"
#include <opencv2/core/cuda_stream_accessor.hpp>


namespace cv::cuda {

    struct FastGlobalSmootherFilter::Pimpl {

        using WorkType = float;
        static constexpr int WorkVectorType = CV_32FC1;

        Pimpl(InputArray guide, double lambda, double sigmaColor, double lambdaAttenuation, int numIter, bool zeroNormalization);

        cv::cuda::GpuMat sourceMat_;
        cv::cuda::GpuMat descMat_;
        float sigmaColor_;
        float lambda_;
        float lambdaAttenuation_;
        int numIter_;
        bool zeroNormalization_;

        int width_;
        int height_;

        GpuMat guideMat_;
        GpuMat chorMat_;
        GpuMat cVertMat_;
        GpuMat interDMat_;

        void processHorizontalWeghts(const cv::cuda::GpuMat &src, cv::cuda::GpuMat &desc, cv::cuda::Stream &stream);
        void processVericalWeights(const cv::cuda::GpuMat &src, cv::cuda::GpuMat &desc, cv::cuda::Stream &stream);

        void generalProcessing(const cv::cuda::GpuMat &src, cv::cuda::GpuMat &desc, cv::cuda::Stream &stream);

    private:
        void horizontalPass(cv::cuda::GpuMat &dest, cv::cuda::Stream &stream);
        void virticalPass(cv::cuda::GpuMat &dest, cv::cuda::Stream &stream);
    };

    FastGlobalSmootherFilter::Pimpl::Pimpl(InputArray guide, double lambda, double sigmaColor, double lambdaAttenuation, int numIter, bool zeroNormalization)
        : sigmaColor_(static_cast<float>(sigmaColor))
        , lambda_(static_cast<float>(lambda))
        , lambdaAttenuation_(static_cast<float>(lambdaAttenuation))
        , numIter_(numIter_)
        , zeroNormalization_(zeroNormalization)
        , width_(guide.cols())
        , height_(guide.rows())
    {
        CV_Assert(!guide.empty() && lambda >= 0 && sigmaColor >= 0 && numIter >= 1);
        CV_Assert(guide.depth() == CV_32F && (guide.channels() == 1));

        chorMat_.create(height_, width_, WorkVectorType);
        cVertMat_.create(height_, width_, WorkVectorType);
        interDMat_.create(height_, width_, WorkVectorType);

        //TODO: might no need to do it.
        // Guide Matrix acually is the SOURCE matrix.
        // Also Desctination - is the COPY OF SOURCE.
        // THIS MEANS THAT THE OPERATION ITSELF IS UNARY => G(Mat) => Mat

        guideMat_ = guide.getGpuMat().clone();
#if 0
        sigmaColor_ = static_cast<float>(sigmaColor);
        lambda_ = static_cast<float>(lambda);
        lambdaAttenuation_ = static_cast<float>(lambdaAttenuation);
        numIter_ = numIter;
        zeroNormalization_ = zeroNormalization;
        //TODO: Viktor Y: INVESTIGATE we are using the TBB also !!!
        //num_stripes = getNumThreads();
        num_stripes = 1;
        //int num_levels = 3*256*256;
        width_ = guide.cols();
        height_ = guide.rows();

        w = guide.cols();
        h = guide.rows();
        Chor.create(h, w, WorkVec_type);
        Cvert.create(h, w, WorkVec_type);
        interD.create(h, w, WorkVec_type);
        Mat guideMat = guide.getMat();
#endif

#if 0
        parallel_for_(Range(0, num_stripes), ComputeHorizontalWeights_ParBody(*this, guideMat, num_stripes, h));
        parallel_for_(Range(0, num_stripes), ComputeVerticalWeights_ParBody(*this, guideMat, num_stripes, w));
#endif
    }

    void FastGlobalSmootherFilter::Pimpl::processHorizontalWeghts(const cv::cuda::GpuMat &input, cv::cuda::GpuMat &output, cv::cuda::Stream &stream) {
        ni::cuda::algorithms::processHorizontalWeightWrapper(input, output, stream);
    }

    void FastGlobalSmootherFilter::Pimpl::processVericalWeights(const cv::cuda::GpuMat &input, cv::cuda::GpuMat & output, cv::cuda::Stream &stream) {
        ni::cuda::algorithms::processVerticalWeightWrapper(input, output, stream);
    }

    void FastGlobalSmootherFilter::Pimpl::generalProcessing(const cv::cuda::GpuMat &src, cv::cuda::GpuMat &dest, cv::cuda::Stream &stream) {

        float lambdaLocalCopy = lambda_;
        src.copyTo(dest);
        for (int i = 0; i < numIter_; ++i) {
            horizontalPass(dest, stream);
            virticalPass(dest, stream);
            lambda_ *= lambdaAttenuation_;
        }
        lambda_ = lambdaLocalCopy;
#if 0
        float lambda_ref = lambda;
        lambda = lambda_ref;

        //Move data in src to dst.
        Mat& dstMat = dst.getMatRef();
        src.getMat().copyTo(dstMat);

        //Smoothen dst in-place.
        for (int n = 0; n < num_iter; n++)
        {
            horizontalPass(dstMat);
            verticalPass(dstMat);
            lambda *= lambda_attenuation;
        }
        lambda = lambda_ref;
#endif
    }

    void FastGlobalSmootherFilter::Pimpl::horizontalPass(cv::cuda::GpuMat &dest, cv::cuda::Stream &stream) {
        ni::cuda::algorithms::horizontalPass(chorMat_, interDMat_, dest, stream, lambda_);
    }

    void FastGlobalSmootherFilter::Pimpl::virticalPass(cv::cuda::GpuMat &dest, cv::cuda::Stream &stream) {
        ni::cuda::algorithms::verticalPass(cVertMat_, interDMat_, dest, stream, lambda_, height_);
    }

//TODO: Implementation is not finished for now.
//      Also It needs to be covered by testcases

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //TODO: remove guide from here !!!
    FastGlobalSmootherFilter::FastGlobalSmootherFilter(InputArray guide, double lambda, double sigmaColor, double lambdaAttenuation, int numIter, bool zeroNormalization)
        :pimpl_(new Pimpl(guide, lambda, sigmaColor, lambdaAttenuation, numIter, zeroNormalization))
    {}

    void FastGlobalSmootherFilter::apply(InputArray src, OutputArray dst, Stream& stream) {
        const cv::cuda::GpuMat input = src.getGpuMat();
        cv::cuda::GpuMat out = dst.getGpuMat();

        //TODO: check input/output
        pimpl_->processHorizontalWeghts(input, out, stream);
        //TODO: must be called internaly, input = guideMat, output = cVertMat which is initialized in ctor
        pimpl_->processVericalWeights(input, out, stream);
        pimpl_->generalProcessing(input, out, stream);
    }

    Ptr<Filter> createFastGlobalSmoothFilter(InputArray guide, double lambda, double sigmaColor, double lambdaAttenuation, int numIter, bool zeroNormalization) {
        return makePtr<FastGlobalSmootherFilter>(guide, lambda, sigmaColor, lambdaAttenuation, numIter, zeroNormalization);
    }
}
