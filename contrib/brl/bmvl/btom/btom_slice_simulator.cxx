// This is brl/bmvl/btom/btom_slice_simulator.cxx
#include "btom_slice_simulator.h"
//:
// \file
// See btom_slice_simulator.h
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <btom/btom_gauss_cylinder.h>
#include <btom/btom_gauss_cylinder_sptr.h>
//
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------
// Constructors
//

btom_slice_simulator::btom_slice_simulator(btom_slice_simulator_params& ssp)
  : btom_slice_simulator_params(ssp)
{
}

btom_slice_simulator::~btom_slice_simulator()
{
}

void btom_slice_simulator::gaussian_sinogram(vil1_memory_image_of<float> & sinogram,
                                             vil1_memory_image_of<float> & reconst)
{
  int xmax = 512, ymax = 512, off = 64;
  float xt = xmax*0.5f, yt = ymax*0.5f;
  int theta_max = 360;
  int tmax = (int)vcl_sqrt(double((xmax)*(xmax)+(ymax)*(ymax)));
  int tt = (tmax-1)/2;
  sinogram.resize(theta_max,tmax);
  reconst.resize(xmax, ymax);
  reconst.fill(0.0);
  //construct the cylinders
  vcl_vector<btom_gauss_cylinder_sptr> cyls;
  double linear_cyl = vcl_sqrt(double(ncyl_));
  int delta_x = (int)((xmax-2*off)/linear_cyl),
      delta_y = (int)((ymax-2*off)/linear_cyl);
  float delta_sigma = (max_xy_sigma_-min_xy_sigma_)/ncyl_;
  float sigma = min_xy_sigma_;
  for (int y = off; y<ymax; y+=delta_y)
    for (int x=off; x<xmax; x+= delta_x, sigma+=delta_sigma)
      cyls.push_back(new btom_gauss_cylinder(sigma, 5.0, 10.0, 1.0, x-xt, y-yt));

  for (int t = -tt; t<tt; t++)
    for (int th = 0; th<theta_max; th++)
      {
        float total_density = 0;
        for (vcl_vector<btom_gauss_cylinder_sptr>::iterator cit = cyls.begin();
             cit != cyls.end(); cit++)
          total_density += (*cit)->radon_transform(float(th),float(t));
        sinogram(th,t+tt) = total_density;
      }

  for (int y = -(int)yt; y<yt; y++)
    for (int x=-(int)xt; x<xt; x++)
      {
        float intensity = 0.0;
        for (vcl_vector<btom_gauss_cylinder_sptr>::iterator cit = cyls.begin();
             cit != cyls.end(); cit++)
          intensity += (*cit)->cylinder_intensity(float(x),float(y));

        reconst(int(x+xt),int(y+yt)) = intensity;
      }
}
