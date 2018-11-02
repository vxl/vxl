#include "bsol_algs.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_convex_hull_2d.h>

// Destructor
bsol_algs::~bsol_algs()
= default;

//-----------------------------------------------------------------------------
//: Compute a bounding box for a set of vsol_point_2ds.
//-----------------------------------------------------------------------------
vbl_bounding_box<double,2> bsol_algs::
bounding_box(std::vector<vsol_point_2d_sptr> const& points)
{
  vbl_bounding_box<double, 2> b;
  for (const auto & point : points)
    b.update(point->x(), point->y());
  return b;
}

//-----------------------------------------------------------------------------
//: Compute a bounding box for a set of vsol_line_2ds.
//-----------------------------------------------------------------------------
vbl_bounding_box<double,2>  bsol_algs::
bounding_box(std::vector<vsol_line_2d_sptr> const & lines)
{
  vbl_bounding_box<double, 2> b;
  for (const auto & line : lines)
  {
    vsol_point_2d_sptr p0 = line->p0();
    vsol_point_2d_sptr p1 = line->p1();
    b.update(p0->x(), p0->y());
    b.update(p1->x(), p1->y());
  }
  return b;
}

//-----------------------------------------------------------------------------
//: Compute a bounding box for a set of vsol_point_3ds.
//-----------------------------------------------------------------------------
vbl_bounding_box<double,3> bsol_algs::
bounding_box(std::vector<vsol_point_3d_sptr> const& points)
{
  vbl_bounding_box<double, 3> b;
  for (const auto & point : points)
    b.update(point->x(), point->y(), point->z());
  return b;
}

//-----------------------------------------------------------------------------
//: Determine if a point is inside a bounding box
//-----------------------------------------------------------------------------
bool bsol_algs::in(vsol_box_2d_sptr const & b, double x, double y)
{
  if (!b)
    return false;
  double xmin = b->get_min_x(), ymin = b->get_min_y();
  double xmax = b->get_max_x(), ymax = b->get_max_y();
  return x >= xmin && x <= xmax
      && y >= ymin && y <= ymax;
}

//: returns true if the boxes a and b intersect
bool bsol_algs::meet(vsol_box_2d_sptr const & a, vsol_box_2d_sptr const & b)
{
  double min_x_a = a->get_min_x(), max_x_a = a->get_max_x();
  double min_y_a = a->get_min_y(), max_y_a = a->get_max_y();
  double min_x_b = b->get_min_x(), max_x_b = b->get_max_x();
  double min_y_b = b->get_min_y(), max_y_b = b->get_max_y();
  return min_x_b <= max_x_a && min_x_a <= max_x_b
      && min_y_b <= max_y_a && min_y_a <= max_y_b;
}

//: find the intersection of two boxes. Return false if no intersection
bool bsol_algs::intersection(vsol_box_2d_sptr const & a,
                             vsol_box_2d_sptr const & b,
                             vsol_box_2d_sptr& a_int_b)
{
  vgl_point_2d<double> a_min(a->get_min_x(), a->get_min_y());
  vgl_point_2d<double> a_max(a->get_max_x(), a->get_max_y());
  vgl_box_2d<double> vga(a_min, a_max);

  vgl_point_2d<double> b_min(b->get_min_x(), b->get_min_y());
  vgl_point_2d<double> b_max(b->get_max_x(), b->get_max_y());
  vgl_box_2d<double> vgb(b_min, b_max);
  vgl_box_2d<double> temp = vgl_intersection(vga, vgb);
  if (temp.is_empty())
    return false;
  a_int_b = new vsol_box_2d();
  a_int_b->add_point(temp.min_x(), temp.min_y());
  a_int_b->add_point(temp.max_x(), temp.max_y());
  return true;
}

