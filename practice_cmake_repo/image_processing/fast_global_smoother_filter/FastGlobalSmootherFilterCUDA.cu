#include "FastGlobalSmootherFilterCUDA.h"
#include "cuda_runtime.h"
#include <opencv2/core/cuda_stream_accessor.hpp>

namespace impl {

    //INFO: original
    //inline float get_negative_weight(float p1, float p2, bool zero_norm) {
    //    //Farbman's weight works better than the one involving exp.
    //    if (zero_norm)
    //        return -1.0f / ((p1 - p2)*(p1 - p2) + 1e-7f);
    //    return -1.0f / (abs(p1 - p2) + 1e-4f);
    //}

    template<bool isZeroNormalization = false>
    struct NegativeWeightProcessor {
        float __device__ operator()(const float p1, const float p2) {
            return -1.0f / (fabsf(p1 - p2) + 1e-4f);
        }
    };

    template<>
    struct  NegativeWeightProcessor<true> {
        float __device__ operator()(const float p1, const float p2) {
            return -1.0f / ((p1 - p2)*(p1 - p2) + 1e-7f);
        }
    };

    __global__ void processHorizontalWeightsKernel(const cv::cuda::PtrStepSzf input, cv::cuda::PtrStepSzf output)
    {
        int x = blockIdx.x * blockDim.x + threadIdx.x;
        int y = blockIdx.y * blockDim.y + threadIdx.y;

        int nextX = x + 1;

        bool xValueInBound = x >= 0 && x <= input.cols - 2;
        bool nextXValueInBound = nextX >= 1 && nextX <= input.cols - 1;
        bool yValueInBound = y >= 0 && y <= input.rows - 1;

        if (xValueInBound && nextXValueInBound && yValueInBound) {
            //output(y, x) = NegativeWeightProcessor<true>()(input(y, x), input(y, nextX));
             output(y, x) = NegativeWeightProcessor<true>()(input(y, x), input(y, nextX));
        }

        bool isLastX = (x == (input.cols - 1));

        if (isLastX && yValueInBound) {
            output(y, x) = 0;
        }
    }

    __global__ void processVerticalWeightsKernel(const cv::cuda::PtrStepSzf input, cv::cuda::PtrStepSzf output) {

        int x = blockIdx.x * blockDim.x + threadIdx.x;
        int y = blockIdx.y * blockDim.y + threadIdx.y;

        int nextY = y + 1;

        bool xValueInBound = x >= 0 && x <= input.cols - 1;
        bool yValueInBound = y >= 0 && y <= input.rows - 2;
        bool nextYValueInBound = nextY >= 1 && nextY <= input.rows - 1;

        if (xValueInBound && yValueInBound && nextYValueInBound) {
            output(y, x) = NegativeWeightProcessor<true>()(input(y, x), input(nextY, x));
        }
        
        bool isLastY = (y == (input.rows - 1));
        if (isLastY && xValueInBound) {
            output(y, x) = 0;
        }
    }

    __global__ void process4RowsPerBlock(cv::cuda::PtrStepSzf chorData, cv::cuda::PtrStepSzf interD, cv::cuda::PtrStepSzf current, float lambda, int width) {
        int rowIdx = blockIdx.x * blockDim.x + threadIdx.x;

        if (rowIdx >= 0 && rowIdx <= current.rows - 1) {
            cv::cuda::PtrStepSzf::elem_type *chorRowPtr = chorData.ptr(rowIdx);
            cv::cuda::PtrStepSzf::elem_type *interRowPtr = interD.ptr(rowIdx);
            cv::cuda::PtrStepSzf::elem_type *currentRowPtr = current.ptr(rowIdx);

            float coefPrev = lambda * chorRowPtr[0];

            //INFO: head processing(forward pass) for X=0 elements
            interRowPtr[0] = coefPrev / (1 - coefPrev);
            currentRowPtr[0] = currentRowPtr[0] / (1 - coefPrev);

            //TODO: think of removing True data dependecies => might be parallelized on grid !!!
            for (int i = 1; i < width; ++i) {

                coefPrev = lambda * chorRowPtr[i - 1];
                float coefCur = lambda * chorRowPtr[i];
                float denom = (1 - coefPrev - coefCur) - interRowPtr[i - 1] * coefPrev;
                interRowPtr[i] = coefCur / denom;
                currentRowPtr[i] = (currentRowPtr[i] - currentRowPtr[i - 1] * coefPrev) / denom;
            }

            //TODO: think of removing True data dependecies => might be parallelized on grid !!!
            //backward pass:
            for (int i = width - 2; i >= 0; --i) {
                chorRowPtr[i] = chorRowPtr[i] - interRowPtr[i] * chorRowPtr[i + 1];
            }
        }
    }

