// This is oxl/mvl/HomgNorm2D.cxx
//:
// \file

#include <iostream>
#include "HomgNorm2D.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h> // for matrix * vgl_homg_point_2d

// ctor
HomgNorm2D::HomgNorm2D(std::vector<vgl_homg_point_2d<double> > const& points, bool unit_omega):
  normalized_(points.size()),
  unit_omega_(unit_omega)
{
  normalize(points);
}

HomgNorm2D::HomgNorm2D(const std::vector<HomgPoint2D>& points, bool unit_omega):
  normalized_(points.size()),
  unit_omega_(unit_omega)
{
  normalize(points);
}

// Destructor
HomgNorm2D::~HomgNorm2D() = default;

static bool paranoid = true;
static void centre(const std::vector<HomgPoint2D>& in, std::vector<HomgPoint2D>& out, double* cx_out, double* cy_out);
static void centre(std::vector<vgl_homg_point_2d<double> > const& in, std::vector<HomgPoint2D>& out, double&, double&);
static double scale_xyroot2(const std::vector<HomgPoint2D>& in, std::vector<HomgPoint2D>& out);

//: Put the cog at the origin.
// Scale x,y so that mean (x^2 + y^2) = 2 when z = 1.

void HomgNorm2D::normalize(std::vector<vgl_homg_point_2d<double> > const& points)
{
  // from ho_trivechomg_normalise
  normalized_.resize(points.size());

  double cx, cy;
  centre(points, normalized_, cx, cy);

  double diameter = scale_xyroot2(normalized_, normalized_);
  if (diameter == 0) {
    std::cerr << "HomgNorm2D: All points coincident\n";
    diameter = 1;
    was_coincident_=true;
  }
  else
    was_coincident_=false;

  SimilarityMetric::set_center_and_scale(cx, cy, 1/diameter);

  if (paranoid) {
    SimilarityMetric::scale_matrices(1/diameter);
    for (unsigned i = 0; i < points.size(); ++i) {
      vnl_double_3 ni = normalized_[i].get_vector();
      vgl_homg_point_2d<double> mi = SimilarityMetric::imagehomg_to_homg(points[i]);
      ni[0] -= mi.x(); ni[1] -= mi.y(); ni[2] -= mi.w();
      double l = ni.magnitude();
      if (l > 1e-12) {
        std::cerr << "HomgNorm2D: "
                 << "d = " << diameter
                 << ", ni = " << normalized_[i].get_vector()
                 << ", mi = " << mi
                 << ", Residual = " << ni
                 << " (mag = " << l << ")\n";
      }
    }
  }

  if (unit_omega_) {
    /* homogeneous scale factor is normally undetermined - set it here though. */
    for (auto & p : normalized_) {
      double x = p.x();
      double y = p.y();
      double w = p.w();

      if (w == 0) {
        /* average magnitude of finite points is root(3) after this stage - do the same for pts at infinity. */

        double scaling = std::sqrt(3.0) / vnl_math::hypot(x, y);
        x *= scaling;
        y *= scaling;
      }
      else {
        x /= w;
        y /= w;
        w = 1.0;
      }
      p.set(x,y,w);
    }
  }
}

void HomgNorm2D::normalize(const std::vector<HomgPoint2D>& points)
{
  // from ho_trivechomg_normalise
  normalized_.resize(points.size());

#if 0
  for (unsigned i = 0; i < points.size(); ++i)
    std::cerr << points[i].get_vector() << '\n';
#endif

  double cx, cy;
  centre(points, normalized_, &cx, &cy);

  double diameter = scale_xyroot2(normalized_, normalized_);
  if (diameter == 0) {
    std::cerr << "HomgNorm2D: All points coincident\n";
    diameter = 1;
    was_coincident_=true; // FSM
  }
  else
    was_coincident_=false;

  SimilarityMetric::set_center_and_scale(cx, cy, 1/diameter);

  //std::cerr << "NORM = " << norm_matrix_ << '\n';
  if (paranoid) {
    SimilarityMetric::scale_matrices(1/diameter);
    for (unsigned i = 0; i < points.size(); ++i) {
      vnl_double_3 ni = normalized_[i].get_vector();
      vnl_double_3 mi = SimilarityMetric::imagehomg_to_homg(points[i]).get_vector();
      vnl_double_3 residual = ni - mi;
      double l = residual.magnitude();
      if (l > 1e-12) {
     // std::cerr << "\n\n";
        std::cerr << "HomgNorm2D: "
                 << "d = " << diameter
                 << "ni = " << ni
                 << "mi = " << mi
                 << "Residual = " << residual
                 << " mag = " << l << std::endl;
      }
    }
  }

  if (unit_omega_) {
    /* homogeneous scale factor is normally undetermined - set it here though. */
    for (auto & p : normalized_) {
      double x = p.x();
      double y = p.y();
      double w = p.w();

      if (w == 0) {
        /* average magnitude of finite points is root(3) after this stage - do the same for pts at infinity. */

        double scaling = std::sqrt(3.0) / vnl_math::hypot(x, y);
        x *= scaling;
        y *= scaling;
      }
      else {
        x /= w;
        y /= w;
        w = 1.0;
      }
      p.set(x,y,w);
    }
  }
}

