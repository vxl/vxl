#ifndef vgui_test_h_
#define vgui_test_h_
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vgui_test_sptr.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_text_tableau.h>

class vgui_test_thingy2d : public vgui_tableau {
public:
  vgui_test_thingy2d();
  bool handle(vgui_event const &);
  vcl_string type_name() const { return "vgui_test_thingy2d"; }
protected:
  ~vgui_test_thingy2d() { }
};

struct vgui_test_thingy2d_new : public vgui_test_thingy2d_sptr {
  typedef vgui_test_thingy2d_sptr base;
  vgui_test_thingy2d_new() : base(new vgui_test_thingy2d()) { }
};


class vgui_test_thingy3d : public vgui_tableau {
public:
  vgui_test_thingy3d();
  bool handle(vgui_event const &);
  vcl_string type_name() const { return "vgui_test_thingy3d"; }
protected:
  ~vgui_test_thingy3d() { }
};

struct vgui_test_thingy3d_new : public vgui_test_thingy3d_sptr {
  typedef vgui_test_thingy3d_sptr base;
  vgui_test_thingy3d_new() : base(new vgui_test_thingy3d()) { }
};


class vgui_test_credits : public vgui_text_tableau {
public:
  vgui_test_credits();
  bool handle(vgui_event const &);
  vcl_string type_name() const { return "vgui_test_credits"; }
protected:
  ~vgui_test_credits() { }
};

struct vgui_test_credits_new : public vgui_test_credits_sptr {
  typedef vgui_test_credits_sptr base;
  vgui_test_credits_new() : base(new vgui_test_credits()) { }
};

#endif // vgui_test_h_
