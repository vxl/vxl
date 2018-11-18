//:
// \file
// \brief Load in a set of points, and a list of indices. Write out given subset.
// \author Tim Cootes

#include <iostream>
#include <cstdlib>
#include <msm/msm_points.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vul/vul_arg.h>

void print_usage()
{
  std::cout<<"points_subset index_file in_pts out_pts"<<std::endl;
  std::cout<<"Load list of indices from text file index_file"<<std::endl;
  std::cout<<"Load set of points from in_pts"<<std::endl;
  std::cout<<"Select subset of in_pts, using indices in index_file"<<std::endl;
  std::cout<<"Save to out_pts"<<std::endl;
  std::cout<<"Index file just lists integers, in range [0,n_pts-1], to be used."<<std::endl;
  std::cout<<"Note, it can have comments using //"<<std::endl;
  std::cout<<"For instance:"<<std::endl;
  std::cout<<" 0 1 // Eyes"<<std::endl;
  std::cout<<" 5 6 7 8 9 // Mouth"<<std::endl;
}

bool load_index(const char* path, size_t n_points, std::vector<int>& index)
{
  // Load in indices, and check they are in [0,n-1]
  std::ifstream ifs(path,std::ios::in);
  if (!ifs)
  {
    std::cerr<<"Unable to open "<<path<<std::endl;
    return false;
  }

  constexpr int MAXLEN = 255;
  char comment[MAXLEN];

  ifs>>std::ws;
  while (!ifs.eof())
  {
    std::string s;
    ifs>>s;
    if ( (s.size()>=2) && (s[0]=='/') && (s[1]=='/') )
    {
      // Comment line, so read to end
      ifs.getline(comment,MAXLEN);
      ifs>>std::ws;
      continue;
    }

    int i = atoi(s.c_str());

    if (i<0 || i>=n_points)
    {
      std::cerr<<"Illegal index: "<<i<<" is not in range [0.."<<n_points-1<<"]"<<std::endl;
      return false;
    }

    index.push_back(i);
    ifs>>std::ws;
  }
  ifs.close();
  return true;
}

int main(int argc, char** argv)
{
  if (argc!=4)
  {
    print_usage();
    return 0;
  }

  msm_points points, new_points;

  std::string in_path = std::string(argv[2]);
  if (!points.read_text_file(in_path))
  {
    std::cerr<<"Failed to load points from "<<in_path<<std::endl;
    return 1;
  }
  std::cout<<"Loaded "<<points.size()<<" points from "<<in_path<<std::endl;

  std::vector<int> index;
  if (!load_index(argv[1],points.size(),index)) return 2;

  std::cout<<"Loaded "<<index.size()<<" indices"<<std::endl;

  // Create subset
  int n = index.size();
  std::vector<vgl_point_2d<double> > new_pts(n);
  for (int i=0;i<n;++i)
    new_pts[i] = points[index[i]];

  new_points.set_points(new_pts);

  std::string out_path = std::string(argv[3]);
  if (!new_points.write_text_file(out_path))
  {
    std::cerr<<"Failed to write points to "<<out_path<<std::endl;
    return 3;
  }
  std::cout<<new_points.size()<<" points saved to "<<out_path<<std::endl;

  return 0;
}
