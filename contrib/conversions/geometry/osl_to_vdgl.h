#ifndef osl_to_vdgl_h_
#define osl_to_vdgl_h_

#include <osl/osl_edgel_chain.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_interpolator_linear.h>

inline vdgl_digital_curve osl_to_vdgl(osl_edgel_chain const& dc)
{
  vcl_vector<vdgl_edgel> edgels; edgels.reserve(dc.size());
  for (unsigned int i=0; i<dc.size(); ++i)
  {
    // Create a vdgl_edgel and push it on the edgel list:
    edgels.push_back(vdgl_edgel(dc.x[i],dc.y[i],dc.grad[i],dc.theta[i]));
  }
  return vdgl_digital_curve(new vdgl_interpolator_linear(new vdgl_edgel_chain(edgels)));
}

#endif // osl_to_vdgl_h_
