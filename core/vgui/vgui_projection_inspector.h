// This is core/vgui/vgui_projection_inspector.h
#ifndef vgui_projection_inspector_h_
#define vgui_projection_inspector_h_
//:
// \file
// \brief contains class vgui_projection_inspector
// \author fsm
//
// \verbatim
//  Modifications
//   14-Aug-2002 K.Y.McGaul - Converted to Doxygen style comments.
// \endverbatim

#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_4x4.h>
#include <vcl_iosfwd.h>

//:
class vgui_projection_inspector
{
 public:
  //: Constructor - with default projection and modelview matrices.
  vgui_projection_inspector() { inspect(); }

  //: Destructor.
  ~vgui_projection_inspector() {}

  //: Returns the projection matrix.
  vnl_double_4x4 const& projection_matrix() const { return P; }

  //: Returns the modelview matrix.
  vnl_double_4x4 const& modelview_matrix() const { return M; }

  //: Returns the viewport.
  int const *viewport() const { return vp; }

  //: Send info on this projection_inspector to the given stream.
  void print(vcl_ostream&) const;

  //: Returns projection matrix multiplied by modelview matrix.
  vnl_double_4x4 total_transformation() const { return P*M; }

  //: Returns true if the projection matrix has a special form.
  // True iff the current total projection matrix has the form
  // \verbatim
  // s0       t0
  //    s1    t1
  //       s2 t2
  //          1
  // \endverbatim
  // in which case x1,y1, x2,y2 will contain the corners of the
  // backprojection of the viewport onto the plane z=0 in the
  // world and s,t will contain the nonzero entries.
  bool diagonal_scale_3d;

  // Bottom left of viewport - x coord.
  float x1;
  // Bottom left of viewport - y coord.
  float y1;
  // Top right of viewport - x coord.
  float x2;
  // Top right of viewport - y coord.
  float y2;

  float s[3], t[3];

  //: Convert window coords (eg. from vgui_event) to image coords.
  void window_to_image_coordinates(int, int, float &,float &) const;

  //: Convert image coords to window coords.
  void image_to_window_coordinates(float, float, float &,float &) const;

  //: Returns the corners of the backprojection of the viewport onto z=0.
  bool image_viewport(float& bottom_left_x, float& bottom_left_y,
                      float& top_right_x, float& top_right_y);

  //: Offset and scaling to transform window (x,y) to image (ix, iy) coords.
  //        ix = (x - token.offsetX) / token.scaleX;
  //        iy = (y - token.offsetY) / token.scaleY;
  bool compute_as_2d_affine(int width, int height,
                            float* offsetX, float* offsetY,
                            float* scaleX, float* scaleY);

  //: Convert window coords (eg. from vgui_event) to image coords.
  //  Some people prefer to put &s on "return value" parameters.
  void window_to_image_coordinates(int wx, int wy, float *ix,float *iy) const
  { window_to_image_coordinates(wx, wy, *ix, *iy); }

  //: Convert image coords to window coords.
  //  Some people prefer to put &s on "return value" parameters.
  void image_to_window_coordinates(float ix,float iy,float *wx,float *wy) const
  { image_to_window_coordinates(ix, iy, *wx, *wy); }

  //: Back-projection of a given point onto a given plane p.
  // From homogeneous viewport coordinates to homogeneous world coordinates.
  bool back_project(double const x[3], double const p[4], double X[4]) const;

  //: Back-projection of a given point onto a given plane p.
  //  Returns a 3-vcl_vector.
  vnl_vector<double> back_project(double x,double y, vnl_double_4 const &p) const;

  //: Back-projection of a given point onto a given plane p.
  //  Returns a 4-vcl_vector.
  vnl_vector<double> back_project(double x,double y,double z,vnl_double_4 const &p) const;

  //: Back-projection of a given point onto a given plane p.
  //  x can be a 2 or 3-vcl_vector. The returned vcl_vector has size 1+x.size().
  vnl_vector<double> back_project(vnl_vector<double> const &x,vnl_double_4 const &p) const;

 private:
  int vp[4]; // viewport
  vnl_double_4x4 P; // projection matrix
  vnl_double_4x4 M; // modelview matrix
  void inspect();
};

#endif // vgui_projection_inspector_h_
