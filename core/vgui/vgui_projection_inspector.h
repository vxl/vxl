// This is oxl/vgui/vgui_projection_inspector.h
#ifndef vgui_projection_inspector_h_
#define vgui_projection_inspector_h_
//: 
// \file
// \author fsm@robots.ox.ac.uk
// \brief
// 
//  Contains classes:  vgui_projection_inspector
//
// \verbatim
//  Modifications:
//    14-Aug-2002 K.Y.McGaul - Converted to Doxygen style comments.
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vgui_projection_inspector 
{
public:
  //: Constructor - with default projection and modelview matrices.
  vgui_projection_inspector();

  //: Destructor - currently does nothing.
  ~vgui_projection_inspector();

  //: Returns the projection matrix.
  vnl_matrix<double> const &projection_matrix() const { return P; }

  //: Returns the modelview matrix.
  vnl_matrix<double> const &modelview_matrix() const { return M; }

  //: Returns the viewport.
  int const *viewport() const { return vp; }

  //: Send info on this projection_inspector to the given stream.
  void print(vcl_ostream&) const;

  //: Returns projection matrix multiplied by modelview matrix.
  vnl_matrix<double> total_transformation() const { return P*M; }

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

  float x1,y1; // bottom left of viewport
  float x2,y2; // top right of viewport
  float s[3], t[3];

  // conversions
  void window_to_image_coordinates(int, int, float &,float &) const;
  void image_to_window_coordinates(float, float, float &,float &) const;
  bool image_viewport(float&, float&, float&, float&);
  bool compute_as_2d_affine(int width, int height,
                            float* offsetX, float* offsetY,
                            float* scaleX, float* scaleY);

  // some people prefer to put &s on "return value" parameters.
  void window_to_image_coordinates(int wx, int wy, float *ix,float *iy) const
  { window_to_image_coordinates(wx, wy, *ix, *iy); }
  void image_to_window_coordinates(float ix, float iy, float *wx,float *wy) const
  { image_to_window_coordinates(ix, iy, *wx, *wy); }

  //: Back-projection of a given point onto a given plane p.
  // From homogeneous viewport coordinates to homogeneous world coordinates.
  bool back_project(double const x[3], double const p[4], double X[4]) const;

  //: Back-projection of a given point onto a given plane p.
  //  Returns a 3-vcl_vector.
  vnl_vector<double> back_project(double x,double y,/*z=1,*/ vnl_vector<double> const &p /*4*/) const;

  //: Back-projection of a given point onto a given plane p.
  //  Returns a 4-vcl_vector.
  vnl_vector<double> back_project(double x,double y,double z,vnl_vector<double> const &p /*4*/) const;

  //: Back-projection of a given point onto a given plane p.
  //  x can be a 2 or 3-vcl_vector. The returned vcl_vector has size 1+x.size().
  vnl_vector<double> back_project(vnl_vector<double> const &x,vnl_vector<double> const &p /*4*/) const;

private:
  int vp[4]; // viewport
  vnl_matrix<double> P; // projection matrix
  vnl_matrix<double> M; // modelview matrix
  void inspect();
};

#endif // vgui_projection_inspector_h_
