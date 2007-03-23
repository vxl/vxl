// This is brl/bbas/vidl2/vidl2_istream_image_resource.h
#ifndef vidl2_istream_image_resource_h_
#define vidl2_istream_image_resource_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Wraps an input stream and frame number as a vil_image_resource
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date Mar 23, 2007
//
//\verbatim
//  Modifications
//\endverbatim

#include <vil/vil_image_resource.h>
#include <vil/vil_pixel_format.h>
#include <vidl2/vidl2_istream_sptr.h>


//: A vil_image_resource to a frame of a vidl2_istream
// With this class you can create image resources to serveral frames
// of an open vidl2 istream.  When get_view is called the stream is
// seeked to the relevant frame number and the frame is decoded and
// converted to an image view.  The stream must be seekable unless
// views are requested in squential order.  The stream is closed
// when all image resources using it are destroyed.
class vidl2_istream_image_resource : public vil_image_resource
{
  vidl2_istream_sptr istream_;
  int frame_number_;
  unsigned int ni_;
  unsigned int nj_;
  unsigned int np_;
  vil_pixel_format format_;

 public:

  vidl2_istream_image_resource(const vidl2_istream_sptr& stream, int frame,
                               unsigned int ni, unsigned int nj, unsigned int np,
                               vil_pixel_format format);
  vidl2_istream_image_resource(const vidl2_istream_sptr& stream, int frame);
  vidl2_istream_image_resource(const vidl2_istream_sptr& stream);
  ~vidl2_istream_image_resource();

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

 private:
  //: try to find the image format and size from the current frame
  bool find_image_properties();

  //: create an empty image of the appropriate type and size
  vil_image_view_base_sptr create_empty_view() const;
};

#endif // vidl2_istream_image_resource_h_
