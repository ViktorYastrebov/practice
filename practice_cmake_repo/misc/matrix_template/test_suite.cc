#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE MatrixTest

#include "matrix.hpp"
#include "test_suite.h"

inline void init(Matrix<int, 2, 2> & m, int start = 1) {
  int init = start;
  for(int i=0; i < m.size; ++i) {
    for(int j=0; j< m.size; ++j) {
      m[i][j] = init++;
    }
  }
}

void test_def_constructor() {
  
  Matrix<int,2,2> m;

  for(int i=0; i < m.size; ++i) {
    for(int j=0; j < m.size; ++j) {
      BOOST_CHECK(m[i][j] == 0);
    }
  }
}

void test_copy_constructor() {
  
  Matrix<int, 2, 2> m;
  init(m);

  Matrix<int,2,2> k(m);

  int init = 1;
  for(int i =0; i < m.size; ++i) {
    for(int j=0; j< m.size; ++j) {
      BOOST_CHECK(m[i][j] == init++);
    }
  }
}

void test_assign() {
  Matrix<int, 2, 2> m;
  init(m);

  Matrix<int, 2, 2> k = m;

  int init = 1;
  for(int i =0; i < m.size; ++i) {
    for(int j=0; j< m.size; ++j) {
      BOOST_CHECK_EQUAL(m[i][j], init);
      ++init;
    }
  }
}

void test_addition() {
  
  Matrix<int, 2, 2> m;
  init(m);

  Matrix<int, 2, 2> k;
  init(k);

  m += k;

  int init = 1;
  for(int i=0; i < m.size; ++i) {
    for(int j=0; j < m.size; ++j) {
      BOOST_CHECK_EQUAL(m[i][j], 2*init);
      ++init;
    }
  }

}

void test_substraction() {

  Matrix<int, 2, 2> m;
  init(m);

  Matrix<int, 2, 2> k;
  init(k);

  m -= k;

  for(int i=0; i < m.size; ++i) {
    for(int j=0; j < m.size; ++j) {
      BOOST_CHECK_EQUAL(m[i][j], 0);
    }
  }
}

void test_multyplication() {
  Matrix<int, 2, 2> m;
  Matrix<int, 2, 2> k;

  init(m);
  init(k,5);

  int etalon[] = {19,22,43,50};

  m*=k;

  for(int i=0; i < m.size; ++i) {
    for(int j=0; j < m.size; ++j) {
      BOOST_CHECK_EQUAL(m[i][j], etalon[i*m.size + j]);
    }
  }
}

void test_devide() {
  Matrix<int, 2, 2> m;

  int init = 2;
  for(int i =0; i < m.size; ++i) {
    for(int j =0; j < m.size; ++j) {
      m[i][j] = init;
      init+=2;
    }
  }

  m/=2;

  init = 1;
  for(int i =0; i < m.size; ++i) {
    for(int j =0; j < m.size; ++j) {
      BOOST_CHECK_EQUAL(m[i][j],init++);
    }
  }
}
