//:
// \file
// \brief Generate various synthetic shapes
// Enables testing and demonstration of various effects
// \author Tim Cootes

#include <sstream>
#include <msm/msm_points.h>
#include <vul/vul_arg.h>

void print_usage()
{
  std::cout << "Usage: msm_make_shapes -d new_points -n 24\n";
  std::cout<<"Generate various synthetic shapes."<<std::endl;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> output_dir("-d","Output directory","new_points");
  vul_arg<unsigned> n_shapes("-n","Number of shapes",24);

  vul_arg_parse(argc,argv);
  if (output_dir()=="")
  {
    print_usage();
    return 0;
  }

  unsigned n_pts=5;
  std::vector<vgl_point_2d<double> > pts(n_pts);

  // First four are corners of a square
  pts[0].set(0,0);
  pts[1].set(10,0);
  pts[2].set(10,10);
  pts[3].set(0,10);

  for (unsigned i=0;i<n_shapes();++i)
  {
    double A=double(i)/(n_shapes()-1)*0.5*3.1415;
    double r=10;
    pts[4].set(10+r*std::cos(A),r*std::sin(A));

    msm_points points;
    points.set_points(pts);

    std::stringstream ss;
    ss<<"box_hinge_";
    if (i<10) ss<<"0";
    ss<<i<<".pts";

    std::string pts_path=output_dir()+"/"+ss.str();

    if (!points.write_text_file(pts_path))
    {
      std::cerr<<"Failed to write points to "<<pts_path<<std::endl;
      return 1;
    }
  }

  std::cout<<"Saved points to "<<output_dir()<<"/box_hinge_XX.pts"<<std::endl;


  return 0;
}
