// This is core/vgui/vgui_blender_tableau.h
#ifndef vgui_blender_tableau_h_
#define vgui_blender_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Tableau to blend two images.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   27 Oct 99
//
//  Contains class  vgui_blender_tableau
//
// \verbatim
//  Modifications
//   01-OCT-2002 K.Y.McGaul - Added Doxygen style comments.
//                          - Returns the filename when image loaded from file.
//                          - Moved vgui_image_blender to vgui_blender_tableau.
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_blender_tableau_sptr.h>
#include <vil/vil_fwd.h>

class vgui_image_renderer;
class vgui_vil_image_renderer;
class vil1_image;

//: Tableau to blend two images.
//
//  To use this tableau make a vgui_image_tableau containing one of the
//  images to blend and a vgui_blender_tableau containing the other.
//  Put them both in a vgui_composite_tableau.  Set alpha to be less than
//  one to see the blended image.
class vgui_blender_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_blender_tableau_new.
  //  Creates a blender with the given image and alpha value.
  vgui_blender_tableau(char const* file, float a=1.0);

  //: Constructor - don't use this, use vgui_blender_tableau_new.
  //  Creates a blender with the given image and alpha value.
  vgui_blender_tableau(vil1_image const& img, float a=1.0);

  //: Constructor - don't use this, use vgui_blender_tableau_new.
  //  Creates a blender with the given image and alpha value.
  vgui_blender_tableau(vil_image_view_base const& img, float a=1.0);

  //: Constructor - don't use this, use vgui_blender_tableau_new.
  //  Creates a blender with the given image and alpha value.
  vgui_blender_tableau(vil_image_resource_sptr const& img, float a=1.0);

  //: Handle all events sent to this tableau.
  //  In particular use draw events to draw the blended image.
  //  Use '/' and '*' key-press events to change alpha.
  bool handle(vgui_event const &e);

  //: Return the filename of the loaded image (if it was loaded from file).
  vcl_string file_name() const;

  //: Return the type of this tableau ('vgui_blender_tableau').
  vcl_string type_name() const;

  //: Tell the blender that the image pixels have been changed.
  void reread_image();

  //: Image renderer to draw the images.
  vgui_image_renderer *renderer_;
  vgui_vil_image_renderer* vil_renderer_;

  //: Amount of this image to display (1.0 for all, 0.0 for none).
  float alpha_;

 protected:
  //: Destructor - called by vgui_blender_tableau_sptr.
 ~vgui_blender_tableau();

  //: Filename (if the image was loaded from a file).
  vcl_string filename_;
};

//: Creates a smart-pointer to a vgui_blender_tableau.
struct vgui_blender_tableau_new : public vgui_blender_tableau_sptr
{
  //: Constructor - create a blender with the given image and alpha value.
  vgui_blender_tableau_new(char const* file, float a=1.0)
    : vgui_blender_tableau_sptr(new vgui_blender_tableau(file,a)) { }

  //: Constructor - create a blender with the given image and alpha value.
  vgui_blender_tableau_new(vil1_image const& img, float a=1.0)
    : vgui_blender_tableau_sptr(new vgui_blender_tableau(img, a)) { }

  //: Constructor - create a blender with the given image and alpha value.
  vgui_blender_tableau_new(vil_image_resource_sptr const& img, float a=1.0)
    : vgui_blender_tableau_sptr(new vgui_blender_tableau(img, a)) { }

  //: Constructor - create a blender with the given image and alpha value.
  vgui_blender_tableau_new(vil_image_view_base const& img, float a=1.0)
    : vgui_blender_tableau_sptr(new vgui_blender_tableau(img, a)) { }
};

#endif // vgui_blender_tableau_h_
