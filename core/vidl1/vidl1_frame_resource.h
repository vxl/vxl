// This is core/vidl/vidl_frame_resource.h
#ifndef vidl_frame_resource_h_
#define vidl_frame_resource_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date Mar 18, 2005
//
//\verbatim
//  Modifications
//\endverbatim

#include <vil/vil_image_resource.h>
#include <vidl/vidl_frame.h>


//: A vil_image_resource to a frame of a vidl_movie
class vidl_frame_resource : public vil_image_resource
{
  vidl_codec_sptr codec_;
  int frame_number_;
  
 public:

  vidl_frame_resource(const vidl_codec_sptr& codec, int frame);
  vidl_frame_resource(const vidl_frame& frame);
  ~vidl_frame_resource();

  //: Dimensions
  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;

  virtual enum vil_pixel_format pixel_format() const;

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  bool get_property(char const *tag, void *prop = 0) const;
};

#endif // vidl_frame_resource_h_
