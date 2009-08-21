#include <bvpl/bvpl_create_directions.h>
#include <vnl/vnl_math.h>

bvpl_create_directions_a::bvpl_create_directions_a()
{
    //polar phi=0
    vnl_float_3 axis(0.0f, 0.0f, 1.0f);
    axes_.push_back(axis);
    double theta_res = vnl_math::pi_over_4;
    double phi_res   = vnl_math::pi_over_4;
    // phi=pi/4,pi/2,
    for (double phi=vnl_math::pi_over_4; phi<3*vnl_math::pi/4-1e-5; phi+=phi_res)
    {
        // theta=0,pi/4,pi/2,3pi/4,pi,5pi/4,3pi/2,7pi/4
        for (double theta=0.0; theta<2.0*vnl_math::pi-1e-5; theta+=theta_res)
        {
            axis[0] = float(vcl_cos(theta) * vcl_sin(phi));
            axis[1] = float(vcl_sin(theta) * vcl_sin(phi));
            axis[2] = float(vcl_cos(phi));
            axes_.push_back(axis);
        }
    }

}

bvpl_create_directions_b::bvpl_create_directions_b()
{
  float theta_res = float(vnl_math::pi_over_4); //azimuth; phi_res = zenith (from the pole)
  vnl_float_3 axis;
  float theta = 0.0f;
  float phi = 0.0f;

  //when zenith angle is 0
  axis[0] =0.0f;
  axis[1] =0.0f;
  axis[2] =1.0f;
  axes_.push_back(axis);
  //when zenith is pi/4 traverse all hemisphere
  phi = float(vnl_math::pi_over_4);
  for (;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = vcl_cos(theta) * vcl_sin(phi);
    axis[1] = vcl_sin(theta) * vcl_sin(phi);
    axis[2] = vcl_cos(phi);
    axes_.push_back(axis);
  }

  //when zenith is pi/2 we only traverse half a hemisphere
  phi = float(vnl_math::pi_over_2);
  theta =0.0f;
  for (;theta < float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = float(vcl_cos(theta) * vcl_sin(phi));
    axis[1] = float(vcl_sin(theta) * vcl_sin(phi));
    axis[2] = float(vcl_cos(phi));
    axes_.push_back(axis);
   }


}