#ifndef vgui_projection_inspector_h_
#define vgui_projection_inspector_h_
/*
  fsm@robots.ox.ac.uk
*/

// .NAME vgui_projection_inspector
// .INCLUDE vgui/vgui_projection_inspector.h
// .FILE vgui_projection_inspector.cxx

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vgui_projection_inspector {
public:
  vgui_projection_inspector();
  ~vgui_projection_inspector();

  //
  vnl_matrix<double> const &projection_matrix() const { return P; }
  vnl_matrix<double> const &modelview_matrix() const { return M; }
  int const *viewport() const { return vp; }

  void print(vcl_ostream&) const;
  vnl_matrix<double> total_transformation() const { return P*M; }

  // true iff the current total projection matrix has the form
  // s0       t0
  //    s1    t1
  //       s2 t2
  //          1
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

  // Back-projection of a given point onto a given plane p.
  // 1. from homogeneous viewport coordinates to homogeneous world coordinates :
  bool back_project(double const x[3], double const p[4], double X[4]) const;
  // 2. Returns a 3-vcl_vector :
  vnl_vector<double> back_project(double x,double y,/*z=1,*/ vnl_vector<double> const &p /*4*/) const;
  // 3. Returns a 4-vcl_vector :
  vnl_vector<double> back_project(double x,double y,double z,vnl_vector<double> const &p /*4*/) const;
  // 3. x can be a 2 or 3-vcl_vector. The returned vcl_vector has size 1+x.size();
  vnl_vector<double> back_project(vnl_vector<double> const &x,vnl_vector<double> const &p /*4*/) const;

private:
  int vp[4]; // viewport
  vnl_matrix<double> P; // projection matrix
  vnl_matrix<double> M; // modelview matrix
  void inspect();
};

#endif // vgui_projection_inspector_h_
