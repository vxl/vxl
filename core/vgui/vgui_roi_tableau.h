//-*- c++ -*-------------------------------------------------------------------
#ifndef vgui_roi_tableau_h_
#define vgui_roi_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_roi_tableau
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_roi_tableau.h
// .FILE vgui_roi_tableau.cxx
//
// .SECTION Description:
//
// vgui_roi_tableau is a class that Marko hasn't documented properly. FIXME
//
// .SECTION Author:
//              Marko Bacic, 18 Jul 2000
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_rubberbander.h>
#include <vil/vil_image.h>
class vil_crop_image_impl;
class vil_image;
class vgui_image_tableau;

#include "vgui_roi_tableau_ref.h"

class vgui_roi_tableau : public vgui_tableau {
 public:
  typedef struct region_of_interest {
    float sx;
    float sy;
    float width;
    float height;
  }ROI;
  vgui_roi_tableau();
  vgui_roi_tableau(vil_image const &, char const *, float, float, float,
		   float);

  ~vgui_roi_tableau();
  vcl_string type_name() const;
  vcl_string file_name() const;
  vcl_string pretty_name() const;
  
  // get
  vil_image get_image() const;
  
  // set
  void set_image(vil_image const &);
  void set_image(char const *);          // <- convenience

  // size of image (0 if none).
  unsigned width() const;
  unsigned height() const;

  bool get_bounding_box(float low[3], float high[3]) const;

 protected:
  bool handle(vgui_event const &e);

 private:
  ROI roi_;
  vil_image cropped_image_;
  vcl_string name_;
};

struct vgui_roi_tableau_new : public vgui_roi_tableau_ref {
  typedef vgui_roi_tableau_ref base;
  vgui_roi_tableau_new() : base(new vgui_roi_tableau()) { }
};

#endif // vgui_roi_tableau_h_
