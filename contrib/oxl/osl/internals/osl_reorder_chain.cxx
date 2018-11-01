// This is oxl/osl/internals/osl_reorder_chain.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <vector>
#include "osl_reorder_chain.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <osl/osl_edgel_chain.h>
#include <osl/osl_OrthogRegress.h>

//:
// Takes a DigitalCurve that is assumed to form a closed loop and finds the
// largest corner in the chain. This point is then used as the starting point for
// the chain and the rest of the points are reordered. Doing this improves the
// segmentation for osl_edgel_chains composed of many straight lines.
//
void osl_reorder_chain(osl_edgel_chain *dc)
{
  int i,j,start=-1;
  int npts=5;
  int mid=npts/2;
  int size=dc->size();

  // The corner detection process involves fitting straight lines to npts
  // and computing the difference in orientation between between the current
  // and last fit. The largest change in slope is recorded and this point used
  // as the new start point.

  // Need at least npts+1 points to do anything (though may as well have more!)
  if ( size < npts+1 )
    return;

  osl_OrthogRegress data;
  double a0,b0,a1,b1,dot,diff,max=0.0;
  double MPIby2 = vnl_math::pi/2.0;

  // Add the first npts to the data set and fit.
  for (i=0; i<npts; ++i)
    data.IncrByXY(dc->GetX(i),dc->GetY(i));
  data.Fit();
  a0 = data.GetA();  b0 = data.GetB();

  // Now redo the fits up to the end of the data set
  //FIXME: should this say "j<size", not "i<size"? -- fsm
  for (j=0; i<size; i++,j++)  {
    data.DecrByXY(dc->GetX(j),dc->GetY(j));
    data.IncrByXY(dc->GetX(i),dc->GetY(i));
    data.Fit();
    a1 = data.GetA();  b1 = data.GetB();

    // Compute the angle between this and the last fit. Store if this is
    // the new maximum. Note that the fits are normalised so that a^2+b^2=1.
    dot = a0*a1+b0*b1;
    if ( dot > 1.0 )
      dot = 1.0;
    if ( dot < -1.0 )
      dot = -1.0;
    diff = std::acos(dot);
    if ( diff > MPIby2)
      diff = vnl_math::pi - diff;
    if ( diff > max ) {
      max = diff;
      start = j+mid;
    }

    a0 = a1;  b0 = b1;
  }

  // Simple error check
  if ( start < 0 )
    return;

  // The curve should now begin at start - shuffle the points around.
  // Buffer the points first, then move.
  std::vector<float> x(size);
  std::vector<float> y(size);
  std::vector<float> grad(size);
  std::vector<float> theta(size);

  for (i=0; i<size; ++i)  {
    x[i] = dc->GetX(i);
    y[i] = dc->GetY(i);
    grad[i] = dc->GetGrad(i);
    theta[i] = dc->GetTheta(i);
  }

  for (i=0,j=start; i<size; i++,j++)  {
    if ( j == size )
      j = 0;
    dc->SetX(x[j],i);
    dc->SetY(y[j],i);
    dc->SetGrad(grad[j],i);
    dc->SetTheta(theta[j],i);
  }
}
