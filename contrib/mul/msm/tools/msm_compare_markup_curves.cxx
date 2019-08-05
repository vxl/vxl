//:
// \file
// \brief Tool to compare point annotations from two or more sources.
// \author Tim Cootes
// Reads in multiple annotations of the same set of images.
// Compares the points from one annotator to curves from another.

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_int_list.h>
#include <mbl/mbl_parse_string_list.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vul/vul_arg.h>
#include <vul/vul_string.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_quick_file.h>

#include <msm/msm_add_all_loaders.h>
#include <msm/msm_points.h>
#include <mbl/mbl_stats_nd.h>
#include <mbl/mbl_sample_stats_1d.h>
#include <msm/utils/msm_dist_to_curves.h>
#include <msm/utils/msm_closest_pt.h>

/*
Points from different annotators are assumed to have the same filenames, but
live in separate directories.

Parameter file format:
<START FILE>
// Directories containing points from each annotator
points_dirs: {
/home/points1/
/home/points2/
/home/points3/
}

// Text indicating marker names
key_text: { Marker1 Marker2 Marker3}


//: Path for image+ellipses, one ellipse per point summarising all sets.
output_summary_path: results.txt

//: Path for details of consistency calculations
output_consistency_path: consistency_results.txt

//: When defined, only display listed points
points_to_show: { 3 7 12 15 }

curves_path: shape.crvs

// Note: Images not used.
image_dir: /home/images/
images: {
  image1.pts : image1.jpg
  image2.pts : image2.jpg
}
<END FILE>
*/