    __global__ void processRow(cv::cuda::PtrStepSzf chorData, cv::cuda::PtrStepSzf interD, cv::cuda::PtrStepSzf current, float lambda, int width) {
        int rowIdx = blockIdx.x * blockDim.x + threadIdx.x;

        if (rowIdx >= 0 && rowIdx <= current.rows - 1) {
            cv::cuda::PtrStepSzf::elem_type *chorRowPtr = chorData.ptr(rowIdx);
            cv::cuda::PtrStepSzf::elem_type *interRowPtr = interD.ptr(rowIdx);
            cv::cuda::PtrStepSzf::elem_type *currentRowPtr = current.ptr(rowIdx);

            //forward pass:
            float coefPrev = lambda * chorRowPtr[0];
            interRowPtr[0] = coefPrev / (1 - coefPrev);
            currentRowPtr[0] = currentRowPtr[0] / (1 - coefPrev);

            for (int i = 1; i < width; ++i) {
                float coefCur = lambda * chorRowPtr[i];
                float denom = (1 - coefPrev - coefCur) - interRowPtr[i - 1] * coefPrev;

                interRowPtr[i] = coefCur / denom;
                currentRowPtr[i] = (currentRowPtr[i] - currentRowPtr[i - 1] * coefPrev) / denom;
                coefPrev = coefCur;
            }

            //backward pass:
            for (int i = width - 2; i >= 0; i--) {
                currentRowPtr[i] = currentRowPtr[i] - interRowPtr[i] * currentRowPtr[i + 1];
            }
        }
    }

    __global__ void processVerticaForwardlPassFirstRow(cv::cuda::PtrStepSzf cVertFirstRow, cv::cuda::PtrStepSzf interDFirstRow, cv::cuda::PtrStepSzf currentFirstRow, float lambda) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;

        constexpr const int FIRST_ROW_IDX = 0;

        if (idx >= 0 && idx < currentFirstRow.rows) {
            float coefCurrent = lambda * cVertFirstRow.ptr(FIRST_ROW_IDX)[idx];
            interDFirstRow.ptr(FIRST_ROW_IDX)[idx] = coefCurrent / (1 - coefCurrent);
            currentFirstRow.ptr(FIRST_ROW_IDX)[idx] = currentFirstRow.ptr(FIRST_ROW_IDX)[idx] / (1 - coefCurrent);
        }
    }
    //INFO: it's not possible to get the cv::cuda::GpuMat row ptr outside the kernek, so let's pass just the IDXs
    // Generaly this algo has 2 loop and outer one has the TRUE data dependecies. So just parallize by inner one

    // INFO:
    //      coef_prev = fgs->lambda*Cvert_row_prev[j];
    //      coef_cur = fgs->lambda*Cvert_row[j];
    //      denom = (1 - coef_prev - coef_cur) - interD_row_prev[j] * coef_prev;
    //      interD_row[j] = coef_cur / denom;
    //      cur_row[j] = (cur_row[j] - cur_row_prev[j] * coef_prev) / denom;

    __global__ void processVerticalForwardPass(cv::cuda::PtrStepSzf cVertMat, cv::cuda::PtrStepSzf interD, cv::cuda::PtrStepSzf current, float lambda, const int currentRowIdx, const int prevRowIdx) {
        int jIdx = blockIdx.x * blockDim.x + threadIdx.x;
        if (jIdx > 0 && jIdx < current.cols) {
            float coefPrev = lambda * cVertMat.ptr(prevRowIdx)[jIdx];
            float coefCurrent = lambda * cVertMat.ptr(currentRowIdx)[jIdx];
            float denom = (1 - coefPrev - coefCurrent) - interD.ptr(prevRowIdx)[jIdx] * coefPrev;
            interD.ptr(currentRowIdx)[jIdx] = coefCurrent / denom;
            current.ptr(currentRowIdx)[jIdx] = (current.ptr(currentRowIdx)[jIdx] - current.ptr(prevRowIdx)[jIdx] * coefPrev) / denom;
        }
    }

    //INFO:
    //  backward pass:
    //  for (int i = fgs->h - 2; i >= 0; i--)
    //  {
    //    interD_row = (WorkType*)fgs->interD.ptr(i);
    //    cur_row = (WorkType*)cur->ptr(i);
    //    cur_row_next = (WorkType*)cur->ptr(i + 1);
    //    int j = start;
    //    //USE CUDA FOR INNER LOOP
    //    for (; j < end; j++)
    //        cur_row[j] = cur_row[j] - interD_row[j] * cur_row_next[j];
    //  }

    __global__ void processVerticalBackwardPass(cv::cuda::PtrStepSzf interD, cv::cuda::PtrStepSzf current, const int currentRowIdx, const int nextRowIdx) {
        int jIdx = blockIdx.x * blockDim.x + threadIdx.x;
        if (jIdx > 0 && jIdx < current.cols) {
            current.ptr(currentRowIdx)[jIdx] = current.ptr(currentRowIdx)[jIdx] - interD.ptr(currentRowIdx)[jIdx] * current.ptr(nextRowIdx)[jIdx];
        }
    }
}

