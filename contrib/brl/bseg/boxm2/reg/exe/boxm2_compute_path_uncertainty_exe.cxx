//:
// \file
// \brief  First attempt at multi gpu render
// \author Andy Miller
// \date 13-Oct-2011
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_load.h>

#include <vnl/vnl_float_4x4.h>
#include <vnl/vnl_float_4.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_inverse.h>

#include <vul/vul_arg.h>
#include <vcl_algorithm.h>
#include <vcl_fstream.h>

int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> img_file("-img", "image filename", "");
  vul_arg_parse(argc, argv);

  //create scene
  vil_image_view_base_sptr img = vil_load(img_file().c_str());

  float phi_min = -float(vnl_math::pi);
  float phi_max =  float(vnl_math::pi);

  float theta_min = 0.0f;
  float theta_max = float(vnl_math::pi/4);
  vil_image_view<float> * uimg  = dynamic_cast<vil_image_view<float> * > (img.ptr());

  float theta_inc = (theta_max- theta_min)/((float)uimg->nj());
  float phi_inc = (phi_max- phi_min)/((float)uimg->ni());

  vnl_float_4x4 A(0.0);
  vnl_float_4 b;
  b[0] = 0.0;
  b[1] = 0.0;
  b[2] = 0.0;
  b[3] = 0.0;
  for (unsigned i = 0; i < uimg->ni(); i++)
  {
    float phi = phi_min + phi_inc * (float)(i);
    float sum_uj = 0.0;
    float sum_thetaj_uj = 0.0;
    for (unsigned j = 0; j < uimg->nj(); j++)
    {
      float theta = theta_min + theta_inc * (float)(j);
      float u = 1 - (*uimg)(i,j);
      sum_uj += u*u;
      sum_thetaj_uj += theta*u*u;
    }
    vnl_float_4x4 temp_A(1.0);
    vnl_float_4   temp_b;
    temp_b[0] = 1.0;

    for (unsigned r = 0; r < 4; r ++)
    {
      for (unsigned c = 0; c < 4; c ++)
      {
        if (r==0 && c==0)
          temp_A(r,c) = 1.0;
        else if (c == 0)
          temp_A(r,c) = temp_A(r-1,c) * phi;
        else
          temp_A(r,c) = temp_A(r,c-1) * phi;
      }
      if ( r != 0)
        temp_b[r] = temp_b[r-1] * phi;
    }

    A= A + sum_uj* temp_A;
    b= b + sum_thetaj_uj*temp_b;
  }

  vnl_float_4 x = vnl_inverse(A)*b;
  vnl_float_4 phis;
  vcl_cout<<"Solution  "<<x<<vcl_endl;

  vcl_ofstream ofile("f:/data.txt");
  for (float phi=phi_min; phi<phi_max; phi+=phi_inc)
  ofile<<phi<<','<<vcl_endl;

  for (float phi=phi_min; phi<phi_max; phi+=phi_inc)
  {
    phis[0] = 1.0;
    phis[1] = phi;
    phis[2] = phi*phi;
    phis[3] = phi*phi*phi;

    ofile<<dot_product(phis,x)<<',';
#ifdef DEBUG
    vcl_cout<<dot_product(phis,x)<<vcl_endl;
#endif
  }
  ofile.close();
  return 0;
}