static void centre(const std::vector<HomgPoint2D>& in,
                   std::vector<HomgPoint2D>& out,
                   double *cx, double *cy)
{
  // Compute center of mass
  double cog_x = 0;
  double cog_y = 0;
  unsigned cog_count = 0;
  unsigned n = in.size();
  for (unsigned i = 0; i < n; ++i) {
    const HomgPoint2D& p = in[i];
    double x,y;
    if (p.get_nonhomogeneous(x, y)) {
      cog_x += x;
      cog_y += y;
      ++cog_count;
    }
  }
  if (cog_count > 0) {
    cog_x /= (double) cog_count;
    cog_y /= (double) cog_count;
  }

  // Remember cog for caller
  *cx = cog_x;
  *cy = cog_y;

  // Build conditioning matrix
  vnl_double_3x3 C;
  C.set_identity();
  C(0,2) = -cog_x;
  C(1,2) = -cog_y;

  // Transform points
  {
    for (unsigned i = 0; i < n; ++i)
      out[i].set(C * in[i].get_vector());
  }

  // Check transformation has worked.
  if (paranoid) {
    cog_x = 0;
    cog_y = 0;
    for (unsigned i = 0; i < n; ++i) {
      const HomgPoint2D& p = out[i];
      double x,y;
      if (p.get_nonhomogeneous(x, y)) {
        cog_x += x;
        cog_y += y;
      }
    }
    if (vnl_math::hypot(cog_x, cog_y) > 1e-10)
      std::cerr << "HomgNorm2D: expected (0,0) computed (" << cog_x << ',' << cog_y << ")\n";
  }
}

static void centre(std::vector<vgl_homg_point_2d<double> > const& in,
                   std::vector<HomgPoint2D>& out,
                   double& cx, double& cy)
{
  // Compute center of mass
  double cog_x = 0;
  double cog_y = 0;
  unsigned cog_count = 0;
  unsigned n = in.size();
  for (unsigned i = 0; i < n; ++i) {
    vgl_homg_point_2d<double> const& p = in[i];
    if (p.w() != 0.0) {
      cog_x += p.x()/p.w();
      cog_y += p.y()/p.w();
      ++cog_count;
    }
  }
  if (cog_count > 0) {
    cog_x /= (double) cog_count;
    cog_y /= (double) cog_count;
  }

  // Remember cog for caller
  cx = cog_x;
  cy = cog_y;

  // Build conditioning matrix
  vnl_double_3x3 C;
  C.set_identity();
  C(0,2) = -cog_x;
  C(1,2) = -cog_y;

  // Transform points
  for (unsigned i = 0; i < n; ++i)
  {
    vgl_homg_point_2d<double> q = C * in[i];
    out[i].set(q.x(),q.y(),q.w());
  }

  // Check transformation has worked.
  if (paranoid) {
    cog_x = 0;
    cog_y = 0;
    for (unsigned i = 0; i < n; ++i) {
      const HomgPoint2D& p = out[i];
      double x,y;
      if (p.get_nonhomogeneous(x, y)) {
        cog_x += x;
        cog_y += y;
      }
    }
    if (vnl_math::hypot(cog_x, cog_y) > 1e-10)
      std::cerr << "HomgNorm2D: expected (0,0) computed (" << cog_x << ',' << cog_y << ")\n";
  }
}

// - Scale the x and y components so that the average bivec magnitude is std::sqrt(2).
// Scale x,y so that mean (x^2 + y^2) = 2 when z = 1.

static double scale_xyroot2(const std::vector<HomgPoint2D>& in,
                            std::vector<HomgPoint2D>& out)
{
  double magnitude = 0;
  unsigned numfinite = 0;
  for (const auto & p : in) {
    double x,y;
    if (p.get_nonhomogeneous(x, y)) {
      magnitude += vnl_math::hypot(x, y);
      ++numfinite;
    }
  }

  // Compute diameter
  double diameter = 1;
  if (numfinite > 0) {
    magnitude /= (double) numfinite;
    diameter = magnitude / std::sqrt(2.0);
  }

  // Scale the points
  {
    for (unsigned i = 0; i < in.size(); ++i)
      out[i].set(in[i].x(), in[i].y(), in[i].w() * diameter);
  }

  if (paranoid) {
    magnitude = 0;
    numfinite = 0;
    for (unsigned i = 0; i < in.size(); ++i) {
      const HomgPoint2D& p = out[i];
      double x,y;
      if (p.get_nonhomogeneous(x, y)) {
        magnitude += vnl_math::hypot(x, y);
        ++numfinite;
      }
    }
    if (numfinite > 0) magnitude /= numfinite;
    const double expected = std::sqrt(2.0);
    if (vnl_math::abs(expected - magnitude) > 1e-13) // 1e-14 gave false alarm -- fsm
      std::cerr << "HomgNorm2D: Expected magnitude " << expected << " computed magnitude " << magnitude << '\n';
  }

  // Return
  return diameter;
}
