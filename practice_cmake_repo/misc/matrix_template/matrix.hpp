#pragma once

template <class T, int d, int s>
class Matrix {
public:

  static const int dim = d;
  static const int size = s;

  /*enum class PRODUCT_TYPE {
    DOT, CROSS
  };*/

  Matrix();
  Matrix(const Matrix<T, d, s> & val);

  ~Matrix();

  Matrix<T, d, s> & operator = (const Matrix<T, d, s> & val);

  Matrix<T, d - 1, s>  operator [] (const int i) const;
  Matrix<T, d - 1, s> & operator [] (const int i);

  Matrix<T, d, s> & operator += (const Matrix<T, d, s> & val);
  Matrix<T, d, s> & operator -= (const Matrix<T, d, s> & val);
  
  Matrix<T, d, s> & operator *= (const T & val);
  Matrix<T, d, s> & operator /= (const T & val);

  //en.wikipedia.org/wiki/Tensor_product
  //(1,n) * (1,n) dot or cross product ?

  //template<PRODUCT_TYPE type = PRODUCT_TYPE::DOT>
  Matrix<T, d, s> & operator *= (const Matrix<T, d, s> & val);
  T operator *= (const Matrix<T, 1, s> & val);

private:
  Matrix<T, dim - 1, size> m_matrix[size];
};

template<class T, int d, int s>
Matrix<T,d, s>::Matrix() {
  for(int i=0; i < size; ++i) {
    m_matrix[i] = Matrix<T, dim -1, size>();
  }
}

template<class T, int d, int s>
Matrix<T, d, s>::Matrix(const Matrix<T, d, s> & val) {
  for(int i = 0; i < size; ++i) {
    //m_matrix[i](val[i]);
    m_matrix[i] = val[i];
  }
}

template<class T, int d, int s>
Matrix<T,d, s>::~Matrix() {}

template<class T, int d, int s>
Matrix<T, d, s> & Matrix<T, d, s>::operator = (const Matrix<T, d, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] = val[i];
  }
  return *this;
}

template<class T, int d, int s>
Matrix<T, d - 1, s>  Matrix<T, d, s>::operator [] (const int i) const {
  if(i < size ) {
    return m_matrix[i];
  }
}

template<class T, int d, int s>
Matrix<T, d - 1, s> & Matrix<T, d, s>::operator [] (const int i) {
  if(i < size ) {
    return m_matrix[i];
  }
}

template<class T, int d, int s>
Matrix<T, d, s> & Matrix<T, d, s>::operator += (const Matrix<T, d, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] +=val[i];
  }
  return *this;
}

template<class T, int d, int s>
Matrix<T, d, s> & Matrix<T, d, s>::operator -= (const Matrix<T, d, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] -= val[i];
  }
  return *this;
}

template<class T, int d, int s>
Matrix<T, d, s> & Matrix<T, d, s>::operator *= (const T & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] *=val;
  }
  return *this;
}

template<class T, int d, int s>
Matrix<T, d, s> & Matrix<T, d, s>::operator *= (const Matrix<T, d, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] *=val[i];
  }
  return *this;
}

template<class T, int d, int s>
Matrix<T, d, s> & Matrix<T, d, s>::operator /= (const T & val) {

  for(int i = 0; i < size; ++i) {
    matrix[i]/=val;
  }
  return *this;
}


template<class T, int s>
class Matrix<T,0,s>
{};


template<class T, int s>
class Matrix<T, 1, s> {
public:

  static const int size = s;
  static const int dim = 1;

  enum class PRODUCT_TYPE {
    DOT, CROSS
  };

  Matrix();
  Matrix(const Matrix<T, 1, s> & val);

  ~Matrix();

  Matrix<T, 1, s> & operator = (const Matrix<T, 1, s> & val);

  T operator [] (const int i) const;
  T & operator [] (const int i);

  Matrix<T, 1, s> & operator += (const Matrix<T, 1, s> & val);
  Matrix<T, 1, s> & operator -= (const Matrix<T, 1, s> & val);

  T operator *= (const Matrix<T, 1, s> & val);
  Matrix<T, 1, s> & operator /= (const T & val);

private:
  T m_matrix[size];
};


template<class T, int s>
Matrix<T, 1, s>::Matrix() {
  for(int i=0; i< size; ++i) {
    m_matrix[i] = T();
  }
}

template<class T, int s>
Matrix<T, 1, s>::Matrix(const Matrix<T, 1, s> & val) {
  for(int i=0; i< size; ++i) {
    //m_matrix[i](val[i]);
    m_matrix[i] = val[i];
  }
}