//: find the convex union of two boxes. Always return true
bool bsol_algs::box_union(vsol_box_2d_sptr const & a,
                          vsol_box_2d_sptr const & b,
                          vsol_box_2d_sptr& a_union_b)
{
  if (!a||!b)
    return false;
  double x_min_a = a->get_min_x(), y_min_a = a->get_min_y();
  double x_max_a = a->get_max_x(), y_max_a = a->get_max_y();
  double x_min_b = b->get_min_x(), y_min_b = b->get_min_y();
  double x_max_b = b->get_max_x(), y_max_b = b->get_max_y();
  double x_min=x_min_a, y_min = y_min_a;
  double x_max=x_max_a, y_max = y_max_a;
  if (x_min_b<x_min)
    x_min = x_min_b;
  if (y_min_b<y_min)
    y_min = y_min_b;
  if (x_max_b>x_max)
    x_max = x_max_b;
  if (y_max_b>y_max)
    y_max = y_max_b;
  a_union_b = new vsol_box_2d();
  a_union_b->add_point(x_min, y_min);
  a_union_b->add_point(x_max, y_max);
  return true;
}

//-----------------------------------------------------------------------------
//: expand or contract a box with the supplied absolute margin
//-----------------------------------------------------------------------------
bool bsol_algs::box_with_margin(vsol_box_2d_sptr const & b,
                                const double margin,
                                vsol_box_2d_sptr& bmod)
{
  if (!b)
    return false;
  double x_min = b->get_min_x(), y_min = b->get_min_y();
  double x_max = b->get_max_x(), y_max = b->get_max_y();
  double width = x_max-x_min, height = y_max-y_min;
  //See if the margin for contraction is too large, i.e. margin is negative
  if ((width+2*margin)<0)
    return false;
  if ((height+2*margin)<0)
    return false;
  bmod = new vsol_box_2d();
  bmod->add_point(x_min-margin, y_min-margin);
  bmod->add_point(x_max+margin, y_max+margin);
  return true;
}

//-----------------------------------------------------------------------------
//: Compute the convex hull of a set of polygons
//-----------------------------------------------------------------------------
bool bsol_algs::hull_of_poly_set(std::vector<vsol_polygon_2d_sptr> const& polys,
                                 vsol_polygon_2d_sptr& hull)
{
  if (!polys.size())
    return false;
  std::vector<vgl_point_2d<double> > points;
  for (const auto & poly : polys)
  {
    if (!poly)
      return false;
    for (unsigned int i=0; i<poly->size(); ++i)
      points.emplace_back(poly->vertex(i)->x(),
                                            poly->vertex(i)->y());
  }
  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> h = ch.hull();
  hull = bsol_algs::poly_from_vgl(h);
  return true;
}

//-----------------------------------------------------------------------------
//: Determine if a point is inside a bounding box
//-----------------------------------------------------------------------------
bool bsol_algs::in(vsol_box_3d_sptr const & b,
                   double x, double y, double z)
{
  if (!b)
    return false;
  double xmin = b->get_min_x(), ymin = b->get_min_y(), zmin = b->get_min_z();
  double xmax = b->get_max_x(), ymax = b->get_max_y(), zmax = b->get_max_z();
  return x >= xmin && x <= xmax
      && y >= ymin && y <= ymax
      && z >= zmin && z <= zmax;
}

vsol_polygon_2d_sptr bsol_algs::poly_from_box(vsol_box_2d_sptr const& box)
{
  std::vector<vsol_point_2d_sptr> pts;
  vsol_point_2d_sptr pa = new vsol_point_2d(box->get_min_x(), box->get_min_y());
  vsol_point_2d_sptr pb = new vsol_point_2d(box->get_max_x(), box->get_min_y());
  vsol_point_2d_sptr pc = new vsol_point_2d(box->get_max_x(), box->get_max_y());
  vsol_point_2d_sptr pd = new vsol_point_2d(box->get_min_x(), box->get_max_y());
  pts.push_back(pa);   pts.push_back(pb);
  pts.push_back(pc);   pts.push_back(pd); //pts.push_back(pa);
  return new vsol_polygon_2d(pts);
}

