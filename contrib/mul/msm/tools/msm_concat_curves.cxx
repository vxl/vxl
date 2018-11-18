//:
// \file
// \brief Load in two or more curve file, write out a file containing all curves.
// Works with msm_concat_points.  Resulting curves refers to combined set of points.
// Increments the indices in each curve appropriately, assuming that each set of curves
// includes the last point (to compute how many points are in each example).
// \author Tim Cootes

#include <msm/msm_curve.h>

#include <vul/vul_arg.h>

void print_usage()
{
  std::cout << "Usage: msm_concat_curves curves1.crvs curves2.crvs ... all_curves.crvs\n";
  std::cout<<"Load in two or more curves file, write out a file with their concatenation."<<std::endl;
}

int main(int argc, char** argv)
{
  if (argc<4)
  {
    print_usage();
    return 0;
  }

  msm_curves all_curves;

  unsigned n_sets = argc-2;
  std::cout<<"Reading in "<<n_sets<<" sets of curves."<<std::endl;

  unsigned offset=0;
  for (unsigned i=1;i<=n_sets;++i)
  {
    msm_curves curves;
    std::string path = std::string(argv[i]);
    if (!curves.read_text_file(path))
    {
      std::cerr<<"Failed to load points from "<<path<<std::endl;
      return 1;
    }
    std::cout<<"Loaded "<<curves.size()<<" curves from "<<path<<std::endl;

    unsigned n_pts = 1+curves.max_index();
    std::cout<<"n_points: "<<n_pts<<std::endl;

    // Add offset to each curve, allowing for points in previous examples
    for (unsigned j=0;j<curves.size();++j)
      curves[j].add_index_offset(offset);

    offset+=n_pts;

    // Add curves to list
    for (unsigned j=0;j<curves.size();++j) all_curves.push_back(curves[j]);
  }


  std::string out_path = std::string(argv[argc-1]);
  if (!all_curves.write_text_file(out_path))
  {
    std::cerr<<"Failed to write new curves to "<<out_path<<std::endl;
    return 2;
  }
  std::cout<<all_curves.size()<<" new curves saved to "<<out_path<<std::endl;

  return 0;
}
