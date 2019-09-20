#include "surface_roughness.h"

#pragma warning(push)
#pragma warning(disable: 4244)

#ifdef _DEBUG
#undef _DEBUG
#include <matplotlibcpp.h>
#define _DEBUG
#else
#include <matplotlibcpp.h>
#endif
#pragma warning(pop)

#include <vector>

namespace image_processing {
	
	namespace helpers {
		void dumpToFile(const cv::Mat &floatMat, const std::string &name) {
			std::ofstream ofs(name.c_str());
			ofs << floatMat;
		}
		
	    void plotHist(const cv::Mat &in) {
			cv::Mat source;
			in.convertTo(source, CV_32S);

			std::size_t totalElements = source.total()*source.channels();
			cv::Mat flat = source.reshape(1, static_cast<int>(totalElements));

			if (!source.isContinuous()) {
				flat = flat.clone();
			}
			std::vector<int> vecData((int*)flat.datastart, (int*)flat.dataend);
			matplotlibcpp::hist(vecData, 255);
			matplotlibcpp::show();
		}

		void showMat(const std::string &name, const cv::Mat &in) {
			cv::Mat normMat;
			cv::normalize(in, normMat, 0, 255, cv::NORM_MINMAX, CV_8UC1);
			imshow(name, normMat);
			cv::waitKey();
		}
		
	}
	
	void RoughnessProcessor::drawHist(const cv::Mat &in, const std::string &/*name*/) const {
		cv::Mat source = in.clone();
		std::size_t totalElements = source.total()*source.channels();
		cv::Mat flat = source.reshape(1, static_cast<int>(totalElements));

		if (!source.isContinuous()) {
			flat = flat.clone();
		}
		std::vector<float> vecData((float*)flat.datastart, (float*)flat.dataend);

		matplotlibcpp::plot(vecData);
		matplotlibcpp::show();
	}

	RoughnessProcessor::RoughnessProcessor(const cv::Mat &input)
		:img_(input.clone())
	{}

	double RoughnessProcessor::process() const {

		Mat img = prepareImage();
		//imwrite(imageName, img);

		cv::GaussianBlur(img, img, Size(5, 5), 0);

		cv::Mat sobelX;
		cv::Mat sobelY;
		cv::Sobel(img, sobelX, CV_32F, 1, 0, 3);
		cv::Sobel(img, sobelY, CV_32F, 0, 1, 3);

		cv::Mat sobelResult;
		cv::addWeighted(sobelX, 0.5, sobelY, 0.5, 0, sobelResult);

		cv::medianBlur(sobelResult, sobelResult, 9);

		Mat sobelAbs;
		cv::convertScaleAbs(sobelResult, sobelAbs);

		cv::Mat sobelNormalized;
		cv::normalize(sobelAbs, sobelNormalized, 0, 255, cv::NORM_MINMAX);

		Scalar mean, stdDev;
		cv::meanStdDev(sobelNormalized, mean, stdDev);

		cv::Mat highThreshold = sobelNormalized < static_cast<float>(mean[0] + stdDev[0]);
		cv::Mat lowThreshold = sobelNormalized > static_cast<float>(mean[0] - stdDev[0]);

		cv::Mat mask;
		cv::bitwise_and(highThreshold, lowThreshold, mask);

		cv::Mat leftData;
		cv::multiply(sobelNormalized, mask, leftData);

		//std::string maskDump = std::filesystem::path(imageName).stem().string() + "_mask.png";
		//imwrite(maskDump, mask * 255);

		cv::Mat invert;
		cv::bitwise_not(mask, invert);

		sobelNormalized.setTo(0, invert);

		int count = cv::countNonZero(sobelNormalized);

		double percentage = static_cast<double>(count) / static_cast<double>((sobelNormalized.size().width * sobelNormalized.size().height)) * 100;
		return percentage;
	}

