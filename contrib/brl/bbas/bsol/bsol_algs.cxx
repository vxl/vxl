//:
// \file
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <bsol/bsol_algs.h>


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
  
