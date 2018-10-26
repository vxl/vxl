// This is brl/bbas/imesh/algo/imesh_imls_surface.h
#ifndef imesh_imls_surface_h_
#define imesh_imls_surface_h_
//:
// \file
// \brief Interpolating Surface functions using IMLS
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date June 4, 2008
//
// This code is based on the paper:
// C. Shen, J. O'Brien, J. Shewchuk
// "Interpolating and Approximating Implicit Surfaces from Polygon Soup"
// SIGGRAPH 2004
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <imesh/imesh_mesh.h>
#include <imesh/algo/imesh_kd_tree.h>
#include <vnl/vnl_double_3x3.h>


class imesh_imls_surface
{
 public:
  //: Constructor
  imesh_imls_surface(const imesh_mesh& mesh,
                     double eps = 0.01, double lambda = 0.1,
                     bool enforce_bounded = false,
                     const std::set<unsigned int>& no_normal_faces = std::set<unsigned int>());

  //: Copy Constructor
  imesh_imls_surface(const imesh_imls_surface& other);

  //: evaluate the implicit surface at a point
  double operator() (const vgl_point_3d<double>& p) const;

  //: evaluate the implicit surface at a point
  double operator() (double x, double y, double z) const
  {
    return (*this)(vgl_point_3d<double>(x,y,z));
  }

  //: evaluate the squared weighting function given a squared distance
  double w2(double dist2) const
  {
    double w = 1.0 / (dist2 + eps2_);
    return w*w;
  }

  //: return a bounding box for the original input mesh
  vgl_box_3d<double> bounding_box() const;

  //: evaluate the function and its derivative (returned by reference)
  double deriv(const vgl_point_3d<double>& p, vgl_vector_3d<double>& dp) const;

  //: evaluate the function and its first and second derivatives (returned by reference)
  double deriv2(const vgl_point_3d<double>& p, vgl_vector_3d<double>& dp,
                vnl_double_3x3& ddp) const;

  //: change the epsilon (smoothness) of the surface
  void set_epsilon(double eps);

  //: change the lambda (accuracy parameter)
  void set_lambda(double lambda) { lambda_ = lambda; }

  //: a data structure to hold the integral terms
  struct integral_data
  {
    integral_data()
      : I(0), I_phi(0), dI(0,0,0), dI_phi(0,0,0) {}
    integral_data(double i, double i_phi,
                  const vgl_vector_3d<double>& di,
                  const vgl_vector_3d<double>& di_phi)
      : I(i), I_phi(i_phi), dI(di), dI_phi(di_phi) {}
    integral_data operator+ (const integral_data& other) const
    {
      return integral_data(I+other.I, I_phi+other.I_phi,
                           dI+other.dI, dI_phi+other.dI_phi);
    }
    integral_data& operator+= (const integral_data& other)
    {
      I += other.I;   I_phi += other.I_phi;
      dI += other.dI; dI_phi += other.dI_phi;
      return *this;
    }
    integral_data& operator*= (const double& s)
    {
      I *= s;   I_phi *= s;
      dI *= s; dI_phi *= s;
      return *this;
    }

    double I, I_phi;
    vgl_vector_3d<double> dI, dI_phi;
  };

  //: integrals of f(x)dx and x*f(x)dx over [0,1] where f(x)= 1/((x+k1)^2 + k2)^2
  static void line_integrals(double k1, double k2, double& I1, double& Ix);

  //: integrals of f(x)dx and x*f(x)dx over [0,1] where f(x)= 1/((x+k1)^2 + k2)^2
  //  Also compute the integrals when f(x)=1/((x+k1)^2 + k2)^3 (for use in derivatives)
  static void line_integrals(double k1, double k2,
                             double& I1, double& Ix,
                             double& dI1, double& dIx, double& dIx2);

  //: line integral of the squared weight function times a linear value on the line from p0 to p1
  //  (value at p0 is v0 and at p1 is v1)
  //  \a eps2 is epsilon^2
  static double line_integral(const vgl_point_3d<double>& x,
                              const vgl_point_3d<double>& p0,
                              const vgl_point_3d<double>& p1,
                              double v0, double v1, double eps);

  //: The derivative of the line integral with respect to x
  static vgl_vector_3d<double>
  line_integral_deriv(const vgl_point_3d<double>& x,
                      const vgl_point_3d<double>& p0,
                      const vgl_point_3d<double>& p1,
                      double v0, double v1, double eps2);

