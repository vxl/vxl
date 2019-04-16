//:
// \file
// \brief Tool to compare point annotations from two or more sources.
// \author Tim Cootes
// Reads in multiple annotations of the same set of images.
// Compares the points from different annotators.
// Estimates the mean and covar. of the difference between annotators
// for each point.
// Creates an image to visualise the differences, superimposed on
// the first image.

#include <sstream>
#include <iostream>
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

#include <msm/msm_aligner.h>
#include <msm/msm_add_all_loaders.h>
#include <mbl/mbl_stats_nd.h>
#include <mbl/mbl_sample_stats_1d.h>
#include <msm/utils/msm_draw_shape_to_eps.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_load.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>

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

// Sequence of colours to use for the points
point_colours:  { red blue }

// Colour used for summary of all annotations
summary_colour: green

// Text for key (shown in top left of image)
key_text: { Marker1   Marker2 }

//: Aligner used to map to common frame
aligner: msm_similarity_aligner

//: Radius of points to display
point_radius: 1.5

//: Line width
line_width: 1.5

//: If greater than one, show points i*point_step
// Enables display of subset of the points
point_step: 2

//: When true, use iterative algorithm
//  This re-estimates offsets and taking account of uncertainties
// WARNING: Experimental.  I'm unconvinced that this is doing the right thing.
// May be missing a constraint, leading to odd solutions.
use_iterations: false

//: Path for image+ellipses, one for every annotation set.
output_all_path: image+pts_all.eps

//: Path for image+ellipses, one ellipse per point summarising all sets.
output_summary_path: image+pts_sum.eps

// If provided, generate one image + markup for each annotation set.
// Name given by output_for_one_base_path+key_text[i].eps
output_for_one_base_path: image+pts_

//: When defined, only display listed points
points_to_show: { 3 7 12 15 }

image_dir: /home/images/
images: {
  image1.pts : image1.jpg
  image2.pts : image2.jpg
}

<END FILE>
*/

