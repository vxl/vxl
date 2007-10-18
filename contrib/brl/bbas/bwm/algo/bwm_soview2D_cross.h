#include <vgui/vgui_soview2D.h>

//: 2-dimensional object cross
class bwm_soview2D_cross : public vgui_soview2D_point
{
 public:
  //: Constructor - create a default soview2D.
   bwm_soview2D_cross(float x, float y, float r) : vgui_soview2D_point(x,y), r_(r){}

  //: Destructor - delete this soview2D.
  virtual ~bwm_soview2D_cross() {}

  virtual vcl_string type_name() const { return "bwm_soview2D_cross"; }

  void draw() const;

private:
  // radius of the cross, the distance from the center to the one vertex
  float r_;
};
