#ifndef vidl_frame_as_image_h
#define vidl_frame_as_image_h
//-----------------------------------------------------------------------------
//
// .NAME vidl_frame_as_image - see a frame as an image
// .LIBRARY vidl
// .HEADER vxl package
// .INCLUDE vidl/vidl_frame_as_image.h
// .FILE vidl_frame_as_image.cxx
//
// .SECTION Description
//   This is a convenient container to be able to make easily
//   an image from a frame.
//
// .SECTION See also
//   vidl_frame
//
// .SECTION Author
//   Nicolas Dano, september 1999
//
// .SECTION Modifications
//   Julien ESTEVE, june 2000
//   Ported from TargetJr
//
//-----------------------------------------------------------------------------


#include <vil/vil_image_impl.h>
#include <vidl/vidl_frame_as_image_ref.h>
#include <vidl/vidl_frame_ref.h>
#include <vidl/vidl_frame.h>

class vidl_frame_as_image : public vil_image_impl
{
   public:

     vidl_frame_as_image(vidl_frame_ref frame);
     ~vidl_frame_as_image();

     vil_component_format component_format() const { return VIL_COMPONENT_FORMAT_UNSIGNED_INT; }
     int bits_per_component() const { return 8; }
     int components() const;
     int planes() const { return 1; }
     int width() const { return frame_->width(); }    
     int height() const { return frame_->height(); }    
     bool get_section(void* ib, int x0, int y0, int width, int height) const;
     bool put_section(void const* ib, int x0, int y0, int width, int height) { return false; }

   protected:

     vidl_frame_ref frame_;
};
#endif // vidl_frame_as_image_h