void print_usage()
{
  std::cout << "msm_compare_markups -p param_file\n"
           << "Analyse differences between points from two annotators.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Aligner for shape model
  std::unique_ptr<msm_aligner> aligner;

  //: Radius of points to display
  double point_radius;

  //: Line width
  double line_width;

  //: If greater than one, show points i*point_step
  // Enables display of subset of the points
  unsigned point_step;

  //: When defined, only display listed points
  std::vector<unsigned> points_to_show;

  //: When true, use iterative algorithm
  //  This re-estimates offsets and taking account of uncertainties
  bool use_iterations;

  //: Directory containing images
  std::string image_dir;

  //: Directories containing points
  std::vector<std::string> points_dir;

  //: Colours for points (and key text)
  std::vector<std::string> point_colour;

  //: Colour used for summary of all annotations
  std::string summary_colour;

  //: Text for key
  std::vector<std::string> key_text;

  //: Path for image+ellipses, one for every annotation set.
  std::string output_all_path;

  //: Path for image+ellipses, one ellipse per point summarising all sets.
  std::string output_summary_path;

  // If provided, generate one image + markup for each annotation set.
  // Name given by output_for_one_base_path+key_text[i].eps
  std::string output_for_one_base_path;


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

  output_all_path=props.get_optional_property("output_all_path","image+pts_all.eps");
  output_summary_path=props.get_optional_property("output_summary_path",
                                                  "image+pts_sum.eps");
  output_for_one_base_path=props.get_optional_property("output_for_one_base_path","");

  summary_colour=props.get_optional_property("summary_colour","green");

  use_iterations=vul_string_to_bool(props.get_optional_property("use_iterations","false"));

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

  std::string point_colours_str=props.get_optional_property("point_colours","");
  if (point_colours_str!="")
    mbl_parse_string_list(point_colours_str,point_colour);
  else
  {
    point_colour.resize(2);
    point_colour[0]=props.get_required_property("point_colour1");
    point_colour[1]=props.get_required_property("point_colour2");
  }

  if (points_dir.size()!=point_colour.size())
    std::cout<<"WARNING: "
      <<"Number of colours does not match number of data sets"<<std::endl;

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

  point_radius=vul_string_atof(props.get_optional_property("point_radius","1.5"));
  line_width=vul_string_atof(props.get_optional_property("line_width","1"));
  point_step=vul_string_atoi(props.get_optional_property("point_step","1"));

  {
    std::string aligner_str
       = props.get_required_property("aligner");
    std::stringstream ss(aligner_str);
    aligner = msm_aligner::create_from_stream(ss);
  }

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



// Draw an axis aligned ellipse centred at c, with radii rx, ry
// Assumes current scaling is zero - otherwise need sx,sy from the writer
void draw_axis_ellipse(std::ofstream& ofs, double h, vgl_point_2d<double> c, double rx, double ry)
{
  ofs<<"gsave\n";
  ofs<<"newpath "<<c.x()<<" "<<h-c.y()<<" translate 1 "<<ry<<" "<<rx<<" div  scale ";
  ofs<<"0 0 "<<rx<<" 0 360 arc\n";
  ofs<<"1 "<<rx<<" "<<ry<<" div scale stroke\n";
  ofs<<"grestore\n";
}

// Draw an ellipse centred at c, with radii rx, ry, rotated by angle A
// Assumes current scaling is zero - otherwise need sx,sy from the writer
void draw_ellipse(std::ofstream& ofs, double h, vgl_point_2d<double> c, double rx, double ry, double A)
{
  // Note: Rotate by -A, because we flip y coordinates
  ofs<<"gsave\n";
  ofs<<"newpath "<<c.x()<<" "<<h-c.y()<<" translate "<<-A<<" rotate 1 "<<ry<<" "<<rx<<" div  scale ";
  ofs<<"0 0 "<<rx<<" 0 360 arc\n";
  ofs<<"1 "<<rx<<" "<<ry<<" div scale stroke\n";
  ofs<<"grestore\n";
}

//: Compute running estimate of covariance matrix
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

  //: Return inverse of covariance matrix (assumes |det()|>0)
  vnl_matrix_fixed<double,2,2> inv_covar() const
  {
    double d=det(); if (abs(d)<0.0001) d=0.0001;
    vnl_matrix_fixed<double,2,2> S_inv;
    S_inv[0][0]=var22()/d;   S_inv[1][0]=-var12()/d;
    S_inv[0][1]=S_inv[1][0]; S_inv[1][1]=var11()/d;
    return S_inv;
  }

};

void write_ellipses(mbl_eps_writer& writer, double region_height,
                    const msm_points& ref_shape, double point_radius,
                    const std::vector<mbm_covar_stats_2d>& pt_stats,
                    unsigned step=1)
{
  for (unsigned k=0;k<pt_stats.size();k+=step)
  {
    // Draw mean point
    vgl_point_2d<double> pt(ref_shape[k].x()+pt_stats[k].mean_x(),
                            ref_shape[k].y()+pt_stats[k].mean_y());

    writer.draw_disk(pt,point_radius);

    double rx,ry,A;
    pt_stats[k].eigen_values(rx,ry,A);
    draw_ellipse(writer.ofs(),region_height,pt,
                 2*std::sqrt(rx),2*std::sqrt(ry),A*180/3.14159);
  }
}

// Write ellipses for subset of points
void write_ellipses(mbl_eps_writer& writer, double region_height,
                    const msm_points& ref_shape, double point_radius,
                    const std::vector<mbm_covar_stats_2d>& pt_stats,
                    const std::vector<unsigned>& subset)
{
  for (unsigned ik=0;ik<subset.size();ik++)
  {
    unsigned k=subset[ik];

    // Draw mean point
    vgl_point_2d<double> pt(ref_shape[k].x()+pt_stats[k].mean_x(),
                            ref_shape[k].y()+pt_stats[k].mean_y());

    writer.draw_disk(pt,point_radius);

    double rx,ry,A;
    pt_stats[k].eigen_values(rx,ry,A);
    draw_ellipse(writer.ofs(),region_height,pt,
                 2*std::sqrt(rx),2*std::sqrt(ry),A*180/3.14159);
  }
}

