#ifndef DigitalCurve_to_vdgl_h_
#define DigitalCurve_to_vdgl_h_

#include <DigitalGeometry/DigitalCurve.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>
#include <vcl_cassert.h>

inline vdgl_digital_curve DigitalCurve_to_vdgl(DigitalCurve const& dc)
{
  vcl_vector<vdgl_edgel> edgels; edgels.reserve(dc.length());
  float* dc_x = dc.GetX();
  float* dc_y = dc.GetY();
  float* dc_z = dc.GetZ();
  float* dc_g = dc.GetGrad();
  float* dc_t = dc.GetTheta();
  for (int i=0; i<dc.length(); ++i)
  {
    // Make sure that the digital curve is planar:
    assert (dc_z[i] == 0);
    // Create a vdgl_edgel and push it on the edgel list:
    edgels.push_back(vdgl_edgel(dc_x[i],dc_y[i],dc_g[i],dc_t[i]));
  }
  return vdgl_digital_curve(new vdgl_interpolator_linear(new vdgl_edgel_chain(edgels)));
}

#endif // DigitalCurve_to_vdgl_h_
