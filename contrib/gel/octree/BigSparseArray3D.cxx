// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
//
// Class: BigSparseArray3D
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 02 Oct 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl_cassert.h>
#include "BigSparseArray3D.h"

// Default ctor
template <class T>
BigSparseArray3D<T>::BigSparseArray3D(unsigned int n1, unsigned int n2, unsigned int n3):
  _n1(n1),
  _n2(n2),
  _n3(n3)
{
}

// Copy ctor
template <class T>
BigSparseArray3D<T>::BigSparseArray3D(const BigSparseArray3D& that)
{
  operator=(that);
}

// Assignment
template <class T>
BigSparseArray3D<T>& BigSparseArray3D<T>::operator=(const BigSparseArray3D& that)
{
  _n1 = that._n1;
  _n2 = that._n2;
  _n3 = that._n3;
  _storage = that._storage;
  return *this;
}

// Destructor
template <class T>
BigSparseArray3D<T>::~BigSparseArray3D()
{
}

// locals
inline long long bigencode(unsigned i, unsigned j, unsigned k)
{
  // A bit of an arbitrary restriction, for efficiency 
  // 2048 is 1 << 11, could be 1 << 21 for 64bit machines.
  // Use a map of tuples if you need bigger sparse arrays
  assert( (i < 2097152) && (j < 2097152) && (k < 2097152) );
  return (((unsigned long long)(i)) << 42) | (((unsigned long long)(j)) << 21) | ((unsigned long long)(k));
}

inline void bigdecode(unsigned long long v, unsigned& i, unsigned& j, unsigned& k)
{
  unsigned long long lowmask = (1U << 21) - 1;
  k = v & lowmask;
  j = (v >> 11) & lowmask;
  i = (v >> 22) & lowmask;
}

template <class T>
T& BigSparseArray3D<T>::operator () (unsigned i, unsigned j, unsigned k)
{
  //   cout << "{BigSparseArray3D(" << i << "," << j << "," << k << ") - " << "storage[" << encode(i,j,k) << "] - " << _storage[encode(i,j,k)] << "}";
  
  return _storage[bigencode(i,j,k)];
}

template <class T>
const T& BigSparseArray3D<T>::operator () (unsigned i, unsigned j, unsigned k) const
{
  Map::const_iterator p = _storage.find(bigencode(i,j,k));
  //cout << "{BigSparseArray3D(" << i << "," << j << "," << k << ") - " << "storage[" << encode(i,j,k) << "] - " << _storage[encode(i,j,k)] << "}";

  assert(p != _storage.end());

  return (*p).second;
}

template <class T>
bool BigSparseArray3D<T>::fullp(unsigned i, unsigned j, unsigned k) const
{
  // cout << "{BigSparseArray3D::fullp(" << i << "," << j << "," << k << ") - " << (_storage.find(encode(i,j,k)) != _storage.end()) << "}";

  return (_storage.find(bigencode(i,j,k)) != _storage.end());
}

template <class T>
bool BigSparseArray3D<T>::put(unsigned i, unsigned j, unsigned k, const T& t)
{
  unsigned int v = bigencode(i,j,k);
  vcl_pair<Map::iterator,bool> res = _storage.insert(Map::value_type(v,t));
  //  cout << "{BigSparseArray3D::put(" << i << "," << j << "," << k << ") - " << res.second << "}";

  return res.second;
} 

template <class T>
ostream& BigSparseArray3D<T>::print(ostream& out) const
{
  for(Map::const_iterator p = _storage.begin(); p != _storage.end(); ++p) {
    unsigned i,j,k;
    bigdecode((*p).first, i, j, k);
    out << "(" << i << "," << j << "," << k << "): " << (*p).second << endl;
  }
  return out;
}

#ifdef MAIN
int main()
{
  BigSparseArray3D<double> x;

  x(1,2,3) = 1.23;
  x(100,200,3) = 100.2003;

  cout << "123 = " << x(1,2,3) << endl;
  cout << "222 = " << x(2,2,2) << endl;

  cout << "333 is full? " << x.fullp(3,3,3) << endl;
  
  cout << x;
}
#endif