void write_ellipses(mbl_eps_writer& writer, double region_height,
                    const msm_points& ref_shape, double point_radius,
                    const std::vector<mbm_covar_stats_2d>& pt_stats,
                    const std::vector<unsigned>& subset,
                    unsigned point_step)
{
  if (subset.size()>0)
    write_ellipses(writer,region_height,ref_shape,point_radius,pt_stats,subset);
  else
    write_ellipses(writer,region_height,ref_shape,point_radius,pt_stats,point_step);
}



// Draw disk at each point, at ref_shape[k]+pt_stats[k].mean
void write_centre_points(mbl_eps_writer& writer, double region_height,
                    const msm_points& ref_shape, double point_radius,
                    const std::vector<mbm_covar_stats_2d>& pt_stats,
                    unsigned step=1)
{
  for (unsigned k=0;k<pt_stats.size();k+=step)
  {
    // Draw mean point
    vgl_point_2d<double> pt(ref_shape[k].x()+pt_stats[k].mean_x(),
                            ref_shape[k].y()+pt_stats[k].mean_y());

    writer.draw_disk(pt,point_radius);
  }
}


// Draw disk at each point, at ref_shape[k]+pt_stats[k].mean
void write_centre_points(mbl_eps_writer& writer, double region_height,
                    const msm_points& ref_shape, double point_radius,
                    const std::vector<mbm_covar_stats_2d>& pt_stats,
                    const std::vector<unsigned>& subset)
{
  for (unsigned ik=0;ik<subset.size();ik++)
  {
    unsigned k=subset[ik];

    // Draw mean point
    vgl_point_2d<double> pt(ref_shape[k].x()+pt_stats[k].mean_x(),
                            ref_shape[k].y()+pt_stats[k].mean_y());

    writer.draw_disk(pt,point_radius);
  }
}


