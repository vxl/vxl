#ifndef mbl_index_sort_h_
#define mbl_index_sort_h_
//:
// \file
// \brief Algorithm to produced list of sorted data indicies
// \author Ian Scott

#include <vcl_vector.h>
#include <vcl_algorithm.h>

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
  for(unsigned i =0;i < n; ++i) index[i] = i;

  vcl_sort(index.begin(), index.end(), c);
}

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
  for(unsigned i =0;i < n; ++i) index[i] = i;

  vcl_sort(index.begin(), index.end(), c);
}

#endif // mbl_index_sort_h_
