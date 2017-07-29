#pragma once

#include <string>
#include <array>

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"


namespace digit_recogtion {
    class digit_predictor  final {
      public:
        digit_predictor(const std::string  &grapth);
        ~digit_predictor();

        //int predict(const std::string &model_path);
        int predict(const std::array<float, 784> &image);
        void load_model(const std::string  &model_path);
      private:
        void process_status(const tensorflow::Status  &status);
      private:
        tensorflow::Session  *session_;
        tensorflow::GraphDef graph_def_;
    };
}

//template< class T, template<T> container,