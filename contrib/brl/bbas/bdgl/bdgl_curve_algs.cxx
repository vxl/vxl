//:
// \file
// \author J.L. Mundy

#include <vcl_iostream.h>
#include <bdgl/bdgl_curve_algs.h>
#include <vdgl/vdgl_edgel_chain.h>

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
  if (!ec)
  {
    vcl_cout << "In bdgl_curve_algs::closest_point(..) - warning, null chain\n";
    return 0;
  }
  //for now just scan the curve and save the closest point
  double mind = -1.0;
  int N =ec->size(), imin = 0;

  for (int i = 0; i<N; i++)
  {
    vdgl_edgel ed = ec->edgel(i);
    double d = (ed.x()-x)*(ed.x()-x) + (ed.y()-y)*(ed.y()-y);
    if (mind < 0 || d<mind)
    {
      mind = d;
      imin = i;
    }
  }
  return imin;
}
