//:
// \file
// \brief Warp a set of points using a Thin Plate Spline
// \author Tim Cootes

#include <msm/msm_points.h>
#include <mbl/mbl_thin_plate_spline_2d.h>

#include <vul/vul_arg.h>

void print_usage()
{
  std::cout << "Usage: msm_apply_tps_warp -sc src_control.pts -dc"
           << " dest_control.pts -sp src.pts -dp new_dest.pts"
           << std::endl;
  std::cout<<"Uses a Thin Plate Spline, constructed from src_control"
          <<" points and dest_control points to warp src points.\n"
          <<"Results saved to new_dest.pts"<<std::endl;

  vul_arg_display_usage_and_exit();

}

int main(int argc, char** argv)
{
  vul_arg<std::string> src_con_pts_path("-sc","Source control points");
  vul_arg<std::string> dest_con_pts_path("-dc","Destination control points");
  vul_arg<std::string> src_pts_path("-sp","Source points");
  vul_arg<std::string> dest_pts_path("-dp","Destination points","output.pts");

  vul_arg_parse(argc,argv);

  if (src_con_pts_path()=="" ||
      dest_con_pts_path()=="" ||
      src_pts_path()=="")
  {
    print_usage();
    return 0;
  }

  msm_points src_con_points,dest_con_points;
  msm_points src_points,dest_points;

  std::vector<vgl_point_2d<double> > src_con_pts,dest_con_pts;
  std::vector<vgl_point_2d<double> > src_pts,dest_pts;


  if (!src_con_points.read_text_file(src_con_pts_path()))
  {
    std::cerr<<"Failed to load points from "
            <<src_con_pts_path()<<std::endl;
    return 1;
  }
  src_con_points.get_points(src_con_pts);

  if (!dest_con_points.read_text_file(dest_con_pts_path()))
  {
    std::cerr<<"Failed to load points from "
            <<dest_con_pts_path()<<std::endl;
    return 1;
  }
  dest_con_points.get_points(dest_con_pts);

  if (!src_points.read_text_file(src_pts_path()))
  {
    std::cerr<<"Failed to load points from "
            <<src_pts_path()<<std::endl;
    return 1;
  }
  src_points.get_points(src_pts);

  mbl_thin_plate_spline_2d tps;
  tps.build(src_con_pts,dest_con_pts);

  dest_pts.resize(src_pts.size());
  for (unsigned i=0;i<src_pts.size();++i)
    dest_pts[i]=tps(src_pts[i]);

  dest_points.set_points(dest_pts);
  if (!dest_points.write_text_file(dest_pts_path()))
  {
    std::cerr<<"Failed to write points to "
            <<dest_pts_path()<<std::endl;
    return 2;
  }
  std::cout<<"Warped points saved to "<<dest_pts_path()<<std::endl;

  return 0;
}
