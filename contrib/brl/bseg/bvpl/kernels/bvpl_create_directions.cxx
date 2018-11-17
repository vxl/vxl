#include <iostream>
#include <iomanip>
#include "bvpl_create_directions.h"
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bxml/bsvg/bsvg_document.h>
#include <bxml/bsvg/bsvg_element.h>
#include <bxml/bxml_write.h>

bvpl_create_directions_xyz::bvpl_create_directions_xyz()
{
  axes_.clear();
  angles_.clear();

  vnl_float_3 axis_x (1.0f, 0.0f, 0.0f);
  axes_.push_back(axis_x);
  angles_.push_back(0.0f);
  vnl_float_3 axis_y (0.0f, 1.0f, 0.0f);
  axes_.push_back(axis_y);
  angles_.push_back(0.0f);
  vnl_float_3 axis_z (0.0f, 0.0f, 1.0f);
  axes_.push_back(axis_z);
  angles_.push_back(0.0f);
}

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
    for (double theta=0.0; theta<vnl_math::twopi-1e-5; theta+=theta_res)
    {
      axis[0] = float(std::cos(theta) * std::sin(phi));
      axis[1] = float(std::sin(theta) * std::sin(phi));
      axis[2] = float(std::cos(phi));
      axes_.push_back(axis);
      angles_.push_back(0.0f);
    }
  }
 this->save_directions_map("directions_a.svg");
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
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    axes_.push_back(axis);
    angles_.push_back(0.0f);
  }
  //when zenith is pi/4 traverse all hemisphere
  phi = vnl_math::pi_over_4;
  for (;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    axes_.push_back(axis);
    angles_.push_back(0.0f);
  }
  //when zenith is pi/2 we only traverse half a hemisphere
  phi = vnl_math::pi_over_2;
  theta =0.0;
  for (;theta < vnl_math::pi-1e-5; theta +=theta_res)
  {
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    axes_.push_back(axis);
    angles_.push_back(0.0f);
   }

  save_directions_map("directions_b.svg");
}

bvpl_create_directions_c::bvpl_create_directions_c()
{
  double theta = 0.0;
  double phi = 0.0;

  //when zenith angle is 0
  vnl_float_3 axis(0.0f, 0.0f, 1.0f);
  axes_.push_back(axis);
  angles_.push_back(0.0f);

  //when zenith is pi/6 traverse all hemisphere
  phi = vnl_math::pi/6;
  double theta_res = vnl_math::pi_over_4; //azimuth; phi_res = zenith (from the pole)
  for (;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    axes_.push_back(axis);
    angles_.push_back(0.0f);
  }
  //when zenith is pi/3 traverse all hemisphere
  phi = vnl_math::pi/3;

  for (theta=0.0;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    axes_.push_back(axis);
    angles_.push_back(0.0f);
  }
  phi = vnl_math::pi/4;

  for (theta=0.0;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    axes_.push_back(axis);
    angles_.push_back(0.0f);
  }
  //when zenith is pi/2 we only traverse half a hemisphere
  phi = vnl_math::pi_over_2;
  theta =0.0;
  for (;theta < vnl_math::pi-1e-5; theta +=theta_res)
  {
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    axes_.push_back(axis);
    angles_.push_back(0.0f);
   }

  save_directions_map("directions_c.svg");
}

void bvpl_create_directions_base::save_directions_map(const std::string& outfile)
{
  bsvg_document doc(400, 800);
  auto iter1 = axes_.begin();
  auto iter2 = angles_.begin();
  int i=0;
  for (;iter1!=axes_.end(); ++iter1, ++iter2, i++)
  {
    std::ostringstream os_dir;
    os_dir.precision(2);
    os_dir<<'['<<std::setw(3)<<(*iter1)[0]<<','<<std::setw(3)<<(*iter1)[1]<<','<<std::setw(3)<<(*iter1)[2]<<','<<std::setw(2)<<*iter2<<']';
    bsvg_text* t = new bsvg_text(os_dir.str());
    t->set_font_size(15);
    t->set_location(10.0f, 15.0f*float(i+1));

    doc.add_element(t);
  }

  bxml_write(outfile, doc);
}


