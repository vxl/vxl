// This is algo/bapl/bapl_lowe_pyramid.h
#ifndef bapl_lowe_pyramid_h_
#define bapl_lowe_pyramid_h_
//:
// \file
// \brief A Lowe-style image pyramid
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Wed Nov 19 2003
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vil/vil_image_view.h>
//#include <vil/vil_image_view_base.h>


template< class T >
class bapl_lowe_pyramid  
{
public:
  //: Constructor
  bapl_lowe_pyramid(unsigned octave_size=3, unsigned num_octaves=0);

  //: Resize the number of octaves in the pyramid
  void resize(unsigned num_octaves);

  //: Access by octave and sub-index
  vil_image_view< T > & operator() (unsigned octave, unsigned sub_index);

  //: Access by octave and sub-index
  const vil_image_view< T > & operator() (unsigned octave, unsigned sub_index) const;

  //: create a 3x3x3 image estimating the 26 neighbors at location (i,j)
  vil_image_view< T > neighbors(unsigned index, unsigned x, unsigned y) const;
  
private:
  //: The image data
  vcl_vector< vcl_vector< vil_image_view< T > > > data_;
  
  //: Number of images per octave
  int octave_size_;

};

#endif // bapl_lowe_pyramid_h_
