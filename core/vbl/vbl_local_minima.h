// This is core/vbl/vbl_local_minima.h
#ifndef vbl_local_minima_h_
#define vbl_local_minima_h_
//:
// \file
// \brief Find local minima in arrays
// \author J.L. Mundy
//  October 2, 2010
// \verbatim
//  Modifications
//   <None>
// \endverbatim

#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>

//: Find the local minima in arrays
//  The result is an array with non-zero elements where local minima
//  exist. A local minimum must be smaller than \e all neighboring elements
//  by a threshold.
//  The result is an array with non-zero elements at minima.
//  The value of non-zero element indicates the smallest difference
//  between the minima and the neighboring elements.
template <class T>
vbl_array_1d<T> vbl_local_minima(vbl_array_1d<T> const& in, T thresh = T(0))
{
  vbl_array_1d<T> m(in.size(), T(0));
  if (local_minima(in, m, thresh)) return m;
  else return vbl_array_1d<T>();
}

//: Find the local minima in arrays
//  The result is an array with non-zero elements where local minima
//  exist. A local minimum must be smaller than \e all neighboring elements
//  by a threshold. The neighborhoods are 4-connected.
//  The result is an array with non-zero elements at minima.
//  The value of non-zero element indicates the smallest difference
//  between the minima and the neighboring elements.
template <class T>
vbl_array_2d<T> vbl_local_minima(vbl_array_2d<T> const& in, T thresh = T(0))
{
  vbl_array_2d<T> m(in.rows(), in.cols(), T(0));
  if (local_minima(in, m, thresh)) return m;
  else return vbl_array_2d<T>();
}

//: Find the local minima in arrays
//  The result is an array with non-zero elements where local minima
//  exist. A local minimum must be smaller than \e all neighboring elements
//  by a threshold. The neighborhoods are 8-connected.
//  The result is an array with non-zero elements at minima.
//  The value of non-zero element indicates the smallest difference
//  between the minima and the neighboring elements.
template <class T>
vbl_array_3d<T> vbl_local_minima(vbl_array_3d<T> const& in, T thresh = T(0))
{
  vbl_array_3d<T> m=in;
  if (local_minima(in, m, thresh)) return m;
  else return vbl_array_3d<T>();
}


//: DEPRECATED
// \deprecated in favour of vbl_local_minima
template <class T>
bool local_minima(vbl_array_1d<T> const& in, vbl_array_1d<T>& minima,
                  T thresh = T(0));

//: DEPRECATED
// \deprecated in favour of vbl_local_minima
template <class T>
bool local_minima(vbl_array_2d<T> const& in, vbl_array_2d<T>& minima,
                  T thresh = T(0));

//: DEPRECATED
// \deprecated in favour of vbl_local_minima
template <class T>
bool local_minima(vbl_array_3d<T> const& in, vbl_array_3d<T>& minima,
                  T thresh = T(0));

#endif // vbl_local_minima_h_