void print_usage()
{
  std::cout << "msm_compare_markup_curves -p param_file\n"
           << "Analyse differences between curves from two or more annotators.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: When defined, only display listed points
  std::vector<unsigned> points_to_show;

  //: Directory containing images
  std::string image_dir;

  //: Directories containing points
  std::vector<std::string> points_dir;

  //: Text for key
  std::vector<std::string> key_text;

  //: Path for summary results
  std::string output_summary_path;

  //: Path for details of consistency calculations
  std::string output_consistency_path;


  std::string curves_path;

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

  output_summary_path=props.get_optional_property("output_summary_path",
                                                  "results_summary.txt");

  output_consistency_path=props.get_optional_property("output_consistency_path",
                                                  "results_consistency.txt");

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

  curves_path=props.get_optional_property("curves_path","");

  std::string key_text_str=props.get_optional_property("key_text","");
  if (key_text_str!="")
    mbl_parse_string_list(key_text_str,key_text);

  if (key_text.size()>0 && key_text.size()!=points_dir.size())
    std::cerr<<"WARNING: "
      <<"Number of key text lines does not match number of data sets"<<std::endl;

  std::string points_to_show_str=props.get_optional_property("points_to_show","");
  if (points_to_show_str!="")
  {
    std::stringstream ss(points_to_show_str);
    mbl_parse_int_list(ss,std::back_inserter(points_to_show),unsigned());
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

void compare_two_sets(const std::vector<msm_points>& points0,
                      const std::vector<msm_points>& points1,
                      const msm_curves& curves,
                      std::vector<mbl_sample_stats_1d>& p2p_stats,
                      mbl_sample_stats_1d& all_p2p_stats,
                      std::vector<mbl_sample_stats_1d>& p2c_stats,
                      mbl_sample_stats_1d& all_p2c_stats)
{
  unsigned n=points0.size();
  unsigned n_pts=points0[0].size();
  p2p_stats.resize(n_pts);
  p2c_stats.resize(n_pts);
  msm_points dpoints;
  for (unsigned i=0;i<n;++i)
  {
    dpoints.vector()=points0[i].vector()-points1[i].vector();

    for (unsigned k=0;k<n_pts;++k)
    {
      double d2=dpoints[k].x()*dpoints[k].x() + dpoints[k].y()*dpoints[k].y();
      p2p_stats[k].add_sample(std::sqrt(d2));
      all_p2p_stats.add_sample(std::sqrt(d2));

      // Compute distance of point k to curve on other shape
      double d2c01=msm_dist_to_curves(points0[i],curves,points1[i][k]);
      double d2c10=msm_dist_to_curves(points1[i],curves,points0[i][k]);
      double d2c=0.5*(d2c01+d2c10);  // Average of 0-1 and 1-0
      p2c_stats[k].add_sample(d2c);
      all_p2c_stats.add_sample(d2c);
    }
  }
}


//: Examine consistency of point placement along curves.
// Find position along curve through points0[i] of closest point
// to points1[i][pt_index].  Represent as t, where int(t)=index
// of first point on line segment, t-int(t)= relative distance
// along the segment.
// If there is a consistent placement, t will have small variance.
void analyze_pt_pos(const std::vector<msm_points>& points0,
                      const std::vector<msm_points>& points1,
                      const msm_curve& curve,
                      unsigned pt_index,
                      mbl_stats_1d& t_stats)
{
  t_stats.clear();
  unsigned n=points0.size();
  unsigned n_pts=points0[0].size();
  msm_line_seg_pt seg;
  double d2;
  for (unsigned i=0;i<n;++i)
  {
    seg = msm_closest_seg_pt_on_curve(points0[i],curve,
                                      points1[i][pt_index],d2);
    t_stats.obs(seg.i0 + seg.alpha);
  }
}

void test_revised_pt_pos(const std::vector<msm_points>& points0,
                      const std::vector<msm_points>& points1,
                      const msm_curve& curve,
                      unsigned pt_index, double new_pt_pos,
                      mbl_stats_1d& d_stats0, mbl_stats_1d& d_stats1)
{
  unsigned n=points0.size();
  unsigned n_pts=points0[0].size();
  unsigned i0=int(new_pt_pos);
  msm_line_seg_pt seg(i0,i0+1,new_pt_pos-i0);
  std::vector<vgl_point_2d<double> > pts;
  vgl_vector_2d<double> dp;
  for (unsigned i=0;i<n;++i)
  {
    // Distance between equivalent points on two curves.
    dp=points0[i][pt_index]-points1[i][pt_index];
    d_stats0.obs(dp.length());

    // Get interpolated point on points0
    points0[i].get_points(pts);
    dp=seg.point(pts)-points1[i][pt_index];
    d_stats1.obs(dp.length());
  }
}

//: Display stats for a given quantity
inline void print_summary(std::ostream& os, const mbl_sample_stats_1d& stats)
{
    os<<"mean: "<<std::setprecision(2)<<std::fixed<<stats.mean()
      <<" med: "<<stats.median()
      <<" 90%: "<<stats.quantile(0.90)
      <<" 95%: "<<stats.quantile(0.95)<<std::endl;
}

//: Display stats for a given quantity
inline void print_summary(std::ostream& os, const mbl_stats_1d& stats)
{
    os<<"mean: "<<std::setprecision(2)<<std::fixed<<stats.mean()
      <<" sd: "<<stats.sd()
      <<" ["<<stats.min()<<","<<stats.max()<<"]"<<std::endl;
}

void print_summaries(std::ostream& os, std::string key,
                     const std::vector<mbl_sample_stats_1d>& stats,
                     const std::vector<unsigned>& index)
{
  if (index.size()==0)
  {
    for (unsigned i=0;i<stats.size();++i)
    {
      os<<key<<i;
      if (i<10) os<<" ";
      os<<" ";
      print_summary(os,stats[i]);
    }
  }
  else
  {
    // Only print those listed in the index.
    for (unsigned i=0;i<index.size();++i)
    {
      if (index[i]>=stats.size())
      {
        std::cout<<"Warning "<<index[i]<<" outsize valid points."<<std::endl;
        continue;
      }
      os<<key<<index[i];
      if (index[i]<10) os<<" ";
      os<<" ";
      print_summary(os,stats[index[i]]);
    }
  }

}

//: Compares how consistently two sets of points are placed along curves.
// For instance, in one set point 33 might be placed at the position half-way
// between 33 and 34 as marked by the second annotator.
// Evaluates this position, then evaluates the Pt-Pt error if the points were
// slid along to match.
void test_consistency_along_curves(const std::vector<msm_points>& points0,
                      const std::vector<msm_points>& points1,
                      const msm_curve& curve,
                      const std::vector<unsigned>& pt_index,
                      std::ostream& os)
{
  mbl_stats_1d t_stats;
  mbl_stats_1d d_stats0_all,d_stats1_all;  // Pt-Pt distance before and after correction.

  for (unsigned i=0;i<pt_index.size();++i)
  {
    analyze_pt_pos(points0,points1,curve,pt_index[i],t_stats);
    os<<pt_index[i]<<" t_stats (Pt"<<pt_index[i]<<"): ";
    print_summary(os,t_stats);

    mbl_stats_1d d_stats0,d_stats1;  // Pt-Pt distance before and after correction.
    test_revised_pt_pos(points0,points1,curve,pt_index[i],
                          t_stats.mean(),d_stats0,d_stats1);
    os<<pt_index[i]<<" Pt-Pt: "<<d_stats0.mean()
      <<" Revised Pt-Pt: "<<d_stats1.mean()<<std::endl;

    d_stats0_all+=d_stats0;
    d_stats1_all+=d_stats1;
  }
  os<<std::endl;
  os<<"Overall Pt-Pt before: "<<d_stats0_all.mean()
    <<"  corrected: "<<d_stats1_all.mean()<<std::endl;
  os<<std::endl;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg<std::string> out_path("-o","Output summary path");
  vul_arg_parse(argc,argv);

  msm_add_all_loaders();

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

  if (out_path()!="")
    params.output_summary_path=out_path();

  msm_curves curves;
  if (!curves.read_text_file(params.curves_path))
  {
    std::cerr<<"Failed to load points from "<<params.curves_path<<std::endl;
    return 1;
  }
  std::cout<<"Loaded "<<curves.size()<<" curves from "<<params.curves_path<<std::endl;


  unsigned n_sets = params.points_dir.size();

  // Load in all points
  std::vector<std::vector<msm_points> > points(n_sets);
  for (unsigned i=0;i<n_sets;++i)
    load_shapes(params.points_dir[i],params.points_names,points[i]);

  std::cout<<"Loaded "<<points[0].size()
          <<" points files for each annotator."<<std::endl;

  unsigned n_pts=points[0][0].size();
  std::cout<<"Each file contains "<<n_pts<<" points."<<std::endl;

  std::ofstream ofs1(params.output_summary_path.c_str());
  if (!ofs1)
    std::cout<<"Failed to open "
             <<params.output_summary_path<<" for writing."<<std::endl;

  std::ofstream ofs2(params.output_consistency_path.c_str());
  if (!ofs2)
    std::cout<<"Failed to open "
             <<params.output_consistency_path<<" for writing."<<std::endl;

  std::vector<mbl_sample_stats_1d> total_p2p_stats(n_pts),total_p2c_stats(n_pts);

  for (unsigned i0=0;i0+1<n_sets;++i0)
    for (unsigned i1=i0+1;i1<n_sets;++i1)
    {
      if (params.key_text.size()>i1)
        std::cout<<"Comparing "<<params.key_text[i0]
                 <<" with "<<params.key_text[i1]<<std::endl;
      else
        std::cout<<"Comparing set "<<i0<<" with "<<i1<<std::endl;

      std::vector<mbl_sample_stats_1d> p2p_stats,p2c_stats;
      mbl_sample_stats_1d all_p2p_stats,all_p2c_stats;

      compare_two_sets(points[i0],points[i1],curves,
                       p2p_stats,all_p2p_stats,
                       p2c_stats,all_p2c_stats);

      std::cout<<"  Pt-Pt  distance error: ";
      print_summary(std::cout,all_p2p_stats);
      std::cout<<"  Pt-Crv distance error: ";
      print_summary(std::cout,all_p2c_stats);

      if (params.key_text.size()>i1)
        ofs2<<"=== Comparing "<<params.key_text[i0]
                 <<" with "<<params.key_text[i1]<<" ==="<<std::endl;
      else
        ofs2<<"=== Comparing set "<<i0<<" with "<<i1<<" ==="<<std::endl;
      test_consistency_along_curves(points[i0],points[i1],curves[0],params.points_to_show,ofs2);


      for (unsigned j=0;j<n_pts;++j)
      {
        total_p2p_stats[j]+=p2p_stats[j];
        total_p2c_stats[j]+=p2c_stats[j];
      }

      if (params.key_text.size()>i1)
        ofs1<<"Comparing "<<params.key_text[i0]
                 <<" with "<<params.key_text[i1]<<std::endl;
      else
        ofs1<<"Comparing set "<<i0<<" with "<<i1<<std::endl;

      ofs1<<"  Pt-Pt  distance error: ";
      print_summary(ofs1,all_p2p_stats);
      ofs1<<"  Pt-Crv distance error: ";
      print_summary(ofs1,all_p2c_stats);

      ofs1<<"Breakdown by points"<<std::endl;
      print_summaries(ofs1,"Pt-Pt  ",p2p_stats,params.points_to_show);
      ofs1<<std::endl;
      print_summaries(ofs1,"Pt-Crv ",p2c_stats,params.points_to_show);
      ofs1<<std::endl;
    }

  ofs1<<"=== Average over all pair-wise comparisons ==="<<std::endl;
  print_summaries(ofs1,"Pt-Pt  ",total_p2p_stats,params.points_to_show);
  ofs1<<std::endl;
  print_summaries(ofs1,"Pt-Crv ",total_p2c_stats,params.points_to_show);
  ofs1<<std::endl;


  ofs1.close();
  std::cout<<"Results saved to "<<params.output_summary_path<<std::endl;

  ofs2.close();
  std::cout<<"Results (consistency) saved to "<<params.output_consistency_path<<std::endl;
  return 0;
}
