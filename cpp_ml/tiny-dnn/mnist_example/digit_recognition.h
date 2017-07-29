#pragma once

#include <string>
#include <iosfwd>

#include "tiny_dnn/tiny_dnn.h"

namespace digit_recognition {


  class digit_recognizer
  {
  public:

    static void convert_image(const std::string& imagefilename, tiny_dnn::vec_t& data, double minv = -1.0, double maxv = 1.0, int w=32, int h=32);

    digit_recognizer();
    virtual ~digit_recognizer();

    int predict(const tiny_dnn::vec_t  &data);
    void train();
    tiny_dnn::network<tiny_dnn::sequential> &getNN();
    const tiny_dnn::network<tiny_dnn::sequential> &getNN() const;
  private:
    tiny_dnn::core::backend_t backend_type_;
    tiny_dnn::network<tiny_dnn::sequential> network_;

#define O true
#define X false
    static constexpr const bool tbl[] = {
      O, X, X, X, O, O, O, X, X, O, O, O, O, X, O, O,
      O, O, X, X, X, O, O, O, X, X, O, O, O, O, X, O,
      O, O, O, X, X, X, O, O, O, X, X, O, X, O, O, O,
      X, O, O, O, X, X, O, O, O, O, X, X, O, X, O, O,
      X, X, O, O, O, X, X, O, O, O, O, X, O, O, X, O,
      X, X, X, O, O, O, X, X, O, O, O, O, X, O, O, O
    };
#undef O
#undef X
  };
}