template<class T, int s>
Matrix<T, 1, s>::~Matrix(){}

template<class T, int s>
Matrix<T, 1, s> & Matrix<T,1,s>::operator = (const Matrix<T, 1, s> & val) {
  for(int i=0; i < size; ++i) {
    m_matrix[i] = val[i];
  }
  return *this;
}

template<class T, int s>
T Matrix<T, 1, s>::operator [] (const int i) const {
  if(i < size ) {
    return m_matrix[i];
  }
}

template<class T, int s>
T & Matrix<T, 1, s>::operator [] (const int i) {
  if(i < size ) {
    return m_matrix[i];
  }
}

template<class T, int s>
Matrix<T, 1, s> & Matrix<T, 1, s>::operator += (const Matrix<T, 1, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] += val[i];
  }
  return *this;
}

template<class T, int s>
Matrix<T, 1, s> & Matrix<T, 1, s>::operator -= (const Matrix<T, 1, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] -= val[i];
  }
  return *this;
}

template<class T, int s >
T Matrix<T, 1, s>::operator *= (const Matrix<T, 1, s> & val) {

  T val;
  for(int i=0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      val += m_matrix[i]*val[j];
    }
  }
  return val;
}

template<class T, int s >
Matrix<T, 1, s> & Matrix<T,1,s>::operator /= (const T & val) {
  for(int i=0; i < size; ++i) {
    m_matrix[i] /= val;
  }
  return *this;
}

template<class T, int s>
class Matrix<T,2, s> {
public:

  static const int size = s;
  static const int dim = 2;

  Matrix();
  Matrix(const Matrix<T, 2, s> & val);

  ~Matrix();

  Matrix<T, 2, s> & operator = (const Matrix<T, 2, s> & val);

  Matrix<T, 1, s> operator [] (const int i) const;
  Matrix<T, 1, s> & operator [] (const int i);

  Matrix<T, 2, s> & operator += (const Matrix<T, 2, s> & val);
  Matrix<T, 2, s> & operator -= (const Matrix<T, 2, s> & val);

  Matrix<T, 2, s> & operator *= (const Matrix<T, 2, s> &val);
  Matrix<T, 2, s> & operator /= (const T & val);

private:
  Matrix<T,1,s> m_matrix[size];
};

template<class T, int s>
Matrix<T, 2, s>::Matrix() {

  for(int i=0; i < size; ++i) {
    m_matrix[i] = Matrix<T,1,s>();
  }
}

template<class T, int s>
Matrix<T, 2, s>::Matrix(const Matrix<T, 2, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] = val[i];
  }
}

template<class T, int s>
Matrix<T,2,s>::~Matrix() {}

template<class T, int s>
Matrix<T, 2, s> & Matrix<T, 2, s>::operator = (const Matrix<T, 2, s> & val) {

  *this(val);
  return *this;
}

template<class T, int s>
Matrix<T, 1, s> Matrix<T,2,s>::operator [] (const int i) const {
  if(i < size) {
    return m_matrix[i];
  }
}

template<class T, int s>
Matrix<T, 1, s> & Matrix<T, 2, s>::operator [] (const int i) {
  if(i < size ) {
    return m_matrix[i];
  }
}

template<class T, int s>
Matrix<T, 2, s> & Matrix<T, 2, s>::operator += (const Matrix<T, 2, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] +=val[i];
  }
  return *this;
}

template<class T, int s>
Matrix<T, 2, s> & Matrix<T, 2, s>::operator -= (const Matrix<T, 2, s> & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] -=val[i];
  }
  return *this;
}

template<class T, int s>
Matrix<T, 2, s> & Matrix<T, 2, s>::operator *=(const Matrix<T, 2, s> &val) {

  Matrix<T, 1, s> temp[size];
  
  for(int i=0; i < size; ++i) {
    for(int j=0; j < size; ++j) {
      T sum = T();
      for(int index = 0; index < size; ++index) {
        sum += m_matrix[i][index] * val[index][j];
      }
      temp[i][j] = sum;
    }
  }

  for(int i=0; i < size; ++i) {
    m_matrix[i] = temp[i];
  }

  return *this;
}

template<class T, int s>
Matrix<T, 2, s> & Matrix<T, 2, s>::operator /= (const T & val) {

  for(int i=0; i < size; ++i) {
    m_matrix[i] /= val;
  }
  return *this;
}