// This is oxl/vgui/vgui_roi_tableau.h
#ifndef vgui_roi_tableau_h_
#define vgui_roi_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Marko Bacic, Robotics Research Group, University of Oxford
// \date   18 Jul 2000
// \brief  Tableau which makes an ROI of an image act like a whole image.
//
//  ROI means "region of interest".  In this case it can only be
//  a rectangular region.
//  Contains classes:  vgui_roi_tableau  vgui_roi_tableau_new
//
// \verbatim
//  Modifications:
//    18-Jul-2000  Marko Bacic - Initial version.
//    08-Aug-2002  K.Y.McGaul - Changed to Doxygen style comments.
// \endverbatim


#include <vgui/vgui_tableau.h>
#include <vil/vil_image.h>
class vil_crop_image_impl;
class vil_image;
class vgui_image_tableau;

#include "vgui_roi_tableau_sptr.h"

//: Tableau which makes an ROI of an image act like a whole image.
//
//  ROI means "region of interest".  In this case it can only be
//  a rectangular region.
class vgui_roi_tableau : public vgui_tableau
{
 public:
  //: Data defining a region of interest (position, height and width).
  typedef struct region_of_interest {
    float sx;
    float sy;
    float width;
    float height;
  }ROI;

  //: Constructor - don't use this, use vgui_roi_tableau_new.
  vgui_roi_tableau();

  //: Constructor - don't use this, use vgui_roi_tableau_new.
  vgui_roi_tableau(vil_image const &, char const *, float, float, float,
                   float);
  
  //: Returns the type of this tableau ('vgui_roi_tableau').
  vcl_string type_name() const;

  //: Returns the filename of the image (if it knows it).
  vcl_string file_name() const;

  //: Returns a nice version of the name, including the filename.
  vcl_string pretty_name() const;

  //: Returns the image cropped by the ROI.
  vil_image get_image() const;

  //: Make the given image the underlying image for this tableau.
  void set_image(vil_image const &);

  //: Make image loaded from the given filename the image for this tableau.
  void set_image(char const *);          // <- convenience

  //: Width of image (0 if none).
  unsigned width() const;

  //: Height of image (0 if none).
  unsigned height() const;

  //: Returns the bounding box of the ROI.
  bool get_bounding_box(float low[3], float high[3]) const;

 protected:
  //: Destructor - called by vgui_roi_tableau_sptr.
  ~vgui_roi_tableau();

  //: Handle all events passed to this tableau.
  bool handle(vgui_event const &e);

 private:
  ROI roi_;
  vil_image cropped_image_;
  vcl_string name_;
};

//: Creates a smart-pointer to a vgui_roi_tableau.
struct vgui_roi_tableau_new : public vgui_roi_tableau_sptr
{
  typedef vgui_roi_tableau_sptr base;

  //: Constructor - creates a vgui_roi_tableau.
  vgui_roi_tableau_new() : base(new vgui_roi_tableau()) { }
};

#endif // vgui_roi_tableau_h_
