#ifndef vgui_test_h_
#define vgui_test_h_
/*
  fsm@robots.ox.ac.uk
*/
// .NAME vgui_test
// .INCLUDE vgui/vgui_test.h
// .FILE vgui_test.cxx

#include "vgui_test_ref.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_text.h>

class vgui_test_thingy2d : public vgui_tableau {
public:
  vgui_test_thingy2d();
  bool handle(vgui_event const &);
  vcl_string type_name() const { return "vgui_test::thingy2d"; }
protected:
  ~vgui_test_thingy2d() { }
};

struct vgui_test_thingy2d_new : public vgui_test_thingy2d_ref {
  typedef vgui_test_thingy2d_ref base;
  vgui_test_thingy2d_new() : base(new vgui_test_thingy2d()) { }
};



class vgui_test_thingy3d : public vgui_tableau {
public:
  vgui_test_thingy3d();
  bool handle(vgui_event const &);
  vcl_string type_name() const { return "vgui_test::thingy3d"; }
protected:
  ~vgui_test_thingy3d() { }
};

struct vgui_test_thingy3d_new : public vgui_test_thingy3d_ref {
  typedef vgui_test_thingy3d_ref base;
  vgui_test_thingy3d_new() : base(new vgui_test_thingy3d()) { }
};



class vgui_test_credits : public vgui_text {
public:
  vgui_test_credits();
  bool handle(vgui_event const &);
  vcl_string type_name() const { return "vgui_test::credits"; }
protected:
  ~vgui_test_credits() { }
};

struct vgui_test_credits_new : public vgui_test_credits_ref {
  typedef vgui_test_credits_ref base;
  vgui_test_credits_new() : base(new vgui_test_credits()) { }
};

#endif // vgui_test_h_