	cv::Mat RoughnessProcessor::prepareImage() const {

		constexpr const double RELATE_MIN = 0.2;

		cv::Mat cpy = img_.clone();

		cpy.convertTo(cpy, CV_32F);

		auto peaksAndHist = getPeaks(cpy);

		using IdxAndAplitudeVector = std::vector<std::pair<int, float>>;
		IdxAndAplitudeVector preResult;
		for (int i = 0; i < peaksAndHist.first.size().height; ++i) {
			if (peaksAndHist.first.at<uchar>(i) > 0) {
				preResult.push_back(std::make_pair(i, peaksAndHist.second.at<float>(i)));
			}
		}

		//INFO: filter by relations between the peeks
		std::vector<int> peaks;

		if (preResult.size() == 1) {
			peaks.push_back(preResult.front().first);
		} else if(preResult.size() > 1) {

			std::sort(preResult.begin(), preResult.end(), [](const auto &left, const auto &right) -> bool {
				return left.second > right.second;
			});

			//INFO: Conditions :
			// 1) we need only 2 peaks !
			// 2) and second one should be not less then 0.5 of the biggest
			std::pair<int, float> &biggest = preResult[0];
			std::pair<int, float> &next = preResult[1];

			peaks.push_back(biggest.first);


			double peaksRelation = next.second / biggest.second;
			if (peaksRelation > RELATE_MIN) {
				peaks.push_back(next.first);
			}
		}

		if (peaks.size() == 2) {
			float SEPARATOR = static_cast<float>( (peaks[0] + peaks[1]) / 2);
			cv::Mat mask1 = cpy > SEPARATOR;
			mask1.convertTo(mask1, CV_32F);
			mask1 /= 255.0f;

			double minVal, maxVal;
			cv::minMaxLoc(cpy, &minVal, &maxVal);

			cpy = (cpy - minVal) / (maxVal - minVal);

			cv::Mat mask2 = 1.0f - mask1;

			cv::Mat img1;
			cv::Mat img2;

			cv::multiply(cpy, mask1, img1, 1.0, CV_32F);
			cv::multiply(cpy, mask2, img2, 1.0, CV_32F);

			Scalar mean1, std1;
			cv::meanStdDev(img1, mean1, std1);

			Scalar mean2, std2;
			cv::meanStdDev(img2, mean2, std2);

			img1 = img1 / std1[0];
			img2 = img2 / std2[0];

			double meanDiff = std::abs(mean1[0] - std1[0] - mean2[0] - std2[0]);
			if (mean1[0] > mean2[0]) {
				cv::add(img2, meanDiff, img2, mask2 > 0, img2.type());
			} else {
				cv::add(img1, meanDiff, img1, mask1 > 0, img1.type());
			}

			cv::Mat result = img1 + img2;
			cv::normalize(result, result, 0, 255, cv::NORM_MINMAX, CV_8UC1);
			return result;
		}
		//INFO: nothing to do
		return img_.clone();
	}

	std::pair<cv::Mat,cv::Mat> RoughnessProcessor::getPeaks(const cv::Mat &source) const {

		constexpr const int HIST_KERNEL = 21;
		constexpr const int HIST_SIGMA_X = 30;

		int bins = 256;
		int histSize[] = { bins };

		float range[] = { 0, 255 };
		const float *ranges[] = { range };
		int channels[] = { 0 };
		cv::MatND hist;

		//cv::Mat source = img_.clone();
		cv::calcHist(&source, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);

		//drawHist(hist, "test_hist.png");

		cv::Mat histSmoothed(hist.size(), hist.type());

		cv::medianBlur(hist, histSmoothed, HIST_KERNEL);
		//drawHist(histSmoothed, "test_medianBlur.png");

		cv::GaussianBlur(histSmoothed, histSmoothed, Size(1, HIST_KERNEL), HIST_SIGMA_X);
		//drawHist(histSmoothed, "gaussian.png");

		constexpr const int KERNEL_SIZE = 10;
		cv::Mat movingAvarage = cv::Mat::ones(KERNEL_SIZE, 1, CV_32F) / KERNEL_SIZE;
		cv::filter2D(histSmoothed, histSmoothed, histSmoothed.depth(), movingAvarage);

		//drawHist(histSmoothed, "test_movingEvarage.png");

		histSmoothed.setTo(0, histSmoothed <= 10);

		//drawHist(histSmoothed, "test_threshold.png");

		cv::Mat derivation = getDerivation(histSmoothed);
		//drawHist(derivation, "test_derivation.png");

		//INFO: smooth the derivation, because It could have the small fluctuations at the ends of the segment !!!
		cv::Mat derivationSmoothed(derivation.size(), derivation.type());
		cv::medianBlur(derivation, derivationSmoothed, HIST_KERNEL);
		cv::GaussianBlur(derivationSmoothed, derivationSmoothed, Size(1, HIST_KERNEL), HIST_SIGMA_X);

		//drawHist(derivationSmoothed, "test_derivationSmoothed.png");

		// np.where(derivation <= 0, -1, 1)
		//cv::Mat mask = derivation <= 0;
		cv::Mat quantizeDerivation = cv::Mat::ones(derivationSmoothed.size(), CV_32FC1);
		quantizeDerivation.setTo(-1, derivationSmoothed <= 0);

		//drawHist(quantizeDerivation, "test_quant_derivation.png");

		cv::Mat peaks = getDerivation(quantizeDerivation);

		//drawHist(peaks, "test_peaks.png");

		cv::Mat resultPeeksMask = peaks < 0;

		//dumpToFile(resultPeeksMask, "result.txt");
		return std::make_pair(resultPeeksMask, hist);
	}

	cv::Mat RoughnessProcessor::getDerivation(const cv::Mat &in) const {
		static const cv::Mat xKernel = (cv::Mat_<float>(2, 1) << 1.0f,  -1.0f);

		Mat derivation;
		cv::filter2D(in, derivation, in.depth(), xKernel);
		return derivation;
	}
}