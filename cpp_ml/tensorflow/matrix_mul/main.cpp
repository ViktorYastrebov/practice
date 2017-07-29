#include <iostream>
#include <stdexcept>

#include "matrix_mul.h"

/*
Python code:

with tf.Session() as sess:


    a = tf.placeholder(tf.float32, [2,2], name='a')
    b = tf.placeholder(tf.float32, [2,2], name='b')
    c = tf.mul(a,b, name='c')
    print(sess.run(c,feed_dict={ a: [[1.0, 2.0],[3.0,4.0]], b: [[1.0, 2.0],[3.0,4.0]]}) )
    tf.train.write_graph(sess.graph_def, "model/", "matrix_mul.pb", as_text=False)
*/

auto main(int argc, char*argv[]) -> int {

  try {
    std::string path("matrix_mul.pb");
    digit_recogtion::digit_predictor predictor(path);
    predictor.matrix_mul_proces();
  } catch(const std::exception  &ex) {
    std::cout << ex.what() << std::endl;
  }
  return 0;
}
