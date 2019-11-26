//:
// \file
// \brief Tool to combine point annotations from three or more sources.
// \author Tim Cootes
// Reads in multiple annotations of the same set of images.
// Compares the points from different annotators.
// Use a robust estimate (trimmed mean) of true point

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_int_list.h>
#include <mbl/mbl_parse_string_list.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include "vul/vul_arg.h"
#include "vul/vul_string.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_quick_file.h"

#include <msm/msm_points.h>
#include <mbl/mbl_stats_nd.h>
#include <mbl/mbl_sample_stats_1d.h>
#include "vul/vul_file.h"

/*
Points from different annotators are assumed to have the same filenames, but
live in separate directories.

Parameter file format:
<START FILE>
// Directories containing points from each annotator
points_dirs: {
/home/points1/
/home/points2/
}

// Text for key
key_text: { Marker1   Marker2 }

//: Directory to which to save resulting point files
output_dir: average_results

//: Number of points to keep to compute trimmed mean
n_to_keep=3;

//: When defined, only work on listed points
points_to_use: { 3 7 12 15 }

image_dir: /home/images/
images: {
  image1.pts : image1.jpg
  image2.pts : image2.jpg
}

<END FILE>
*/

void print_usage()
{
  std::cout << "msm_robust_mean -p param_file\n"
           << "Tool to combine point annotations from three or more sources..\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Number of points to keep to compute trimmed mean
  unsigned n_to_keep;

  //: When defined, only display listed points
  std::vector<unsigned> points_to_use;

  //: Directory containing images
  std::string image_dir;

  //: Directories containing points
  std::vector<std::string> points_dir;

  //: Text for key
  std::vector<std::string> key_text;

  //: Directory to which to save resulting point files
  std::string output_dir;

  //: List of image names
  std::vector<std::string> image_names;

  //: List of points file names
  std::vector<std::string> points_names;

  //: Parse named text file to read in data
  //  Throws a mbl_exception_parse_error if fails
  void read_from_file(const std::string& path);
};

//: Parse named text file to read in data
//  Throws a mbl_exception_parse_error if fails
void tool_params::read_from_file(const std::string& path)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
  {
    std::string error_msg = "Failed to open file: "+path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);

  output_dir=props.get_optional_property("output_dir","robust_mean");
  n_to_keep=vul_string_atoi(props.get_optional_property("n_to_keep","0"));

  std::string points_dirs_str=props.get_optional_property("points_dirs","");
  if (points_dirs_str!="")
  {
    mbl_parse_string_list(points_dirs_str,points_dir);
  }
  else
  {
    // Assume exactly two directories.
    points_dir.resize(2);
    points_dir[0]=props.get_required_property("points_dir1");
    points_dir[1]=props.get_required_property("points_dir2");
  }

  std::string key_text_str=props.get_optional_property("key_text","");
  if (key_text_str!="")
    mbl_parse_string_list(key_text_str,key_text);

  if (key_text.size()>0 && key_text.size()!=points_dir.size())
    std::cerr<<"WARNING: "
      <<"Number of key text lines does not match number of data sets"<<std::endl;

  std::string points_to_use_str=props.get_optional_property("points_to_use","");
  if (points_to_use_str!="")
  {
    std::stringstream ss(points_to_use_str);
    mbl_parse_int_list(ss,std::back_inserter(points_to_use),unsigned());
  }

  image_dir=props.get_optional_property("image_dir","./");

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

void load_shapes(const std::string& points_dir,
                 const std::vector<std::string>& filenames,
                 std::vector<msm_points>& shapes)
{
  unsigned n=filenames.size();

  shapes.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    std::string path = points_dir+"/"+filenames[i];
    if (!shapes[i].read_text_file(path))
    {
      mbl_exception_parse_error x("Failed to load points from "+path);
      mbl_exception_error(x);
    }

    if (shapes[i].size()!=shapes[0].size())
    {
      std::cerr<<"WARNING: "<<path<<" has different number of points ("
              <<shapes[i].size()<<") to first set ("<<shapes[0].size()<<")"<<std::endl;
    }
  }
}

//: Compute mean of points on j-th example
msm_points mean_shape(const std::vector<std::vector<msm_points> >& points, unsigned j)
{
  unsigned n_sets = points.size();
  msm_points mean=points[0][j];
  for (unsigned i=1;i<n_sets;++i) mean.vector()+=points[i][j].vector();
  mean.vector()/=n_sets;
  return mean;
}

//: Compute running estimate of mean and covariance matrix
class mbm_covar_stats_2d
{
  public:
  double sum1, sum2;
  double sum11, sum12, sum22;
  unsigned n;

  mbm_covar_stats_2d() : sum1(0),sum2(0),sum11(0),sum12(0),sum22(0),n(0) {}

  //: Add 2D observation
  void obs(double x, double y)
  {
    sum1+=x; sum2+=y;
    sum11+=x*x; sum12+=x*y; sum22+=y*y;
    n++;
  }

