#ifndef vidl_vil1_frame_h
#define vidl_vil1_frame_h
//:
// \file
// \author Nicolas Dano, march 1999
//
// \verbatim
//  Modifications
//   May 2000  Julien Esteve          Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//   10/9/2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vil1/vil1_image.h>
#include <vidl_vil1/vidl_vil1_frame_sptr.h>
#include <vidl_vil1/vidl_vil1_codec.h>

//: A single frame of a Video Sequence.
class vidl_vil1_frame : public vbl_ref_count
{
   // PUBLIC INTERFACE
 public:

  // Constructors/Initializers/Destructors
  vidl_vil1_frame(int position, vidl_vil1_codec_sptr coder);
  vidl_vil1_frame(vidl_vil1_frame const& f)
    : vbl_ref_count(), position_(f.position_), coder_(f.coder_) {}
  ~vidl_vil1_frame();

  vil1_image get_image();
  virtual bool get_section(
                void* ib, int x0, int y0, int width, int heigth) const;

  vidl_vil1_codec_sptr get_codec() {return coder_;}

  inline char get_image_class()const { return coder_->get_image_class(); }
  inline char get_format() const     { return coder_->get_format(); }
  inline int  width() const     { return coder_->width(); }
  inline int  height() const     { return coder_->height(); }
  inline int  get_bits_pixel() const { return coder_->get_bits_pixel(); }
  inline int  get_bytes_pixel()const { return coder_->get_bytes_pixel(); }

  //: Return the index of this frame in the movie.
  int get_real_frame_index() const { return position_; }

 protected:

  // Data Members
  const int position_;
  vidl_vil1_codec_sptr coder_;
  vil1_image image_;
};

#endif // vidl_vil1_frame_h
