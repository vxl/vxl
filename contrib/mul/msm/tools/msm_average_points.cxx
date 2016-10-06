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
  vcl_cout<<"msm_average_points"<<vcl_endl;
  vcl_cout << "Usage: msm_average_points points1.pts points2.pts ...  output.pts\n";
  vcl_cout<<"Load in two or more of points, write out a file containing averaged points over all sets."<<vcl_endl;
  vcl_cout<<"All point sets need to have the same number of points."<<vcl_endl;
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
  unsigned n_pts = 0;
  vcl_cout<<"Reading in "<<n_sets<<" sets of points."<<vcl_endl;

  for (unsigned i=1;i<=n_sets;++i)
  {
    vcl_string path = vcl_string(argv[i]);
    if (!points.read_text_file(path))
    {
      vcl_cerr<<"Error: Failed to load points from "<<path<<vcl_endl;
      return 1;
    }
    vcl_cout<<"Loaded "<<points.size()<<" points from "<<path<<vcl_endl;

    if (i==1) 
    { 
      n_pts = points.size(); 
	
      // Initialise/add points to sum_all_pts
      points.get_points(pts);
      for (unsigned j=0;j<pts.size();++j) { all_pts.push_back(pts[j]); }
    }	
    else if (n_pts != points.size())
    {
      vcl_cerr<<"Error: Point set "<<path<<" has a different number of points than the first set."<<vcl_endl;
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
  for (unsigned j=0;j<all_pts.size();++j) 
    { all_pts[j].set(all_pts[j].x()/n_sets, all_pts[j].y()/n_sets); }
 
  points.set_points(all_pts);

  vcl_string out_path = vcl_string(argv[argc-1]);
  if (!points.write_text_file(out_path))
  {
    vcl_cerr<<"Error: Failed to write points to "<<out_path<<vcl_endl;
    return 3;
  }
  vcl_cout<<all_pts.size()<<" points saved to "<<out_path<<vcl_endl;

  return 0;
}

