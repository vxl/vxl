#include <math.h>
#include <vcl_iostream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bdgl/bdgl_curve_algs.h>
const double bdgl_curve_algs::tol = 1e-6;

//: Destructor
bdgl_curve_algs::~bdgl_curve_algs()
{
}
//:
//-----------------------------------------------------------------------------
// Finds the index on an edgel_chain closest to the given
// point (x, y). Later this routine can become a method on 
// vdgl_edgel_chain.
//-----------------------------------------------------------------------------
int bdgl_curve_algs::closest_point(vdgl_edgel_chain_sptr& ec,
                                   const double x, const double y)

{
  if(!ec)
    {
      vcl_cout << "In bdgl_curve_algs::closest_point(..) - warning, null chain"
               << vcl_endl;
      return 0;
    }
  //for now just scan the curve and save the closest point
  double mind = vnl_numeric_traits<double>::maxval;
  int N =ec->size(), imin = 0;

  for(int i = 0; i<N; i++)
    {
      vdgl_edgel ed = ec->edgel(i);
      double d = sqrt((ed.x()-x)*(ed.x()-x) + (ed.y()-y)*(ed.y()-y));
      if(d<mind)
        {
          mind = d;
          imin = i;
        }
   }
  return imin;
}
//: It is sometimes necessary to reverse the order of the digital curve
//  so that the inital point corresponds to v1 of a topology edge
vdgl_digital_curve_sptr bdgl_curve_algs::reverse(vdgl_digital_curve_sptr& dc)
{
  if(!dc)
    return 0;
  vdgl_interpolator_sptr intrp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = intrp->get_edgel_chain();
  int N = ec->size();
  vdgl_edgel_chain_sptr rev_ec = new vdgl_edgel_chain();
  for(int i = 0; i<N; i++)
    rev_ec->add_edgel((*ec)[N-1-i]);
  vdgl_interpolator_sptr rev_intrp = new vdgl_interpolator_linear(rev_ec);
  vdgl_digital_curve_sptr rev_dc = new vdgl_digital_curve(rev_intrp);
  return rev_dc;
}

