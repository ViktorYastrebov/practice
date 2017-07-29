#include <iostream>
#include <stdexcept>

#include "mnist_reader.hpp"
#include "digits_recognizer.hpp"

std::pair<uint8_t, std::array<float,784>> get_image_file(int idx) {
  std::array<float,784> img_data;
  auto images = mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>();

  const float scaler = 1.0f/255.0f;
  std::transform(images.training_images[idx].begin(), images.training_images[idx].end(), img_data.begin(),
   [=](uint8_t value)
   {
     return static_cast<float>(value *scaler);
   }
   );
   return std::make_pair(images.training_labels[idx], img_data);
}


auto main(int argc, char*argv[]) -> int {

  try {
    std::string graph_path(argv[1]);
    std::string model_path(argv[2]);

    digit_recogtion::digit_predictor predictor(graph_path);
    predictor.load_model(model_path);

    const int imgID = 16877;
    std::pair<uint8_t, std::array<float, 784>>  test = get_image_file(imgID);
    int image_value = predictor.predict(test.second);
    std::cout << "predicted value = " << image_value << std::endl;
    std::cout << "expected value = " << static_cast<int>(test.first) << std::endl;

  } catch(const std::exception  &ex) {
    std::cout << ex.what() << std::endl;
  }
  return 0;
}
