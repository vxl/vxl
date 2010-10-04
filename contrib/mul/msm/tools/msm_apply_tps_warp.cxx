//:
// \file
// \brief Warp a set of points using a Thin Plate Spline
// \author Tim Cootes

#include <msm/msm_points.h>
#include <mbl/mbl_thin_plate_spline_2d.h>

#include <vul/vul_arg.h>

void print_usage()
{
  vcl_cout << "Usage: msm_apply_tps_warp -sc src_control.pts -dc"
           << " dest_control.pts -sp src.pts -dp new_dest.pts"
           << vcl_endl;
  vcl_cout<<"Uses a Thin Plate Spline, constructed from src_control"
          <<" points and dest_control points to warp src points.\n"
          <<"Results saved to new_dest.pts"<<vcl_endl;

  vul_arg_display_usage_and_exit();

}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> src_con_pts_path("-sc","Source control points");
  vul_arg<vcl_string> dest_con_pts_path("-dc","Destination control points");
  vul_arg<vcl_string> src_pts_path("-sp","Source points");
  vul_arg<vcl_string> dest_pts_path("-dp","Destination points","output.pts");

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

  vcl_vector<vgl_point_2d<double> > src_con_pts,dest_con_pts;
  vcl_vector<vgl_point_2d<double> > src_pts,dest_pts;


  if (!src_con_points.read_text_file(src_con_pts_path()))
  {
    vcl_cerr<<"Failed to load points from "
            <<src_con_pts_path()<<vcl_endl;
    return 1;
  }
  src_con_points.get_points(src_con_pts);

  if (!dest_con_points.read_text_file(dest_con_pts_path()))
  {
    vcl_cerr<<"Failed to load points from "
            <<dest_con_pts_path()<<vcl_endl;
    return 1;
  }
  dest_con_points.get_points(dest_con_pts);

  if (!src_points.read_text_file(src_pts_path()))
  {
    vcl_cerr<<"Failed to load points from "
            <<src_pts_path()<<vcl_endl;
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
    vcl_cerr<<"Failed to write points to "
            <<dest_pts_path()<<vcl_endl;
    return 2;
  }
  vcl_cout<<"Warped points saved to "<<dest_pts_path()<<vcl_endl;

  return 0;
}

