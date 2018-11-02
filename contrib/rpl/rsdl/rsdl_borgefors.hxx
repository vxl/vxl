#ifndef rsdl_borgefors_txx_
#define rsdl_borgefors_txx_
//:
// \file

#include "rsdl_borgefors.h"
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_math.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//********************* NOTE *************************
// The map constructed is only valid 1 pixel inside it.
// In other words, the 1st row and column and the last
// row and column are invalid.
//****************************************************
template <class T>
rsdl_borgefors<T>::rsdl_borgefors()
{
  this->reset();
}

template <class T>
rsdl_borgefors<T>::rsdl_borgefors(int org_x, int org_y,
                                  int size_x, int size_y,
                                  iterator_type  begin, iterator_type end, bool release_dist_map):
  org_x_(org_x), org_y_(org_y), size_x_(size_x), size_y_(size_y)
{
  initialize(begin,end);
  if ( release_dist_map )
    distance_map_.clear();
}

template <class T>
void
rsdl_borgefors<T>::set(int org_x, int org_y,
                       int size_x, int size_y,
                       iterator_type  begin, iterator_type end, bool release_dist_map)
{
  org_x_ = org_x;
  org_y_ = org_y;
  size_x_ = size_x;
  size_y_ = size_y;

  initialize(begin,end);
  if ( release_dist_map )
    distance_map_.clear();
}

template <class T>
void
rsdl_borgefors<T>::set(int org_x, int org_y,
                       int size_x, int size_y,
                       iterator_type  begin, iterator_type end,
                       vbl_array_2d<int> index_map,
                       vbl_array_2d<int>* distance_map)
{
  // set origin and size
  org_x_ = org_x;
  org_y_ = org_y;
  size_x_ = size_x;
  size_y_ = size_y;

  // when the number of elements is large,
  // memory allocation and deallocation are too many to aford
  // count the number first and reserve the size
  unsigned num=0;
  for ( iterator_type i=begin; i!=end; ++i )
    ++num;

  // store ptrs of the internal objects to the data_
  data_.reserve( num );
  for (iterator_type i = begin; i!= end; ++i) {
    data_.push_back(i);
  }

  // set maps
  index_map_ = index_map;
  if ( distance_map )
    distance_map_ = *distance_map;

  is_valid_ = true;
}

template <class T>
void
rsdl_borgefors<T>::reset()
{
  org_x_ = org_y_ = size_x_ = size_y_ = 0;
  data_.clear();
  distance_map_.clear();
  index_map_.clear();
  is_valid_ = false;
}

template <class T>
bool
rsdl_borgefors<T>::in_map(int x, int y) const
{
  return x >= org_x_ && x < size_x_ + org_x_
      && y >= org_y_ && y < size_y_ + org_y_
      && index_map_[ y - org_y_][x - org_x_] >= 0;
}

//: Returns the approximated distance data between (x,y) and closest data record.
//  If (x,y) is out of range, -1 is returned instead.
//
template <class T>
double
rsdl_borgefors<T>::distance(int x, int y) const
{
  assert( distance_map_.rows() == index_map_.rows() &&
          distance_map_.cols() == index_map_.cols() );
  assert( in_map(x,y) );
  return distance_map_[y - org_y_][x - org_x_]/3.0;
}

//: Returns the data located closest to (x,y). If (x,y) is out of range, it aborts.
//  Therefore, should always check in_map(x,y) first.
//
template <class T>
typename rsdl_borgefors<T>::const_iterator_type
rsdl_borgefors<T>::nearest(int x, int y) const
{
  assert( in_map(x,y) );

  int index = index_map_[y - org_y_][x - org_x_];
  return data_[index];
}

template <class T>
void
rsdl_borgefors<T>::origin(int& org_x, int& org_y) const
{
  org_x = org_x_;
  org_y = org_y_;
}

template <class T>
bool
rsdl_borgefors<T>::operator==(const rsdl_borgefors<T> & rhs) const
{
  return org_x_ == rhs.org_x_  &&  org_y_ == rhs.org_y_  &&
         size_x_ == rhs.size_x_ && size_y_ == rhs.size_y_ &&
         distance_map_ == rhs.distance_map_ && index_map_ == rhs.index_map_;
}

template <class T>
void
rsdl_borgefors<T>::initialize(iterator_type  begin, iterator_type end)
{
  // when the number of elements is large,
  // memory allocation and deallocation are too many to aford
  // count the number first and reserve the size
  unsigned num=0;
  for ( iterator_type i=begin; i!=end; ++i )
    ++num;

  // 0. Store ptrs of the internal objects to the data_
  data_.reserve( num );
  for (iterator_type i = begin; i!= end; ++i) {
    data_.push_back(i);
  }

  // 1. Resize distance_map_ and index_map_ to accommodate the data and
  //    initialize the maps
  distance_map_.resize(size_y_ , size_x_ );
  index_map_.resize(size_y_ , size_x_ );
  int max_range = std::max( size_x_, size_y_) * 3;
  for (int i = 0; i < size_y_; i++)
    for (int j = 0; j < size_x_; j++) {
      distance_map_[i][j] = max_range ;
      index_map_[i][j] = -1;
    }

  // 2. Mark the data on the maps
  //
  for (unsigned int i = 0; i < data_.size(); ++i) {
    int x = vnl_math::rnd((*data_[i])[0] - org_x_);
    int y = vnl_math::rnd((*data_[i])[1] - org_y_);
    if ( x>= 0 && x < size_x_ && y>=0 && y < size_y_ )
    {
      distance_map_[y][x] = 0;
      index_map_[y][x] = i;
    }
  }

  // 3. Do the Chamfer 3-4 filtering
  //
  chamfer34();

  is_valid_ = true;

  return;
}

