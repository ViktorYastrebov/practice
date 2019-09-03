#pragma once
#include <opencv2/opencv.hpp>

//TODO: Viktor Y; It goes with the Dislocation Analyzer sources.
//      As far as I can see this implementation is for 2015.
//      MIGHT NEED UPDATE. INVESTIGATE PLEASE !!!!!(had no time for that)

//This is fgs_filter from ximgproc from OpenCV, extensively modified by Ben S. More info: https://sites.google.com/site/globalsmoothing/
void opencv_contrib_modified_fastGlobalSmootherFilter(const cv::Mat &src, cv::Mat &dst, float lambda, size_t iterates = 4, size_t reweighting_iterates = 4, bool zero_norm = false);

