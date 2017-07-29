#include <fstream>

#include "digit_recognition.h"

auto main(int argc, char*argv[]) -> int {

  std::string saveModelPath("d:\\programming_workspace\\c++\\cpp_ml\\tiny-dnn\\models\\mnist\\model_net");
  digit_recognition::digit_recognizer recognizer;

  //recognizer.train();
  //recognizer.getNN().save(saveModelPath);

  tiny_dnn::vec_t imgData;
  std::string imgFile("d:\\programming_workspace\\c++\\cpp_ml\\tiny-dnn\\tests\\mnist\\8.jpg");
  digit_recognition::digit_recognizer::convert_image(imgFile, imgData);

  recognizer.getNN().load(saveModelPath);

  //tiny_dnn::label_t label = 8;

  /*tiny_dnn::vec_t imgData;
  tiny_dnn::label_t label;
  {
    std::vector<tiny_dnn::vec_t> train_images;
    std::vector<tiny_dnn::label_t> labels;
    tiny_dnn::parse_mnist_images("d:\\programming_workspace\\c++\\cpp_ml\\data\\train-images.idx3-ubyte", &train_images, -1.0, 1.0, 2, 2);
    imgData = train_images[8893];

    tiny_dnn::parse_mnist_labels("d:\\programming_workspace\\c++\\cpp_ml\\data\\train-labels.idx1-ubyte", &labels);
    label = labels[8893];
  }*/


  //std::cout << "expected result = " << label << std::endl;
  int digit = recognizer.predict(imgData);
  std::cout << "predicted resutl = " << digit << std::endl;

  return 0;
}