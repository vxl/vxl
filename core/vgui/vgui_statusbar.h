// This is core/vgui/vgui_statusbar.h
#ifndef vgui_statusbar_h_
#define vgui_statusbar_h_
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   21 Nov 99
// \brief  contains class vgui_statusbar
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

class vgui_statusbar
{
 public:
  vgui_statusbar() {}
  virtual ~vgui_statusbar() {}

  virtual int write(const char* text, int n) = 0;
  virtual int write(const char* text) = 0;
};

#endif // vgui_statusbar_h_
