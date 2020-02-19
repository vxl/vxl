//:
// \file
// \brief Tool to split any curve at junctions or where curvature is high.
// \author Tim Cootes

#include <msm/msm_split_curves.h>
#include <msm/msm_points.h>
#include "vul/vul_arg.h"

void print_usage()
{
  std::cout << "Usage: msm_split_curves -c curves.crvs [-p points.pts] [-t 0.0 ] -o new_curves.crvs\n";
  std::cout<<"Tool to split any curve at junctions or where curvature is high.\n"
          <<"Junctions are internal points belonging to more than one curve.\n"
          <<"Curvature at a point is measured by the angle between two arcs going\n"
          <<"from that point to its neighbours.\n"
          <<"High curvature points are those with cos(angle)>cos_thresh (-t param).\n"
          <<"Curvature only considered if points are provided."<<std::endl;

  vul_arg_display_usage_and_exit();
}

int main(int argc, char** argv)
{
  vul_arg<std::string> curves_path("-c","Curves file");
  vul_arg<std::string> pts_path("-p","Optional points file (used to calculate curvature)");
  vul_arg<std::string> output_path("-o","Destination file for new curves","new_curves.crvs");
  vul_arg<double> cos_thresh("-t","Threshold on cos(angle) used to define high curvature points",-0.5);

  vul_arg_parse(argc,argv);

  if (curves_path().empty()) {
    print_usage();
    return 0;
  }

  msm_curves curves,new_curves;

  if (!curves.read_text_file(curves_path()))
  {
    std::cerr<<"Failed to load points from "<<curves_path()<<std::endl;
    return 1;
  }
  std::cout<<"Loaded "<<curves.size()<<" curves from "<<curves_path()<<std::endl;

  msm_points points;
  if (!pts_path().empty() && !points.read_text_file(pts_path())) {
    std::cerr<<"Failed to load points from "
            <<pts_path()<<std::endl;
    return 1;
  }

  if (points.size()==0)
  {
    std::vector<bool> is_junc;
    msm_get_junctions(curves,curves.max_index()+1,is_junc);
    msm_split_curves(curves,is_junc,new_curves);
  }
  else
  {
    std::vector<vgl_point_2d<double> > pts;
    points.get_points(pts);
    msm_split_curves(curves,pts,cos_thresh(),new_curves);
  }

  if (!new_curves.write_text_file(output_path()))
  {
    std::cerr<<"Failed to write new curves to "<<output_path()<<std::endl;
    return 2;
  }
  std::cout<<new_curves.size()<<" new curves saved to "<<output_path()<<std::endl;

  return 0;
}
