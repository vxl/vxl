// This is ./oxl/vgui/vgui_image_blender.h
#ifndef vgui_image_blender_h_
#define vgui_image_blender_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   27 Oct 99
// \brief  Tableau to blend two images.
//
//  Contains classes:  vgui_image_blender

#include <vgui/vgui_tableau.h>
class vgui_image_renderer;
class vil_image;

class vgui_image_blender : public vgui_tableau {
public:
  vgui_image_blender(char const* file, float a=1.0);
  vgui_image_blender(vil_image const& img, float a=1.0);

  bool handle(vgui_event const &e);
  vcl_string file_name() const;
  vcl_string type_name() const;

  void reread_image();

  vgui_image_renderer *renderer;
  float alpha;

protected:
 ~vgui_image_blender();
};

#endif // vgui_image_blender_h_
