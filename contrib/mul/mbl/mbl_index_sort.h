#ifndef mbl_index_sort_h_
#define mbl_index_sort_h_
//:
// \file
// \brief Algorithm to produce list of sorted data indices
// \author Ian Scott

#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_functional.h>

//: Implementation class - Do No Use.
template <class T>
struct mbl_index_sort_cmp1
{
  const T *data;
  bool operator () (const unsigned &a, const unsigned &b)
  {
    return data[a] < data[b];
  }
};

//: Sort n elements, giving the resulting order in index.
//  data[index[0]] is smallest element, data[index[n-1]] is largest
template <class T>
void mbl_index_sort(const T* data, int n, vcl_vector<int>& index)
{
  mbl_index_sort_cmp1<T> c;
  c.data = data;

  index.resize(n);
  for (int i =0;i < n; ++i) index[i] = i;

  vcl_sort(index.begin(), index.end(), c);
}

//: Sort n elements, giving the resulting order in index.
//  data[index[0]] is smallest element, data[index[n-1]] is largest
template <class T>
void mbl_index_sort(const T* data, unsigned n, vcl_vector<unsigned>& index)
{
  mbl_index_sort_cmp1<T> c;
  c.data = data;

  index.resize(n);
  for (unsigned i =0;i < n; ++i) index[i] = i;

  vcl_sort(index.begin(), index.end(), c);
}

//: Implementation class - Do No Use.
template <class T>
struct mbl_index_sort_cmp2
{
  const vcl_vector<T> *data;
  bool operator () (const unsigned &a, const unsigned &b)
  {
    return (*data)[a] < (*data)[b];
  }
};

//: Sort n elements, giving the resulting order in index
//  data[index[0]] is smallest element, data[index[n-1]] is largest
template <class T>
void mbl_index_sort(const vcl_vector<T>& data, vcl_vector<int>& index)
{
  mbl_index_sort_cmp2<T> c;
  c.data = &data;

  unsigned n = data.size();
  index.resize(n);
  for (unsigned i =0;i < n; ++i) index[i] = i;

  vcl_sort(index.begin(), index.end(), c);
}

//: Sort n elements, giving the resulting order in index
//  data[index[0]] is smallest element, data[index[n-1]] is largest
template <class T>
void mbl_index_sort(const vcl_vector<T>& data, vcl_vector<unsigned>& index)
{
  mbl_index_sort_cmp2<T> c;
  c.data = &data;

  unsigned n = data.size();
  index.resize(n);
  for (unsigned i =0;i < n; ++i) index[i] = i;

  vcl_sort(index.begin(), index.end(), c);
}

//: A comparator for general index sorting.
// It will take any type of index on to any sort of container
// so long as T container.operator[](index) const is defined.
// 
// For example, a simple index sort can be done as follows.
// \code
//  vcl_vector data<double> (n);
//  vcl_vector index<unsigned> (n/2);
//  ...
//  vcl_sort(index.begin(), index.end(), mbl_index_sort_cmp<double>(data));
// \endcode
template <class T, class INDEX=unsigned, class CONT = vcl_vector<T>,
  class CMP=vcl_less<T> >
  struct mbl_index_sort_cmp: public vcl_binary_function<INDEX, INDEX, bool>
{
  explicit mbl_index_sort_cmp(const CONT &data, const CMP &c = CMP()):
    data_(data), cmp_(c) {}
  const CONT &data_;
  const CMP &cmp_;
  bool operator () (const INDEX &a, const INDEX &b) const
  {  return cmp_(data_[a], data_[b]); } 
};


#endif // mbl_index_sort_h_
