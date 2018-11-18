//:
// \file
// \brief Calculate four points at corners of bounding box of input points.
// \author Tim Cootes

#include <msm/msm_points.h>
#include <mbl/mbl_thin_plate_spline_2d.h>

#include <vul/vul_arg.h>

void print_usage()
{
  std::cout << "Usage: msm_get_bounds -i source.pts -o output.pts\n";
  std::cout<<"Calculate four points at corners of bounding box of input points.\n"
          <<"Results saved to new_dest.pts"<<std::endl;

  vul_arg_display_usage_and_exit();

}

int main(int argc, char** argv)
{
  vul_arg<std::string> in_pts_path("-i","Source points");
  vul_arg<std::string> dest_pts_path("-o","Destination bounding box points","bounds.pts");

  vul_arg_parse(argc,argv);

  if (in_pts_path()=="")
  {
    print_usage();
    return 0;
  }

  msm_points points,dest_points(4);


  if (!points.read_text_file(in_pts_path()))
  {
    std::cerr<<"Failed to load points from "
            <<in_pts_path()<<std::endl;
    return 1;
  }

  vgl_box_2d<double> box = points.bounds();

  dest_points.set_point(0, box.min_x(),box.min_y());
  dest_points.set_point(1, box.max_x(),box.min_y());
  dest_points.set_point(2, box.max_x(),box.max_y());
  dest_points.set_point(3, box.min_x(),box.max_y());

  if (!dest_points.write_text_file(dest_pts_path()))
  {
    std::cerr<<"Failed to write points to "
            <<dest_pts_path()<<std::endl;
    return 2;
  }
  std::cout<<"Bounding box points saved to "<<dest_pts_path()<<std::endl;

  return 0;
}
