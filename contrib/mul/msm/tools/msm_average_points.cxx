//:
// \file
// \brief Load in two or more sets of points, write out a file containing averaged points over all sets.
//  All point sets need to have the same number of points.
// \author Claudia Lindner

#include <msm/msm_points.h>
#include <mbl/mbl_thin_plate_spline_2d.h>

#include <vul/vul_arg.h>

void print_usage()
{
  std::cout<<"msm_average_points"<<std::endl;
  std::cout << "Usage: msm_average_points points1.pts points2.pts ...  output.pts\n";
  std::cout<<"Load in two or more of points, write out a file containing averaged points over all sets."<<std::endl;
  std::cout<<"All point sets need to have the same number of points."<<std::endl;
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
  unsigned n_pts = 0;
  std::cout<<"Reading in "<<n_sets<<" sets of points."<<std::endl;

  for (unsigned i=1;i<=n_sets;++i)
  {
    std::string path = std::string(argv[i]);
    if (!points.read_text_file(path))
    {
      std::cerr<<"Error: Failed to load points from "<<path<<std::endl;
      return 1;
    }
    std::cout<<"Loaded "<<points.size()<<" points from "<<path<<std::endl;

    if (i==1)
    {
      n_pts = points.size();

      // Initialise/add points to sum_all_pts
      points.get_points(pts);
      for (auto pt : pts) { all_pts.push_back(pt); }
    }
    else if (n_pts != points.size())
    {
      std::cerr<<"Error: Point set "<<path<<" has a different number of points than the first set."<<std::endl;
      return 2;
    }
    else
   {
      // Add points to all_pts
      points.get_points(pts);
      for (unsigned j=0;j<all_pts.size();++j)
        { all_pts[j].set(all_pts[j].x()+ pts[j].x(), all_pts[j].y()+ pts[j].y()); }
   }
  }

  // Update all_pts with the average over all sets
  for (auto & all_pt : all_pts)
    { all_pt.set(all_pt.x()/n_sets, all_pt.y()/n_sets); }

  points.set_points(all_pts);

  std::string out_path = std::string(argv[argc-1]);
  if (!points.write_text_file(out_path))
  {
    std::cerr<<"Error: Failed to write points to "<<out_path<<std::endl;
    return 3;
  }
  std::cout<<all_pts.size()<<" points saved to "<<out_path<<std::endl;

  return 0;
}