namespace ni {
    namespace cuda {
        namespace algorithms {

            void processHorizontalWeightWrapper(const cv::cuda::GpuMat &input, cv::cuda::GpuMat &output, cv::cuda::Stream &stream) {
                dim3 cthreads(16, 16);
                dim3 cblocks(
                    static_cast<int>(std::ceil(input.size().width /
                        static_cast<double>(cthreads.x))),
                    static_cast<int>(std::ceil(input.size().height /
                        static_cast<double>(cthreads.y))));

                cudaStream_t cudaStream = cv::cuda::StreamAccessor::getStream(stream);
                impl::processHorizontalWeightsKernel << < cblocks, cthreads, 0, cudaStream >> > (input, output);

                cudaError_t error = cudaGetLastError();
                if (error != cudaSuccess) {
                    throw std::runtime_error(cudaGetErrorString(error));
                }
            }

            void processVerticalWeightWrapper(const cv::cuda::GpuMat &input, cv::cuda::GpuMat &output, cv::cuda::Stream &stream) {
                dim3 cthreads(16, 16);
                dim3 cblocks(
                    static_cast<int>(std::ceil(input.size().width /
                        static_cast<double>(cthreads.x))),
                    static_cast<int>(std::ceil(input.size().height /
                        static_cast<double>(cthreads.y))));

                cudaStream_t cudaStream = cv::cuda::StreamAccessor::getStream(stream);
                impl::processVerticalWeightsKernel << < cblocks, cthreads, 0, cudaStream >> > (input, output);

                cudaError_t error = cudaGetLastError();
                if (error != cudaSuccess) {
                    throw std::runtime_error(cudaGetErrorString(error));
                }
            }

            void horizontalPass(cv::cuda::GpuMat &chorMat, cv::cuda::GpuMat &interDMat, cv::cuda::GpuMat &current, cv::cuda::Stream &stream, float lambda) {

                if (chorMat.size() != current.size() || interDMat.size() != current.size()) {
                    throw std::logic_error("Invalid matrix size");
                }

                //total processors = Height
                const int BLOCKS = 16;
                const int THREADS_PER_BLOCK = static_cast<int>(std::ceil(current.size().height / static_cast<double>(BLOCKS) ));

                cudaStream_t cudaStream = cv::cuda::StreamAccessor::getStream(stream);
                impl::process4RowsPerBlock << < BLOCKS, THREADS_PER_BLOCK, 0, cudaStream >> > (chorMat, interDMat, current, lambda, current.size().width);
                cudaError_t error = cudaGetLastError();
                if (error != cudaSuccess) {
                    throw std::runtime_error(cudaGetErrorString(error));
                }

                impl::processRow << < BLOCKS, THREADS_PER_BLOCK, 0, cudaStream >> > (chorMat, interDMat, current, lambda, current.size().width);
                error = cudaGetLastError();
                if (error != cudaSuccess) {
                    throw std::runtime_error(cudaGetErrorString(error));
                }
            }

            //INFO: original algo also has the outer loop true data dependecies
            //      so it can be parallized by inner loop only
            void verticalPass(cv::cuda::GpuMat &cVertMat, cv::cuda::GpuMat &interDMat, cv::cuda::GpuMat &current, cv::cuda::Stream &stream, float lambda, const int height) {
                if (cVertMat.size() != current.size() || interDMat.size() != current.size()) {
                    throw std::logic_error("Invalid matrix size");
                }

                const int BLOCKS = 16;
                const int THREADS_PER_BLOCK = static_cast<int>(std::ceil(current.size().width / static_cast<double>(BLOCKS)));
                cudaStream_t cudaStream = cv::cuda::StreamAccessor::getStream(stream);

                impl::processVerticaForwardlPassFirstRow << < BLOCKS, THREADS_PER_BLOCK, 0, cudaStream >> > (cVertMat, interDMat, current, lambda);

                for (int i = 1; i < height; ++i) {
                    impl::processVerticalForwardPass << < BLOCKS, THREADS_PER_BLOCK, 0, cudaStream >> > (cVertMat, interDMat, current, lambda, i, i - 1);
                }

                for (int i = height - 2; i >= 0; i--) {
                    impl::processVerticalBackwardPass << < BLOCKS, THREADS_PER_BLOCK, 0, cudaStream >> > (interDMat, current, i, i + 1);
                }

            }
        }
    }
}
