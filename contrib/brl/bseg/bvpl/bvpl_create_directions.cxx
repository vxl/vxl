#include <bvpl/bvpl_create_directions.h>
#include <vnl/vnl_math.h>

bvpl_create_directions_a::bvpl_create_directions_a()
{
  //polar phi=0
  vnl_float_3 axis(0.0f, 0.0f, 1.0f);
  axes_.push_back(axis);
  angles_.push_back(0.0f);
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
      angles_.push_back(0.0f);
    }
  }
}

bvpl_create_directions_b::bvpl_create_directions_b()
{
  double theta = 0.0;
  double phi = 0.0;

  //when zenith angle is 0
  vnl_float_3 axis(0.0f, 0.0f, 1.0f);
  axes_.push_back(axis);
  angles_.push_back(0.0f);

  //when zenith is pi/4 traverse all hemisphere
  phi = vnl_math::pi_over_4;
  double theta_res = vnl_math::pi_over_4; //azimuth; phi_res = zenith (from the pole)
  for (;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = vcl_cos(theta) * vcl_sin(phi);
    axis[1] = vcl_sin(theta) * vcl_sin(phi);
    axis[2] = vcl_cos(phi);
    axes_.push_back(axis);
    angles_.push_back(0.0f);
  }

  //when zenith is pi/2 we only traverse half a hemisphere
  phi = vnl_math::pi_over_2;
  theta =0.0;
  for (;theta < vnl_math::pi-1e-5; theta +=theta_res)
  {
    axis[0] = float(vcl_cos(theta) * vcl_sin(phi));
    axis[1] = float(vcl_sin(theta) * vcl_sin(phi));
    axis[2] = float(vcl_cos(phi));
    axes_.push_back(axis);
    angles_.push_back(0.0f);
   }
}

bvpl_create_directions_c::bvpl_create_directions_c()
{
  double theta = 0.0;
  double phi = 0.0;

  //polar phi=0
  vnl_float_3 axis(0.0f, 0.0f, 1.0f);
  double angle_res = vnl_math::pi_over_4;
  for (double angle =0.0; angle<2.0*vnl_math::pi-1e-5; angle+=angle_res)
  {
    axes_.push_back(axis);
    angles_.push_back(angle);
  }

  double theta_res = vnl_math::pi_over_4;

  //when phi is pi/4 traverse all hemisphere
  phi = (vnl_math::pi_over_4);
  for (double angle =0.0; angle<2.0*vnl_math::pi-1e-5; angle+=angle_res)
  {
    for (;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
    {
      axis[0] = vcl_cos(theta) * vcl_sin(phi);
      axis[1] = vcl_sin(theta) * vcl_sin(phi);
      axis[2] = vcl_cos(phi);
      axes_.push_back(axis);
      angles_.push_back(angle);
    }
  }

  //when zenith is pi/2 we only traverse half a hemisphere
  phi = double(vnl_math::pi_over_2);
  theta =0.0;
  for (double angle =0.0; angle<2.0*vnl_math::pi-1e-5; angle+=angle_res)
  {
    for (;theta < (vnl_math::pi)-1e-5; theta +=theta_res)
    {
      axis[0] = float(vcl_cos(theta) * vcl_sin(phi));
      axis[1] = float(vcl_sin(theta) * vcl_sin(phi));
      axis[2] = float(vcl_cos(phi));
      axes_.push_back(axis);
      angles_.push_back(angle);
    }
  }
}
