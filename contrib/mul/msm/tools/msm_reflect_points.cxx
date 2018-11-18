//:
// \file
// \brief Load in a set of points, reflect about line x=x0, save.
// \author Tim Cootes

#include <iostream>
#include <cstdlib>
#include <msm/msm_points.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <msm/msm_reflect_shape.h>
#include <vul/vul_arg.h>

void print_usage()
{
  std::cout<<"msm_reflect_points -i input.pts -x 511 -o output.pts"<<std::endl;
  std::cout<<"Load in a set of points, reflect about line x=value, save."<<std::endl;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> in_path("-i","Input points file");
  vul_arg<std::string> out_path("-o","Output points file");
  vul_arg<double> x("-x","Reflection line, x=x0",0);
  vul_arg_parse(argc,argv);

  if (in_path()=="" || out_path()=="")
  {
    print_usage();
    return 0;
  }

  msm_points points, new_points;

  if (!points.read_text_file(in_path()))
  {
    std::cerr<<"Failed to load points from "<<in_path()<<std::endl;
    return 1;
  }
  std::cout<<"Loaded "<<points.size()<<" points from "<<in_path()<<std::endl;

  std::vector<unsigned> sym_pts(points.size());
  for (unsigned i=0;i<sym_pts.size();++i) sym_pts[i]=i;

  msm_reflect_shape_along_x(points,sym_pts,new_points,x());

  if (!new_points.write_text_file(out_path()))
  {
    std::cerr<<"Failed to write points to "<<out_path()<<std::endl;
    return 3;
  }
  std::cout<<new_points.size()<<" points saved to "<<out_path()<<std::endl;

  return 0;
}
