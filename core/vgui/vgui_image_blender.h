#ifndef vgui_image_blender_h_
#define vgui_image_blender_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_image_blender - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_image_blender.h
// .FILE vgui_image_blender.cxx
//
// .SECTION Description:
//
// vgui_image_blender is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author:
//              Philip C. Pritchett, 27 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

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
