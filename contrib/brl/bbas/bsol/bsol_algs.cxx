//:
// \file
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_line_2d.h>
#include <bsol/bsol_algs.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vgl/vgl_box_2d.h>



//: Destructor
bsol_algs::~bsol_algs()
{
}

//-----------------------------------------------------------------------------
// :Compute a bounding box for a set of vsol_line_2ds. 
//-----------------------------------------------------------------------------
vbl_bounding_box<double,2>  bsol_algs::
bounding_box(vcl_vector<vsol_line_2d_sptr> const & lines)
{
  vbl_bounding_box<double, 2> b;
  for (vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
       lit != lines.end(); lit++)
    {
      vsol_point_2d_sptr p0 = (*lit)->p0();
      vsol_point_2d_sptr p1 = (*lit)->p1();
      b.update(p0->x(), p0->y());
      b.update(p1->x(), p1->y());
    }
 return b;
}
//:returns true if the a and b intersect
bool bsol_algs::meet(vsol_box_2d_sptr const & a, vsol_box_2d_sptr const & b)
{
  double min_x_a = a->get_min_x(), max_x_a = a->get_max_x();
  double min_y_a = a->get_min_y(), max_y_a = a->get_max_y();
  double min_x_b = b->get_min_x(), max_x_b = b->get_max_x();
  double min_y_b = b->get_min_y(), max_y_b = b->get_max_y();
  if(min_x_b>max_x_a||min_x_a>max_x_b)
    return false;
  if(min_y_b>max_y_a||min_y_a>max_y_b)
    return false;
  return true;
}

//:find the intersection of two boxes. Return false if no intersection
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
  vgl_box_2d<double> temp = intersect(vga, vgb);
  if(temp.is_empty())
    return false;
  a_int_b = new vsol_box_2d();
  a_int_b->add_point(temp.min_x(), temp.min_y());
  a_int_b->add_point(temp.max_x(), temp.max_y());
  return true;
}

vsol_polygon_2d_sptr bsol_algs::poly_from_box(vsol_box_2d_sptr const& box)
{
  vcl_vector<vsol_point_2d_sptr> pts;
  vsol_point_2d_sptr pa = new vsol_point_2d(box->get_min_x(), box->get_min_y());
  vsol_point_2d_sptr pb = new vsol_point_2d(box->get_max_x(), box->get_min_y());
  vsol_point_2d_sptr pc = new vsol_point_2d(box->get_max_x(), box->get_max_y());
  vsol_point_2d_sptr pd = new vsol_point_2d(box->get_min_x(), box->get_max_y());
  pts.push_back(pa);   pts.push_back(pb);
  pts.push_back(pc);   pts.push_back(pd);
  return new vsol_polygon_2d(pts); 
}

//: Transform a vsol_polygon_2d with a general homography.  Return false
// if any of the points are turned into ideal points since vsol geometry
// is assumed finite.
bool bsol_algs::homography(vsol_polygon_2d_sptr const& p,
                           vgl_h_matrix_2d<double> const& H,
                           vsol_polygon_2d_sptr& Hp)
{
  int n = p->size();
  vcl_vector<vsol_point_2d_sptr> pts;
  double tol = 1e-06;
  for(int i = 0; i<n; i++)
    {
      vsol_point_2d_sptr v = p->vertex(i);
      vgl_homg_point_2d<double> hp(v->x(), v->y());
      vgl_homg_point_2d<double> Hhp = H(hp);
      if(Hhp.ideal(tol))
        return false;
      vgl_point_2d<double> q(Hhp);
      vsol_point_2d_sptr qs = new vsol_point_2d(q.x(), q.y());
      pts.push_back(qs);
    }
  Hp = new vsol_polygon_2d(pts);
  return true;
}
      