/*******************************Corners**************************************/

bvpl_main_corner_dirs::bvpl_main_corner_dirs()
{
  //This is for 4 main angles
  vnl_float_3 axis(0.0f, 1.0f, 0.0f);
  axes_.push_back(axis);
  angles_.push_back(float(0.0f));
  axes_.push_back(axis);
  angles_.push_back(float(vnl_math::pi_over_2));
  axes_.push_back(axis);
  angles_.push_back(float(vnl_math::pi));
  axes_.push_back(axis);
  angles_.push_back(3.0f *float(vnl_math::pi_over_2));
}

bvpl_main_plane_corner_dirs::bvpl_main_plane_corner_dirs()
{
  vnl_float_3 axis(0.0f, 1.0f, 0.0f);
  double angle_res = vnl_math::pi_over_4;
  for (double angle =0.0; angle<vnl_math::twopi-1e-5; angle+=angle_res)
  {
    axes_.push_back(axis);
    angles_.push_back(float(angle));
  }
}

bvpl_all_corner_dirs::bvpl_all_corner_dirs()
{
  double theta = 0.0;
  double phi = 0.0;

  //polar phi=0
  vnl_float_3 axis(0.0f, 0.0f, 1.0f);
  double angle_res = vnl_math::pi_over_4;
  for (double angle =0.0; angle<vnl_math::twopi-1e-5; angle+=angle_res)
  {
    axes_.push_back(axis);
    angles_.push_back(float(angle));
  }

  double theta_res = vnl_math::pi_over_4;

  //when phi is pi/4 traverse all hemisphere
  phi = (vnl_math::pi_over_4);
  for (double angle =0.0; angle<vnl_math::twopi-1e-5; angle+=angle_res)
  {
    for (theta =0.0;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
    {
      axis[0] = float(std::cos(theta) * std::sin(phi));
      axis[1] = float(std::sin(theta) * std::sin(phi));
      axis[2] = float(std::cos(phi));
      axes_.push_back(axis);
      angles_.push_back(float(angle));
    }
  }


  //when zenith is pi/2 we only traverse half a hemisphere
  phi = double(vnl_math::pi_over_2);
  for (theta =0.0;theta < (vnl_math::pi)-1e-5; theta +=theta_res)
  {
    for (double angle =0.0; angle<vnl_math::twopi-1e-5; angle+=angle_res)
    {
      axis[0] = float(std::cos(theta) * std::sin(phi));
      axis[1] = float(std::sin(theta) * std::sin(phi));
      axis[2] = float(std::cos(phi));
      axes_.push_back(axis);
      angles_.push_back(float(angle));
    }
  }
   std::cout << "************" << angles_.size() << std::endl;
  save_directions_map("directions_c.svg");
}


bvpl_pi_over_2_corner_dirs::bvpl_pi_over_2_corner_dirs()
{
  double theta = 0.0;
  double phi = 0.0;

  //polar phi=0
  vnl_float_3 axis(0.0f, 0.0f, 1.0f);
  double angle_res = vnl_math::pi_over_2;
  for (double angle =0.0; angle<vnl_math::twopi-1e-5; angle+=angle_res)
  {
    axes_.push_back(axis);
    angles_.push_back(float(angle));
  }

  double theta_res = vnl_math::pi_over_2;


  //when zenith is pi/2 we only traverse half a hemisphere
  phi = double(vnl_math::pi_over_2);
  for (theta =0.0;theta < (vnl_math::pi)-1e-5; theta +=theta_res)
  {
    for (double angle =0.0; angle<vnl_math::twopi-1e-5; angle+=angle_res)
    {
      axis[0] = float(std::cos(theta) * std::sin(phi));
      axis[1] = float(std::sin(theta) * std::sin(phi));
      axis[2] = float(std::cos(phi));
      axes_.push_back(axis);
      angles_.push_back(float(angle));
    }
  }
  std::cout << "************" << angles_.size() << std::endl;
  save_directions_map("directions_90.svg");
}
