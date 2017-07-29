#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <exception>


#define SHOW_IMG 1
#if 1

void edge_detection(const std::string  &file_path) {

  //d:\\programming_workspace\\c++\\practice_cmake_repo\\data\\000031.jpg
  //d:\\programming_workspace\\c++\\practice_cmake_repo\\data\\000031.jpg

  cv::Mat srcImg = cv::imread(file_path, cv::IMREAD_COLOR);
  if (srcImg.empty()) {
    throw std::runtime_error("image is empty");
  }

  cv::Mat destImg(srcImg.size(), srcImg.type());
  cv::Mat srcGray;
  cvtColor(srcImg, srcGray, cv::COLOR_BGR2GRAY);

#ifdef SHOW_IMG
  const char window_name[] = "Window";
  cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
#endif
  cv::Mat detectedEdges;
  cv::blur(srcGray, detectedEdges, cv::Size(3, 3));
  const double threadHold1 = 8;
  const double ratio = 3.0;
  const double kernel_size = 3.0;
  cv::Canny(detectedEdges, detectedEdges, threadHold1, threadHold1* ratio, kernel_size);

  destImg = cv::Scalar::all(0);
  srcImg.copyTo(destImg, detectedEdges);

#ifdef SHOW_IMG
  cv::imshow(window_name, destImg);
  cv::waitKey(0);
#endif
}

void simple_print_text() {
  using namespace cv;
  std::string text = "Funny text inside the box";
  int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
  double fontScale = 2;
  int thickness = 3;

  Mat img(600, 800, CV_8UC3, Scalar::all(0));

  int baseline = 0;
  Size textSize = getTextSize(text, fontFace,
  fontScale, thickness, &baseline);
  baseline += thickness;

  // center the text
  Point textOrg((img.cols - textSize.width) / 2,
  (img.rows + textSize.height) / 2);

  // draw the box
  rectangle(img, textOrg + Point(0, baseline),
  textOrg + Point(textSize.width, -textSize.height),
  Scalar(0, 0, 255));
  // ... and the baseline first
  line(img, textOrg + Point(0, thickness),
  textOrg + Point(textSize.width, thickness),
  Scalar(0, 0, 255));

  // then put the text itself
  putText(img, text, textOrg, fontFace, fontScale,
  Scalar::all(255), thickness, 8);

  const char window_name[] = "Window";
  cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  cv::imshow(window_name, img);
  cv::waitKey(0);
}

void print_test(const std::string  &some_text) {
  using namespace cv;
  const char window_name[] = "Window";
  //cv::Mat imgText(cv::Size(256, 64), CV_32F);
  int width = 600;
  int height = 800;
  cv::Mat imgText(width, height, CV_8UC3, cv::Scalar::all(0));
  //FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 200, 200), 4);
  cv::putText(imgText, some_text, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(255), 1, LINE_AA);
  cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  cv::imshow(window_name, imgText);
  cv::waitKey(0);
}

auto main(int argc, char  *argv[]) -> int {

  try {
    //edge_detection(argv[1]);
    print_test("XA 123 BB");
  } catch (std::exception  &ex) {
    std::cout << "error occurs: " << ex.what() << std::endl;
  }
  catch (...) {
    std::cout << "something terrible has happened" << std::endl;
  }
  return 0;
}

#else
using namespace cv;

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
const char* window_name = "Edge Map";

// @function CannyThreshold
// @brief Trackbar callback - Canny thresholds input with a ratio 1:3

static void CannyThreshold(int, void*)
{
  //![reduce_noise]
  /// Reduce noise with a kernel 3x3
  blur(src_gray, detected_edges, Size(3, 3));
  /// Canny detector
  Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
  dst = Scalar::all(0);
  src.copyTo(dst, detected_edges);
  imshow(window_name, dst);
}

auto main(int argc, char* argv[]) -> int {
  src = imread(argv[1], IMREAD_COLOR); // Load an image

  if (src.empty())
  {
    return -1;
  }
  /// Create a matrix of the same type and size as src (for dst)
  dst.create(src.size(), src.type());
  cvtColor(src, src_gray, COLOR_BGR2GRAY);
  namedWindow(window_name, WINDOW_AUTOSIZE);
  createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

  /// Show the image
  CannyThreshold(0, 0);
  waitKey(0);
  return 0;
}
#endif