//: construct a vsol_polygon from a vgl_polygon
vsol_polygon_2d_sptr bsol_algs::poly_from_vgl(vgl_polygon<double> const& poly)
{
  vsol_polygon_2d_sptr out;
  std::vector<vsol_point_2d_sptr> pts;
  if (poly.num_sheets() != 1)
    return out;
  std::vector<vgl_point_2d<double> > sheet = poly[0];
  for (auto & pit : sheet)
  {
    vsol_point_2d_sptr p = new vsol_point_2d(pit.x(), pit.y());
    pts.push_back(p);
  }
  out = new vsol_polygon_2d(pts);
  return out;
}

vgl_polygon<double>
bsol_algs::vgl_from_poly(vsol_polygon_2d_sptr const& poly)
{
  vgl_polygon<double> vp(1);
  if (!poly)
    return vp;
  unsigned nverts = poly->size();
  for (unsigned i = 0; i<nverts; ++i)
  {
    double x = poly->vertex(i)->x(), y = poly->vertex(i)->y();
    vp.push_back(x, y);
  }
  return vp;
}

//: find the closest point to p in a set of points
vsol_point_2d_sptr
bsol_algs::closest_point(vsol_point_2d_sptr const& p,
                         std::vector<vsol_point_2d_sptr> const& point_set,
                         double& d)
{
  vsol_point_2d_sptr cp;
  int n = point_set.size();
  if (!p||!n)
    return cp;
  double dmin_sq = vnl_numeric_traits<double>::maxval;
  double x = p->x(), y = p->y();
  for (const auto & pit : point_set)
  {
    double xs = pit->x(), ys = pit->y();
    double dsq = (x-xs)*(x-xs)+(y-ys)*(y-ys);
    if (dsq<dmin_sq)
    {
      dmin_sq = dsq;
      cp = pit;
    }
  }
  d = std::sqrt(dmin_sq);
  return cp;
}

vsol_point_3d_sptr
bsol_algs::closest_point(vsol_point_3d_sptr const& p,
                         std::vector<vsol_point_3d_sptr> const& point_set,
                         double& d)
{
  d = 0;
  vsol_point_3d_sptr cp;
  int n = point_set.size();
  if (!p||!n)
    return cp;
  double dmin_sq = vnl_numeric_traits<double>::maxval;
  double x = p->x(), y = p->y(), z = p->z();
  for (const auto & pit : point_set)
  {
    double xs = pit->x(), ys = pit->y(), zs = pit->z();
    double dsq = (x-xs)*(x-xs) + (y-ys)*(y-ys) + (z-zs)*(z-zs);
    if (dsq<dmin_sq)
    {
      dmin_sq = dsq;
      cp = pit;
    }
  }
  d = std::sqrt(dmin_sq);
  return cp;
}

//: Transform a vsol_polygon_2d with a general homography.
//  Return false if any of the points are turned into ideal points
//  since vsol geometry is assumed finite.
bool bsol_algs::homography(vsol_polygon_2d_sptr const& p,
                           vgl_h_matrix_2d<double> const& H,
                           vsol_polygon_2d_sptr& Hp)
{
  const int n = p->size();
  std::vector<vsol_point_2d_sptr> pts;
  const double tol = 1e-06;
  for (int i = 0; i<n; i++)
  {
    vsol_point_2d_sptr v = p->vertex(i);
    vgl_homg_point_2d<double> hp(v->x(), v->y());
    vgl_homg_point_2d<double> Hhp = H(hp);
    if (Hhp.ideal(tol))
      return false;
    vgl_point_2d<double> q(Hhp);
    vsol_point_2d_sptr qs = new vsol_point_2d(q.x(), q.y());
    pts.push_back(qs);
  }
  Hp = new vsol_polygon_2d(pts);
  return true;
}

