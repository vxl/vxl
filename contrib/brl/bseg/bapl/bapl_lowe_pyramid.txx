// This is algo/bapl/bapl_lowe_pyramid.txx
#ifndef bapl_lowe_pyramid_txx_
#define bapl_lowe_pyramid_txx_
//:
// \file

#include "bapl_lowe_pyramid.h"
#include <vcl_cmath.h>
#include <vcl_cassert.h>

//: Constructor
template < class T >
bapl_lowe_pyramid<T>::bapl_lowe_pyramid(unsigned octave_size, unsigned num_octaves)
 : octave_size_(octave_size)
{
  for(unsigned i=0; i<num_octaves; ++i){
    vcl_vector< vil_image_view< T > > octave( octave_size, vil_image_view< T >() );
    data_.push_back(octave);
  }
}


//: Resize the number of octaves in the pyramid
template < class T >
void 
bapl_lowe_pyramid<T>::resize(unsigned num_octaves)
{
  // an empty octave
  vcl_vector< vil_image_view< T > > octave( octave_size_, vil_image_view< T >() );
  data_.resize(num_octaves, octave);
}


//: Access by octave and sub-index
template < class T >
vil_image_view< T > &
bapl_lowe_pyramid<T>::operator() (unsigned octave, unsigned sub_index)
{
  return data_[octave][sub_index];
}


//: Access by octave and sub-index
template < class T >
const vil_image_view< T > &
bapl_lowe_pyramid<T>::operator() (unsigned octave, unsigned sub_index) const
{
  return data_[octave][sub_index];
}


//: create a 3x3x3 image estimating the 26 neighbors at location (i,j)
template < class T >
vil_image_view< T >
bapl_lowe_pyramid<T>::neighbors(unsigned index, unsigned x, unsigned y) const
{
  assert( index > 0 && index < data_.size()*octave_size_-1 );
  vil_image_view< T > neighbors(3,3,3);
  int a_oct = (index+1)/octave_size_;
  int a_sub = (index+1)%octave_size_;
  int c_oct = index/octave_size_;
  int c_sub = index%octave_size_;
  int b_oct = (index-1)/octave_size_;
  int b_sub = (index-1)%octave_size_;
  const vil_image_view< T > & above = data_[a_oct][a_sub];
  const vil_image_view< T > & current = data_[c_oct][c_sub];
  const vil_image_view< T > & below = data_[b_oct][b_sub];

  // Below level
  int scale = (b_oct < c_oct)?2:1; // need to scale by 2 if the image is large
  for(int i=-1; i<2; ++i){
    for(int j=-1; j<2; ++j){
      neighbors(i+1,j+1,0) = below((x>>b_oct)+i*scale, (y>>b_oct)+j*scale);
    }
  }
  
  // Current level
  for(int i=-1; i<2; ++i){
    for(int j=-1; j<2; ++j){
      neighbors(i+1,j+1,1) = current((x>>c_oct)+i, (y>>c_oct)+j);
    }
  }

  // Above
  for(int i=-1; i<2; ++i){
    for(int j=-1; j<2; ++j){
      neighbors(i+1,j+1,2) = above((x>>a_oct)+i, (y>>a_oct)+j);
      // need to interpolate if the image is smaller
      if (a_oct > c_oct)
        neighbors(i+1,j+1,2) = ( above((x>>a_oct),(y>>a_oct)) + neighbors(i+1,j+1,2) )/2;
    }
  }
  
  return neighbors;
}


#define BAPL_LOWE_PYRAMID_INSTANTIATE(T) \
template class bapl_lowe_pyramid<T >

#endif // bapl_lowe_pyramid_txx_
