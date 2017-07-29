#pragma once

#include <string>

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"


namespace digit_recogtion {
    class digit_predictor  final {
      public:
        digit_predictor(const std::string  &grapth);
        ~digit_predictor();

        void matrix_mul_proces();
 
      private:
        void process_status(const tensorflow::Status  &status);
      private:
        tensorflow::Session  *session_;
        tensorflow::GraphDef graph_def_;
    };
}

//template< class T, template<T> container,