//: Transform a vsol_polygon_2d with a point specified as the center of the transformation.
//  i.e. vertices of the polygon are translated so that the specified point is the origin.
/// The transformation is then applied and the point coordinates added back in afterwards.
vsol_polygon_2d_sptr bsol_algs::
transform_about_point(vsol_polygon_2d_sptr const& p,
                      vsol_point_2d_sptr const& c,
                      vgl_h_matrix_2d<double> const& H)
{
  const int n = p->size();
  std::vector<vsol_point_2d_sptr> pts;
  for (int i = 0; i<n; i++)
  {
    vsol_point_2d_sptr v = p->vertex(i);
    //Remove the centroid
    vgl_homg_point_2d<double> hp(v->x() - c->x(), v->y() - c->y());
    vgl_homg_point_2d<double> Hhp = H(hp);
    vgl_point_2d<double> q(Hhp);
    //add it back in
    vsol_point_2d_sptr qs = new vsol_point_2d(q.x() + c->x(), q.y() + c->y());
    pts.push_back(qs);
  }
  return new vsol_polygon_2d(pts);
}

//: Transform a vsol_polygon_2d with a homography
//  Apply the transform with the centroid of the polygon as the
//  origin and then translate by the centroid location vector
vsol_polygon_2d_sptr bsol_algs::
transform_about_centroid(vsol_polygon_2d_sptr const& p,
                         vgl_h_matrix_2d<double> const& H)
{
  vsol_point_2d_sptr c = p->centroid();
  return bsol_algs::transform_about_point(p, c, H);
}

bool bsol_algs::homography(vsol_box_2d_sptr const& b,
                           vgl_h_matrix_2d<double> const& H,
                           vsol_box_2d_sptr& Hb)
{
  vsol_polygon_2d_sptr p = bsol_algs::poly_from_box(b);
  vsol_polygon_2d_sptr Hp;
  if (!homography(p, H, Hp))
    return false;
  Hb = Hp->get_bounding_box();
  return true;
}

void bsol_algs::tangent(vsol_digital_curve_2d_sptr const& dc, unsigned index,
                        double& dx, double& dy)
{
  dx = 0; dy = 0;
  if (!dc)
    return;
  unsigned n = dc->size();
  //cases
  if (index>=n)
    return;
  if (index == 0)// first point on curve
  {
    vsol_point_2d_sptr p_n0 = dc->point(0);
    vsol_point_2d_sptr p_n1 = dc->point(1);
    dx = p_n1->x()-p_n0->x();
    dy = p_n1->y()-p_n0->y();
    return;
  }

  if (index == n-1)// last point on curve
  {
    vsol_point_2d_sptr p_n1 = dc->point(n-1);
    vsol_point_2d_sptr p_n2 = dc->point(n-2);
    dx = p_n1->x()-p_n2->x();
    dy = p_n1->y()-p_n2->y();
    return;
  }
  //the normal case
  vsol_point_2d_sptr p_m1 = dc->point(index-1);
  vsol_point_2d_sptr p_p1 = dc->point(index+1);
  dx = p_p1->x()-p_m1->x();
  dy = p_p1->y()-p_m1->y();
}

void bsol_algs::print(vsol_box_2d_sptr const& b)
{
  if (!b)
    return;
  std::cout << *b << '\n';
}

void bsol_algs::print(vsol_box_3d_sptr const& b)
{
  if (!b)
    return;
  double xmin = b->get_min_x(), ymin = b->get_min_y(), zmin = b->get_min_z();
  double xmax = b->get_max_x(), ymax = b->get_max_y(), zmax = b->get_max_z();

  std::cout << "vsol_box_2d[(" << xmin << ' ' << ymin << ' ' << zmin << ")<("
           << xmax << ' ' << ymax << ' ' << zmax << ")]\n";
}

void bsol_algs::print(vsol_point_2d_sptr const& p)
{
  if (!p)
    return;
  std::cout << *p << '\n';
}

void bsol_algs::print(vsol_point_3d_sptr const& p)
{
  if (!p)
    return;
  std::cout << "vsol_point_3d[ " << p->x() << ' ' << p->y() << ' '
           << p->z() <<  " ]\n";
}