//: Given estimates of point on multiple images from >1 marker, estimate true pos
// Input: p[i][j] point produced by annotator i on image j
// Output:
//   true_p[j]  Estimate of true position for point on image j
//   stats[i]   Mean (offset) and covariance of predictions of points for i
void compute_stats(const std::vector<std::vector<vgl_point_2d<double> > >& p,
                         std::vector<vgl_point_2d<double> >& true_p,
                         std::vector<mbm_covar_stats_2d>& stats)
{
  unsigned n_sets=p.size();
  unsigned n=p[0].size();
  true_p.resize(n);
  stats.resize(n_sets);

  std::vector<mbm_covar_stats_2d> c_stats(n);

  // First estimate mean for each point
  for (unsigned j=0;j<n;++j)
  {
    for (unsigned i=0;i<n_sets;++i) c_stats[j].obs(p[i][j]);
    true_p[j]=vgl_point_2d<double>(c_stats[j].mean_x(),c_stats[j].mean_y());
  }

//  std::cout<<"Estimate of pt 0: ("<<true_p[0].x()<<","<<true_p[0].y()<<")"<<std::endl;

  // Estimate stats for each individual
  for (unsigned i=0;i<n_sets;++i)
  {
    stats[i]=mbm_covar_stats_2d();  // Empty
    for (unsigned j=0;j<n;++j)
      stats[i].obs(p[i][j]-true_p[j]);
  }

  // Now re-estimate the true position, the offset and covar per marker.

  unsigned n_its=13;
  for (unsigned its=0;its<n_its;++its)
  {
    // Re-estimate the true position.
    for (unsigned j=0;j<n;++j)
    {
      vnl_matrix_fixed<double,2,2> W_sum;
      // First add constraint on true_p from prior
      W_sum=c_stats[j].inv_covar();
      double x_sum=W_sum[0][0]*c_stats[j].mean_x()+
                   W_sum[0][1]*c_stats[j].mean_y();
      double y_sum=W_sum[1][0]*c_stats[j].mean_x()+
                   W_sum[1][1]*c_stats[j].mean_y();
      // Prediction from each set
      for (unsigned i=0;i<n_sets;++i)
      {
        double dx=p[i][j].x()-stats[i].mean_x();
        double dy=p[i][j].y()-stats[i].mean_y();

        vnl_matrix_fixed<double,2,2> W=stats[i].inv_covar();
        x_sum += W[0][0]*dx + W[0][1]*dy;
        y_sum += W[1][0]*dx + W[1][1]*dy;
        W_sum+=W;
      }

      vnl_matrix_fixed<double,2,2> W_sum_inv=vnl_inverse(W_sum);
      double mx=W_sum_inv[0][0]*x_sum + W_sum_inv[0][1]*y_sum;
      double my=W_sum_inv[1][0]*x_sum + W_sum_inv[1][1]*y_sum;
      true_p[j]=vgl_point_2d<double>(mx,my);
    }
  std::cout<<"Estimate of pt 0: ("<<true_p[0].x()<<","<<true_p[0].y()
          <<") var_x[0]: "<<stats[0].var11()
          <<" var_x[1]: "<<stats[1].var11()<<std::endl;

    // Estimate stats for each individual
    for (unsigned i=0;i<n_sets;++i)
    {
      stats[i]=mbm_covar_stats_2d();  // Empty
      for (unsigned j=0;j<n;++j)
        stats[i].obs(p[i][j]-true_p[j]);
    }
  }

}