template <class T>
void
rsdl_borgefors<T>::chamfer34()
{
   forward_chamfer();
   backward_chamfer();

   // TODO: two corners (0, size_y_-1) and (size_x_-1, 0) are not filled in
}

//: Determines the minimum of five ints.
//
template <class T>
inline int
rsdl_borgefors<T>::minimum5(int a, int b, int c, int d, int e) const
{
  if ( a<=b && a<=c && a<=d && a<=e )
    return 1;
  else if ( b<=c && b<=d && b<=e )
    return 2;
  else if ( c<=d && c<=e )
    return 3;
  else if ( d<=e )
    return 4;
  else
    return 5;
}

//: Determines the minimum of four ints.
//
template <class T>
inline int
rsdl_borgefors<T>::minimum4(int a, int b, int c, int d) const
{
  if ( a<=b && a<=c && a<=d )
    return 1;
  else if ( b<=c && b<=d )
    return 2;
  else if ( c<=d )
    return 3;
  else
    return 4;
}

//: Performs a forward chamfer convolution on the distance_map_ and update the index_map_ accordingly
//
template <class T>
void
rsdl_borgefors<T>::forward_chamfer()
{
  for (int i=1; i<=size_y_-1; ++i)
    for (int j=1; j<size_x_-1; ++j)
    {
      const int val = minimum5(distance_map_[i-1][j-1]+4,distance_map_[i-1][j]+3,
                               distance_map_[i-1][j+1]+4, distance_map_[i][j-1]+3,
                               distance_map_[i][j]);
      switch (val)
      {
        case 1:
          distance_map_[i][j] = distance_map_[i-1][j-1]+4;
          index_map_[i][j] = index_map_[i-1][j-1];
          break;

        case 2:
          distance_map_[i][j] = distance_map_[i-1][j]+3;
          index_map_[i][j] = index_map_[i-1][j];
          break;

        case 3:
          distance_map_[i][j] = distance_map_[i-1][j+1]+4;
          index_map_[i][j] = index_map_[i-1][j+1];
          break;

        case 4:
          distance_map_[i][j] = distance_map_[i][j-1]+3;
          index_map_[i][j] = index_map_[i][j-1];
          break;

        case 5:
          break;

        default: // this can never happen
          break;
      }
    }

  // the last column is not filled in yet
  for (int i=1; i<=size_y_-1; ++i)
  {
    const int j = size_x_-1;

    const int val = minimum4(distance_map_[i-1][j-1]+4,distance_map_[i-1][j]+3,
                             distance_map_[i][j-1]+3,
                             distance_map_[i][j]);
    switch (val)
    {
      case 1:
        distance_map_[i][j] = distance_map_[i-1][j-1]+4;
        index_map_[i][j] = index_map_[i-1][j-1];
        break;

      case 2:
        distance_map_[i][j] = distance_map_[i-1][j]+3;
        index_map_[i][j] = index_map_[i-1][j];
        break;

      case 3:
        distance_map_[i][j] = distance_map_[i][j-1]+3;
        index_map_[i][j] = index_map_[i][j-1];
        break;

      case 4:
        break;

        default: // this can never happen
          break;
    }
  }
}

//: Performs a forward chamfer convolution on the distance_map_ and update the index_map_ accordingly
//
template <class T>
void
rsdl_borgefors<T>::backward_chamfer()
{
  for (int i=size_y_-2; i>=0; i--)
    for (int j=size_x_-2; j>0; j--)
    {
      const int val = minimum5(distance_map_[i][j],distance_map_[i][j+1]+3,
                               distance_map_[i+1][j-1]+4, distance_map_[i+1][j]+3,
                               distance_map_[i+1][j+1]+4 );
      switch (val)
      {
        case 1:
          break;

        case 2:
          distance_map_[i][j] = distance_map_[i][j+1]+3;
          index_map_[i][j] = index_map_[i][j+1];
          break;

        case 3:
          distance_map_[i][j] = distance_map_[i+1][j-1]+4;
          index_map_[i][j] = index_map_[i+1][j-1];
          break;

        case 4:
          distance_map_[i][j] = distance_map_[i+1][j]+3;
          index_map_[i][j] = index_map_[i+1][j];
          break;

        case 5:
          distance_map_[i][j] = distance_map_[i+1][j+1]+4;
          index_map_[i][j] = index_map_[i+1][j+1];
          break;

        default: // this can never happen
          break;
      }
    }

  // the first column is not filled in yet
  for (int i=size_y_-2; i>=0; i--)
  {
    constexpr int j = 0;

    const int val = minimum4(distance_map_[i][j],distance_map_[i][j+1]+3,
                             distance_map_[i+1][j]+3,
                             distance_map_[i+1][j+1]+4 );
    switch (val)
    {
      case 1:
        break;

      case 2:
        distance_map_[i][j] = distance_map_[i][j+1]+3;
        index_map_[i][j] = index_map_[i][j+1];
        break;

      case 3:
        distance_map_[i][j] = distance_map_[i+1][j]+3;
        index_map_[i][j] = index_map_[i+1][j];
        break;

      case 4:
        distance_map_[i][j] = distance_map_[i+1][j+1]+4;
        index_map_[i][j] = index_map_[i+1][j+1];
        break;

      default: // this can never happen
        break;
    }
  }
}


#undef RSDL_BORGEFORS_INSTANTIATE
#define RSDL_BORGEFORS_INSTANTIATE(REAL_T) \
template class rsdl_borgefors<REAL_T >

#endif // rsdl_borgefors_txx_
