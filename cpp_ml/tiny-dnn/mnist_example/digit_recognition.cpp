#include <ostream>
#include <istream>

#include "digit_recognition.h"

namespace digit_recognition {


  void digit_recognizer::convert_image(const std::string& imagefilename, tiny_dnn::vec_t& data, double minv, double maxv, int w, int h) {

    tiny_dnn::image<> img(imagefilename, tiny_dnn::image_type::grayscale);
    tiny_dnn::image<> resized = resize_image(img, w, h);

    //test of resizing
    resized.save("d:\\programming_workspace\\c++\\cpp_ml\\tiny-dnn\\tests\\mnist\\8_sized.png");

    // mnist dataset is "white on black", so negate required
    std::transform(resized.begin(), resized.end(), std::back_inserter(data),
      [=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
  }

  digit_recognizer::digit_recognizer()
    :backend_type_(tiny_dnn::core::default_engine())
  {
    network_ << tiny_dnn::convolutional_layer<tiny_dnn::activation::tan_h>(32, 32, 5, 1, 6,  // C1, 1@32x32-in, 6@28x28-out
      padding::valid, true, 1, 1, backend_type_)
      << tiny_dnn::average_pooling_layer<tiny_dnn::activation::tan_h>(28, 28, 6, 2)          // S2, 6@28x28-in, 6@14x14-out
      << tiny_dnn::convolutional_layer<tiny_dnn::activation::tan_h>(14, 14, 5, 6, 16,        // C3, 6@14x14-in, 16@10x10-in
        connection_table(tbl, 6, 16),
        padding::valid, true, 1, 1, backend_type_)
      << tiny_dnn::average_pooling_layer<tiny_dnn::activation::tan_h>(10, 10, 16, 2)        // S4, 16@10x10-in, 16@5x5-out
      << tiny_dnn::convolutional_layer<tiny_dnn::activation::tan_h>(5, 5, 5, 16, 120, // C5, 16@5x5-in, 120@1x1-out
        padding::valid, true, 1, 1, backend_type_)
      << tiny_dnn::fully_connected_layer<tiny_dnn::activation::tan_h>(120, 10,        // F6, 120-in, 10-out
        true, backend_type_);
  }

  digit_recognizer::~digit_recognizer()
  {}

  int digit_recognizer::predict(const tiny_dnn::vec_t  &data) {
    return network_.predict_label(data);
  }

  void digit_recognizer::train() {
    tiny_dnn::adagrad optimizer;
    std::cout << "load models..." << std::endl;

    // load MNIST dataset
    std::vector<tiny_dnn::label_t> train_labels, test_labels;
    std::vector<tiny_dnn::vec_t> train_images, test_images;

    std::string data_dir_path("d:\\programming_workspace\\c++\\cpp_ml\\data\\");
    tiny_dnn::parse_mnist_labels(data_dir_path + "train-labels.idx1-ubyte", &train_labels);
    tiny_dnn::parse_mnist_images(data_dir_path + "train-images.idx3-ubyte", &train_images, -1.0, 1.0, 2, 2);
    tiny_dnn::parse_mnist_labels(data_dir_path + "t10k-labels.idx1-ubyte",  &test_labels);
    tiny_dnn::parse_mnist_images(data_dir_path + "t10k-images.idx3-ubyte",  &test_images, -1.0, 1.0, 2, 2);

    std::cout << "start training" << std::endl;

    tiny_dnn::progress_display disp((unsigned long)train_images.size());
    tiny_dnn::timer timer;
    int minibatch_size = 10;
    int num_epochs = 30;

    optimizer.alpha *= static_cast<tiny_dnn::float_t>(std::sqrt(minibatch_size));

    // create callback
    auto on_enumerate_epoch = [&]() {
      std::cout << timer.elapsed() << "s elapsed." << std::endl;
      tiny_dnn::result res = network_.test(test_images, test_labels);
      std::cout << res.num_success << "/" << res.num_total << std::endl;

      disp.restart((unsigned long)train_images.size());
      timer.restart();
    };

    auto on_enumerate_minibatch = [&]() {disp += minibatch_size; };
    // training
    network_.train<tiny_dnn::mse>(optimizer, train_images, train_labels, minibatch_size, num_epochs, on_enumerate_minibatch, on_enumerate_epoch);
    std::cout << "end training." << std::endl;
    // test and show results
    network_.test(test_images, test_labels).print_detail(std::cout);
  }

  tiny_dnn::network<tiny_dnn::sequential> &digit_recognizer::getNN() {
    return network_;
  }

  const tiny_dnn::network<tiny_dnn::sequential> &digit_recognizer::getNN() const {
    return network_;
  }

}