#include <stdexcept>
#include <iostream>
#include "matrix_mul.h"


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

  void digit_predictor::matrix_mul_proces() {
    const int dimSeed = 2;
    tensorflow::Tensor a(tensorflow::DT_FLOAT, tensorflow::TensorShape({dimSeed,dimSeed}));
    tensorflow::Tensor b(tensorflow::DT_FLOAT, tensorflow::TensorShape({dimSeed,dimSeed}));

    a.matrix<float>()(0,0) = 1.0f;
    a.matrix<float>()(0,1) = 2.0f;
    a.matrix<float>()(1,0) = 3.0f;
    a.matrix<float>()(1,1) = 4.0f;

    //b.matrix<float>() << 1, 2, 3, 4;
    b.matrix<float>()(0,0) = 1.0f;
    b.matrix<float>()(0,1) = 2.0f;
    b.matrix<float>()(1,0) = 3.0f;
    b.matrix<float>()(1,1) = 4.0f;

    std::cout << "a matrix:" << std::endl;
    std::cout << a.matrix<float>() << std::endl;

    std::cout << "b matrix:" << std::endl;
    std::cout << b.matrix<float>() << std::endl;

    std::vector<std::pair<std::string, tensorflow::Tensor>> inputMatrices = { { "a", a }, { "b", b }};
    std::vector<tensorflow::Tensor> outputMatrix;

    std::cout << "before session::Run" << std::endl;
    tensorflow::Status status = session_->Run(inputMatrices, {"c"}, {}, &outputMatrix);
    std::cout << "after session::Run" << std::endl;
    process_status(status);
    
    std::cout << "result matrix = " << outputMatrix[0].matrix<float>() << std::endl;
  }

  void digit_predictor::process_status(const tensorflow::Status  &status) {
    if(!status.ok()) {
       throw std::runtime_error(status.ToString());
    }
  }
}
