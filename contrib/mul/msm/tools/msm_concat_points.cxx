//:
// \file
// \brief Load in two or more sets of points, write out a file containing all points.
// \author Tim Cootes

#include <msm/msm_points.h>
#include <mbl/mbl_thin_plate_spline_2d.h>

#include <vul/vul_arg.h>

void print_usage()
{
  vcl_cout << "Usage: msm_concat_points points1.pts points2.pts ...  output.pts\n";
  vcl_cout<<"Load in two or more of points, write out a file containing all points."<<vcl_endl;
}

int main(int argc, char** argv)
{
  if (argc<4)
  {
    print_usage();
    return 0;
  }

  msm_points points;

  vcl_vector<vgl_point_2d<double> > pts, all_pts;

  unsigned n_sets = argc-2;
  vcl_cout<<"Reading in "<<n_sets<<" sets of points."<<vcl_endl;

  for (unsigned i=1;i<=n_sets;++i)
  {
    vcl_string path = vcl_string(argv[i]);
    if (!points.read_text_file(path))
    {
      vcl_cerr<<"Failed to load points from "<<path<<vcl_endl;
      return 1;
    }
    vcl_cout<<"Loaded "<<points.size()<<" points from "<<path<<vcl_endl;
    // Add points to list all_pts
    points.get_points(pts);
    for (unsigned j=0;j<pts.size();++j) all_pts.push_back(pts[j]);
  }

  points.set_points(all_pts);

  vcl_string out_path = vcl_string(argv[argc-1]);
  if (!points.write_text_file(out_path))
  {
    vcl_cerr<<"Failed to write points to "<<out_path<<vcl_endl;
    return 2;
  }
  vcl_cout<<all_pts.size()<<" points saved to "<<out_path<<vcl_endl;

  return 0;
}

