#ifndef vidl_vil1_frame_as_image_h
#define vidl_vil1_frame_as_image_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
// Modifications
// Julien ESTEVE, June 2000
//     Ported from TargetJr
// 10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include <vil1/vil1_image_impl.h>
#include <vidl_vil1/vidl_vil1_frame_as_image_sptr.h>
#include <vidl_vil1/vidl_vil1_frame_sptr.h>
#include <vidl_vil1/vidl_vil1_frame.h>
//: see a frame as an image
//   This is a convenient container to be able to make easily
//   an image from a frame.
class vidl_vil1_frame_as_image : public vil1_image_impl
{
 public:

  vidl_vil1_frame_as_image(vidl_vil1_frame* frame);
  ~vidl_vil1_frame_as_image();

  vil1_component_format component_format() const { return VIL1_COMPONENT_FORMAT_UNSIGNED_INT; }
  int bits_per_component() const { return 8; }
  int components() const;
  int planes() const { return 1; }
  int width() const { return frame_->width(); }
  int height() const { return frame_->height(); }
  bool get_section(void* ib, int x0, int y0, int width, int height) const;
  bool put_section(void const* /*ib*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) { return false; }

 protected:

  // Dumb ptr to avoid cycle
  vidl_vil1_frame* frame_;
};

#endif // vidl_vil1_frame_as_image_h