  //: area integral of the squared weight function times a linearly interpolated value
  //  \a m is the point closest point on the triangle to sample point \a x
  //  \a pp is second closest vertex and \a pm is the furthest
  //  call triangle_quadrature to first split an arbitrary triangle
  //  \a eps2 is epsilon^2
  static vgl_vector_2d<double>
  split_triangle_quadrature(const vgl_point_3d<double>& x,
                            const vgl_point_3d<double>& m,
                            const vgl_point_3d<double>& pp,
                            const vgl_point_3d<double>& pm,
                            double v0, double v1, double v2,
                            double eps);

  //: area integral of the squared weight function times a linearly interpolated value
  //  \a eps2 is epsilon^2
  template <class T, class F>
  static T
  triangle_quadrature(F quad_func,
                      const vgl_point_3d<double>& x,
                      const vgl_point_3d<double>& p0,
                      const vgl_point_3d<double>& p1,
                      const vgl_point_3d<double>& p2,
                      const vgl_vector_3d<double>& n,
                      double v0, double v1, double v2,
                      double eps);

  //: area integral of the squared weight function times a linearly interpolated value
  //  Also computes vector term used in the derivative
  //  \a m is the point closest point on the triangle to sample point \a x
  //  \a pp is second closest vertex and \a pm is the furthest
  //  call triangle_quadrature to first split an arbitrary triangle
  //  \a eps2 is epsilon^2
  static integral_data
  split_triangle_quadrature_with_deriv(const vgl_point_3d<double>& x,
                                       const vgl_point_3d<double>& m,
                                       const vgl_point_3d<double>& pp,
                                       const vgl_point_3d<double>& pm,
                                       double v0, double v1, double v2,
                                       double eps);

 private:

  //: recursively compute the area weighted centroids
  void compute_centroids_rec(const std::unique_ptr<imesh_kd_tree_node>& node,
                             const std::set<unsigned int>& no_normal_faces);

  //: recursively compute the unweighted integrals
  void compute_unweighed_rec(const std::unique_ptr<imesh_kd_tree_node>& node);


  //: compute the iso value such that the mean value at the vertices is zero
  void compute_iso_level();

  //: adjust the phi values until all vertices are within the iso surface
  //  Also computes the iso level
  void compute_enclosing_phi();

  std::vector<vgl_point_3d<double> > verts_;     // mesh vertices
  std::unique_ptr<imesh_regular_face_array<3> > triangles_; // mesh triangles

  std::unique_ptr<imesh_kd_tree_node> kd_tree_;    // root node of a kd-tree on triangles
  std::vector<double> phi_;                      // phi values assigned per vertex
  std::vector<double> area_;                     // surface area per node
  std::vector<double> unweighted_;               // unweighted integrals of phi over the surface per node
  std::vector<vgl_point_3d<double> > centroid_;  // area weighted centroid per node
  std::vector<vgl_vector_3d<double> > normals_;  // area weighted normal per node
  std::vector<double> normal_len_;               // cached magnitude of above normals

  double eps2_;
  double lambda_;
  double iso_level_;
  bool bounded_;
};


//: find the zero crossing point by bisection between positive point \a pp and negative point \a pn
//  Stops searching when $||p_p-p_n|| < x_{eps}$ or $|f(p_m)| < f_{eps}$
vgl_point_3d<double> bisect(const imesh_imls_surface& f,
                            vgl_point_3d<double> pp, vgl_point_3d<double> pn,
                            double feps = 1e-8,  double xeps = 1e-16);


//: Move the point \a p along the gradient direction until reaching a zero crossing of \a f (within \a eps).
//  Return true if successful
bool snap_to_surface(const imesh_imls_surface& f,
                     vgl_point_3d<double>& p,
                     double step = 0.5, double eps = 1e-5);


//: Move the point \a p to minimize (f^2 + (n*f' - 1)^2)/f'*f' a zero crossing of \a f (within \a eps).
//  Return true if successful
bool snap_to_surface_with_normal(const imesh_imls_surface& f,
                                 vgl_point_3d<double>& p,
                                 vgl_vector_3d<double> n,
                                 double step = 0.5, double eps = 1e-8);


//: Move the point \a p along direction \a dir until reaching a zero crossing of \a f (within \a eps).
//  Return true if successful
bool snap_to_surface(const imesh_imls_surface& f,
                     vgl_vector_3d<double> dir,
                     vgl_point_3d<double>& p,
                     double step = 0.5, double eps = 1e-5);


#endif // imesh_imls_surface_h_
