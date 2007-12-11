#ifndef bwm_soview2D_vertex_h_
#define bwm_soview2D_vertex_h_
//:
// \file

#include <bgui/bgui_vsol_soview2D.h>

//: 2-dimensional object cross
class bwm_soview2D_vertex : public vgui_soview2D_circle
{
 public:
  //: Constructor - create a default soview2D.
   bwm_soview2D_vertex(float x, float y, float r, bgui_vsol_soview2D* obj, unsigned i)
     : vgui_soview2D_circle(x, y, r), obj_(obj), v_index_(i) {}

  //: Destructor - delete this soview2D.
  virtual ~bwm_soview2D_vertex() {}

  virtual vcl_string type_name() const { return "bwm_soview2D_vertex"; }

  bgui_vsol_soview2D* obj() { return obj_; }
  int vertex_indx() { return v_index_; }

 private:
  //: the id of the soview2D object that vertex belongs to
  bgui_vsol_soview2D* obj_;

  //: the index of the vertex in the 2d object's vertex list
  int v_index_;
};

#endif
