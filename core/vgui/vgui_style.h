#ifndef vgui_style_h_
#define vgui_style_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vgui_style - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_style.h
// .FILE vgui_style.cxx
//
// .SECTION Description
//
// vgui_style is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 18 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

class vgui_style {
public:
  vgui_style();
  vgui_style(const vgui_style& that) { *this = that; }
  ~vgui_style() { }

  float rgba[4];
  float line_width;
  float point_size;
};

class vgui_style_equal {
public:
  vgui_style_equal(vgui_style* s1_) : s1(s1_) {}

  bool operator() (vgui_style* s2) {
    if (s1->rgba[0] == s2->rgba[0] &&
        s1->rgba[1] == s2->rgba[1] &&
        s1->rgba[2] == s2->rgba[2] &&
        s1->point_size == s2->point_size &&
        s1->line_width == s2->line_width) {
      return true;
    }
    return false;
  }

  vgui_style *s1;
};

#endif // vgui_style_h_
