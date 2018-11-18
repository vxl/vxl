//:
// \file
// \brief Load in two or more sets of points, write out a file containing all points.
// \author Tim Cootes

#include <msm/msm_points.h>
#include <mbl/mbl_thin_plate_spline_2d.h>

#include <vul/vul_arg.h>

void print_usage()
{
  std::cout << "Usage: msm_concat_points points1.pts points2.pts ...  output.pts\n";
  std::cout<<"Load in two or more of points, write out a file containing all points."<<std::endl;
}

int main(int argc, char** argv)
{
  if (argc<4)
  {
    print_usage();
    return 0;
  }

  msm_points points;

  std::vector<vgl_point_2d<double> > pts, all_pts;

  unsigned n_sets = argc-2;
  std::cout<<"Reading in "<<n_sets<<" sets of points."<<std::endl;

  for (unsigned i=1;i<=n_sets;++i)
  {
    std::string path = std::string(argv[i]);
    if (!points.read_text_file(path))
    {
      std::cerr<<"Failed to load points from "<<path<<std::endl;
      return 1;
    }
    std::cout<<"Loaded "<<points.size()<<" points from "<<path<<std::endl;
    // Add points to list all_pts
    points.get_points(pts);
    for (auto pt : pts) all_pts.push_back(pt);
  }

  points.set_points(all_pts);

  std::string out_path = std::string(argv[argc-1]);
  if (!points.write_text_file(out_path))
  {
    std::cerr<<"Failed to write points to "<<out_path<<std::endl;
    return 2;
  }
  std::cout<<all_pts.size()<<" points saved to "<<out_path<<std::endl;

  return 0;
}