int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg<std::string> out_path("-o","Output path for (eps) image","image+pts.eps");
  vul_arg<std::string> d_stats_path("-d","Output path distance statistics","d_stats.txt");
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
    params.output_all_path=out_path();

  unsigned n_sets = params.points_dir.size();

  // Load in all points
  std::vector<std::vector<msm_points> > points(n_sets);
  for (unsigned i=0;i<n_sets;++i)
    load_shapes(params.points_dir[i],params.points_names,points[i]);

  // Compute mean of points on first image to use as a reference.
  msm_points ref_shape=mean_shape(points,0);

  // Transform all points into a common reference frame
  // Compute mean of points on each image, then transform all by same function.
  unsigned n_egs=points[0].size();
  vnl_vector<double> pose;
  for (unsigned j=1;j<n_egs;++j)
  {
    msm_points mean=mean_shape(points,j);
    params.aligner->calc_transform(mean,ref_shape,pose);
    for (unsigned i=0;i<n_sets;++i)
      params.aligner->apply_transform(points[i][j],pose,points[i][j]);
  }

  unsigned n_pts = points[0][0].size();
  std::vector<std::vector<mbm_covar_stats_2d> > pt_stats(n_sets);
  std::vector<mbm_covar_stats_2d> pt_stats_all(n_pts);  // For all annotations
  std::vector<std::vector<mbl_sample_stats_1d> > d_stats(n_sets);  // Dist. to mean.
  for (unsigned i=0;i<pt_stats.size();++i)
  {
    pt_stats[i].resize(n_pts);
    d_stats[i].resize(n_pts);
  }

  for (unsigned j=0;j<n_egs;++j)
  {
    msm_points mean=mean_shape(points,j);
    for (unsigned i=0;i<n_sets;++i)
    {
      msm_points dpoints;
      dpoints.vector()=points[i][j].vector()-mean.vector();
      for (unsigned k=0;k<n_pts;++k)
      {
        pt_stats[i][k].obs(dpoints[k]);
        pt_stats_all[k].obs(dpoints[k]);  // Record deviation for all markers
        double d2=dpoints[k].x()*dpoints[k].x() + dpoints[k].y()*dpoints[k].y();
        d_stats[i][k].add_sample(std::sqrt(d2));
      }
    }
  }

  // Experimental:
  // Use iterative refinement of estimates,
  // taking marker uncertainty into account
  if (params.use_iterations)
  {
    std::cout<<"Using iterative approach to estimate mean positions and uncertainties"<<std::endl;
    std::cout<<"WARNING: This may not work properly - it may converge to odd solutions."<<std::endl;
    for (unsigned k=0;k<n_pts;++k)
    {
      // Perform calculations for point k
      std::vector<std::vector<vgl_point_2d<double> > > p(n_sets);
      std::vector<vgl_point_2d<double> > true_p;
      std::vector<mbm_covar_stats_2d> stats;
      for (unsigned i=0;i<n_sets;++i)
      {
        p[i].resize(n_egs);
        for (unsigned j=0;j<n_egs;++j)
          p[i][j]=points[i][j][k];
      }
      compute_stats(p,true_p,stats);

      // Record results for point k
      for (unsigned i=0;i<n_sets;++i)
        pt_stats[i][k]=stats[i];
      ref_shape.set_point(k,true_p[0]);
    }
  }

  // Create an image with mean point positions overlayed
  vimt_image_2d_of<vxl_byte> image;
  std::string image_path = params.image_dir+"/"+params.image_names[0];
  vimt_load_to_byte(image_path.c_str(), image, 1000.0f);

  vimt_transform_2d i2w=image.world2im().inverse();
  double pixel_width_i = (i2w(1,0)-i2w(0,0)).length();
  double pixel_width_j = (i2w(0,1)-i2w(0,0)).length();

  double region_width=pixel_width_i*image.image().ni();
  double region_height=pixel_width_j*image.image().nj();


  mbl_eps_writer writer(params.output_all_path.c_str(),region_width,region_height);

  writer.draw_image(image.image(),0,0, pixel_width_i,pixel_width_j);

  writer.set_colour("cyan");
  writer.set_line_width(params.line_width);

  // Draw lines connecting mean points
  for (unsigned k=0;k<n_pts;++k)
  {
    for (unsigned i=0;i<n_sets;++i)
    {
      vgl_point_2d<double> pt1(ref_shape[k].x()+pt_stats[i][k].mean_x(),
                               ref_shape[k].y()+pt_stats[i][k].mean_y());
      unsigned i1=(i+1)%n_sets;
      vgl_point_2d<double> pt2(ref_shape[k].x()+pt_stats[i1][k].mean_x(),
                               ref_shape[k].y()+pt_stats[i1][k].mean_y());
      if(n_sets>2 || i<2)  // Don't draw line twice in n_sets==2 case
        writer.draw_line(pt1,pt2);
    }
  }

  unsigned n_colours=params.point_colour.size();

  vgl_point_2d<double> key_p(5,5);
  double key_step=15;

  for (unsigned i=0;i<n_sets;++i)
  {
    for (unsigned k=0;k<n_pts;k+=params.point_step)
    {
      // Draw mean point
      vgl_point_2d<double> pt(ref_shape[k].x()+pt_stats[i][k].mean_x(),
                              ref_shape[k].y()+pt_stats[i][k].mean_y());
      writer.set_colour(params.point_colour[i%n_colours]);
      writer.draw_disk(pt,params.point_radius);

      double rx,ry,A;
      pt_stats[i][k].eigen_values(rx,ry,A);
      draw_ellipse(writer.ofs(),region_height,pt,2*std::sqrt(rx),2*std::sqrt(ry),A*180/3.14159);
    }

    // Write text for key (using same colour)
    if (i<params.key_text.size())
      writer.write_text(key_p.x(),key_p.y()+i*key_step,12,params.key_text[i]);
  }

  writer.close();
  std::cout<<"Graphics saved to "<<params.output_all_path<<std::endl;

  // Write summary file with one ellipse per point indicating spread
  mbl_eps_writer writer2(params.output_summary_path.c_str(),region_width,region_height);
  writer2.draw_image(image.image(),0,0, pixel_width_i,pixel_width_j);
  writer2.set_line_width(params.line_width);
  writer2.set_colour(params.summary_colour);

  write_ellipses(writer2,region_height,
                 ref_shape,params.point_radius,
                 pt_stats_all,params.points_to_show,
                 params.point_step);

  // Write points for mean displacement for each individual annotator
  for (unsigned i=0;i<n_sets;++i)
  {
    writer2.set_colour(params.point_colour[i%n_colours]);
    if (params.points_to_show.size()==0)
      write_centre_points(writer2,region_height,
                   ref_shape,params.point_radius,
                   pt_stats[i],params.point_step);
    else
      write_centre_points(writer2,region_height,
                   ref_shape,params.point_radius,
                   pt_stats[i],params.points_to_show);

  }
  writer2.close();
  std::cout<<"Graphic summarising all variance saved to "
           <<params.output_summary_path<<std::endl;

  // Create one graphic per annotator, showing their result and the average.
  for (unsigned i=0;i<params.key_text.size();++i)
  {
    std::string path=params.output_for_one_base_path+params.key_text[i]+".eps";
    mbl_eps_writer writer3(path.c_str(),region_width,region_height);
    writer3.draw_image(image.image(),0,0, pixel_width_i,pixel_width_j);
    writer3.set_line_width(params.line_width);
    writer3.set_colour(params.point_colour[0]);
    write_ellipses(writer3,region_height,
                  ref_shape,params.point_radius,
                  pt_stats_all,params.points_to_show,params.point_step);
    writer3.write_text(key_p.x(),key_p.y(),12,"All");

    writer3.set_colour(params.point_colour[1]);
    write_ellipses(writer3,region_height,
                  ref_shape,params.point_radius,
                  pt_stats[i],params.points_to_show,params.point_step);
    writer3.write_text(key_p.x(),key_p.y()+key_step,12,params.key_text[i]);
    writer3.close();
    std::cout<<"Graphic for one annotator saved to "<<path<<std::endl;
  }


  if (n_sets==2)
  {
    // Write distance stats to file.
    // Double values, to get distance between points, rather than from points to means.
    std::ofstream ofs(d_stats_path().c_str());
    if (!ofs)
    {
      std::cout<<"Unable to open "<<d_stats_path()<<" for writing."<<std::endl;
      return 2;
    }
    for (unsigned k=0;k<n_pts;++k)
    {
      ofs<<k<<" "<<2*d_stats[0][k].mean()
         <<" "<<2*d_stats[0][k].sd()
         <<" "<<2*d_stats[0][k].median()
         <<" "<<2*d_stats[0][k].quantile(0.95)<<std::endl;
    }
    ofs.close();
    std::cout<<"Saved pt_index, mean, SD, median, 95% to "<<d_stats_path()<<std::endl;
  }
  else
  {
    // Write distance stats for each example to file.
    std::ofstream ofs(d_stats_path().c_str());
    if (!ofs)
    {
      std::cout<<"Unable to open "<<d_stats_path()<<" for writing."<<std::endl;
      return 2;
    }
    for (unsigned i=0;i<n_sets;++i)
    {
      for (unsigned k=0;k<n_pts;++k)
      {
        ofs<<i<<" "<<k<<" "<<d_stats[0][k].mean()
          <<" "<<d_stats[0][k].sd()
          <<" "<<d_stats[0][k].median()
          <<" "<<d_stats[0][k].quantile(0.95)<<std::endl;
      }
    }
    ofs.close();
    std::cout<<"Saved set_index, pt_index, mean, SD, median, 95% to "<<d_stats_path()<<std::endl;
  }

  return 0;
}