  void obs(vgl_point_2d<double> p) { obs(p.x(),p.y()); }
  void obs(vgl_vector_2d<double> p) { obs(p.x(),p.y()); }

  unsigned n_obs() const { return n; }

  double mean_x() const { return sum1/n; }
  double mean_y() const { return sum2/n; }
  vgl_point_2d<double> mean() const { return {mean_x(),mean_y()}; }

  double var11() const { return sum11/n-mean_x()*mean_x(); }
  double var12() const { return sum12/n-mean_x()*mean_y(); }
  double var22() const { return sum22/n-mean_y()*mean_y(); }

  //: Calculate eigenvalues of the covariance matrix and angle of evector 1
  void eigen_values(double& eval1, double& eval2, double& A) const
  {
    double dac=var11()-var22();
    double v12=var12();
    double d=0.5*std::sqrt(dac*dac+4*v12*v12);
    double hac = 0.5*(var11()+var22());
    eval1=hac+d;
    eval2=hac-d;

    A = std::atan2(eval1-var11(),var12());
  }

  double det() const { return var11()*var22()-var12()*var12(); }
};

vgl_point_2d<double> wtd_mean(const std::vector<vgl_point_2d<double> >& pts,
                              const std::vector<double>& w)
{
  double sum_x=0,sum_y=0,sum_w=0;
  for (unsigned i=0;i<pts.size();++i)
  {
    sum_x += w[i]*pts[i].x();
    sum_y += w[i]*pts[i].y();
    sum_w += w[i];
  }
  return vgl_point_2d<double>(sum_x/sum_w,sum_y/sum_w);
}


vgl_point_2d<double> robust_mean(const std::vector<vgl_point_2d<double> >& pts,
                                 unsigned n_to_discard)
{
  unsigned n=pts.size();
  std::vector<double> w(n,1.0);

  vgl_point_2d<double> mean=wtd_mean(pts,w);

  // Remove the furthest points, one at a time (trimmed mean)
  for (unsigned j=0;j<n_to_discard;++j)
  {
    unsigned ind;
    double max_d2=0;
    for (unsigned i=0;i<pts.size();++i)
    {
      if (w[i]==0) continue; // Ignore discarded points
      double d2=(pts[i]-mean).sqr_length();
      if (d2>max_d2) { max_d2=d2; ind=i; }
    }

    w[ind]=0;
    mean=wtd_mean(pts,w);
  }


  return mean;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg<std::string> out_dir("-o","Output directory","");
  vul_arg_parse(argc,argv);

  if (param_path()=="")
  {
    print_usage();
    return 0;
  }

  tool_params params;
  try
  {
    params.read_from_file(param_path());
  }
  catch (mbl_exception_parse_error& e)
  {
    std::cerr<<"Error: "<<e.what()<<'\n';
    return 1;
  }

  if (out_dir()!="")
    params.output_dir=out_dir();

  unsigned n_sets = params.points_dir.size();

  // Load in all points
  std::vector<std::vector<msm_points> > points(n_sets);
  for (unsigned i=0;i<n_sets;++i)
    load_shapes(params.points_dir[i],params.points_names,points[i]);

  unsigned n_egs=points[0].size();

  // Compute mean of points on first image to use as a reference.
  msm_points ref_shape=mean_shape(points,0);

  unsigned n_pts = points[0][0].size();
  std::vector<msm_points> mean_points(n_egs); // For results.

  std::vector<vgl_point_2d<double> > pts(n_sets);

  unsigned n_to_discard=0;
  if (params.n_to_keep>0) n_to_discard=n_sets-params.n_to_keep;

  if (params.points_to_use.size()==0)
  {
    // Use all points
    params.points_to_use.resize(n_pts);
    for (unsigned k=0;k<n_pts;++k)
      params.points_to_use[k]=k;
  }

  unsigned n_used=params.points_to_use.size();

  for (unsigned j=0;j<n_egs;++j)
  {
    mean_points[j].set_size(n_used);
    for (unsigned k=0;k<n_used;++k)
    {
      for (unsigned i=0;i<n_sets;++i)
        pts[i]=points[i][j][params.points_to_use[k]];

      mean_points[j].set_point(k,robust_mean(pts,n_to_discard));
    }
  }


    // check that output directory exists
  if (!vul_file::is_directory(params.output_dir))
  {
    std::cout<<"Directory "<<params.output_dir
            <<" does not exist. Creating it."<<std::endl;
    if (!vul_file::make_directory_path(params.output_dir))
    {
      std::cerr<<"Unable to create it."<<std::endl;
      return 1;
    }
  }

  for (unsigned j=0;j<n_egs;++j)
  {
    std::string pts_new_path = params.output_dir+"/"+
                               params.points_names[j];
    if (!mean_points[j].write_text_file(pts_new_path))
    {
        std::cerr<<"Cannot write updated points file to: "
                 <<params.output_dir<<std::endl;
        return 2;
    }
  }

  std::cout<<"Saved "<<n_egs<<" points files to "<<params.output_dir<<std::endl;

  return 0;
}
