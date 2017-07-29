#include <stdexcept>
#include <iostream>
#include <fstream> 

#include "digits_recognizer.hpp"

namespace digit_recogtion
{
  digit_predictor::digit_predictor(const std::string  &graph)
  {
    tensorflow::Status status = NewSession(tensorflow::SessionOptions(), &session_);
    process_status(status);
    std::cout << "init session" << std::endl;

    status = tensorflow::ReadBinaryProto(tensorflow::Env::Default(), graph.c_str(), &graph_def_);
    if(!status.ok()) {
      status =  tensorflow::ReadTextProto(tensorflow::Env::Default(), graph.c_str(), &graph_def_);
      process_status(status);
    }
    std::cout << "read graph" << std::endl;

    status = session_->Create(graph_def_);
    process_status(status);
    std::cout << "create session with graph" << std::endl;
  }

  digit_predictor::~digit_predictor() {
    session_->Close();
  }

  int digit_predictor::predict(const std::array<float, 784> &image) {
    const int dimSeed = 784;
    tensorflow::Tensor input(tensorflow::DT_FLOAT, tensorflow::TensorShape({1,dimSeed}));
    tensorflow::Tensor keep_prob(tensorflow::DT_FLOAT, tensorflow::TensorShape());
    keep_prob.scalar<float>()() = 1.0f;
    
    //input.matrix<float>()(image.data());
    for(int i = 0; i < dimSeed; ++i) {
      input.matrix<float>()(0,i) = image[i];
    }

    std::vector<tensorflow::Tensor> outputs;
    std::vector<std::pair<std::string, tensorflow::Tensor>> inputs = {{"x", input}, {"keep_prob",keep_prob}};
    tensorflow::Status status = session_->Run(inputs, {"predict"}, {}, &outputs);
    process_status(status);
    int prediction = outputs[0].scalar<int>()(); 
    return prediction;
  }

  void digit_predictor::load_model(const std::string  &model_path) {
    std::vector<tensorflow::Tensor> outputs;
    tensorflow::Tensor checkpoint_filepath(tensorflow::DT_STRING, tensorflow::TensorShape());
    checkpoint_filepath.scalar<std::string>()() = model_path;
    tensorflow::Status status = session_->Run( {{ "save/Const", checkpoint_filepath },}, {}, {"save/restore_all"}, &outputs);
    process_status(status);
  }

  void digit_predictor::process_status(const tensorflow::Status  &status) {
    if(!status.ok()) {
       throw std::runtime_error(status.ToString());
    }
  }
}
