// This is brl/bseg/sdet/sdet_grid_finder.h
#ifndef sdet_grid_finder_h_
#define sdet_grid_finder_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for finding a grid of orthogonal lines in an image
//
//  Given a grid with squares of known spacing, the homography that best
//  maps the grid into the image is found by grouping line segments detected
//  in the image to form the vanishing points for the line segments. An
//  approximate homography is established to enable a search for the best
//  fit of the grid in the image. A final homography is computed using all
//  the line-to-line correspondences in a least squares solution
//
// \author
//  J.L. Mundy - April 10, 2003
//
// \verbatim
//  Modifications
//   DEC 9/3/2003 - replaced line_chamfer_1d with grid_profile_matcher
//                - added functions to output homographies to file
//                - added function to check homography with original image
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <bsol/bsol_hough_line_index_sptr.h>
#include <sdet/sdet_grid_finder_params.h>
#include <vil1/vil1_image.h>

//this helper class does a distance transform on the perpendicular
//distance of parallel lines from the origin (-l.c());
//offsets into the distance array corresponding to each line
//are provided.

#if 0
class line_chamfer_1d
{
 public:
  line_chamfer_1d();
  ~line_chamfer_1d();

  bool insert_lines(vcl_vector<vsol_line_2d_sptr> const& lines);

  bool get_lines_in_interval(const double dlo, const double dhi,
                             vcl_vector<vsol_line_2d_sptr>& lines) const;
  int index_size() const {return size_;}
  int n_lines() const {return distances_.size();}
  double offset(int i) const {return distances_[i];}
  double distance(double x) const;
  void forward_champher();
  void backward_champher();
 private:
  vcl_vector<int> index_;
  vcl_vector<double> distances_;
  vcl_vector<vcl_vector<vsol_line_2d_sptr>* > line_index_;
  int size_;
  double dmin_;
  double dmax_;
};
#endif

class grid_profile_matcher
{
 public:
  grid_profile_matcher();
  ~grid_profile_matcher();
  bool insert_lines(vcl_vector<vsol_line_2d_sptr> const& lines,
                    bool horizontal_lines);
  bool get_lines_in_interval(const double dlo, const double dhi,
                             vcl_vector<vsol_line_2d_sptr>& lines) const;
  double calculate_grid_offset(int n_lines, double spacing);
  int index_size() const {return size_;}
  int n_lines() const {return distances_.size();}
  double offset(int i) const {return distances_[i];}
  double distance(double x) const;
 private:
  vnl_vector<double> image_profile_;
  vcl_vector<double> distances_;
  vcl_vector<vcl_vector<vsol_line_2d_sptr>* > line_index_;
  int size_;
  double dmin_;
  double dmax_;
};


class sdet_grid_finder : public sdet_grid_finder_params
{
 public:
  // Constructors/destructor
  sdet_grid_finder(sdet_grid_finder_params& gfp);

  ~sdet_grid_finder();

  // Process methods
  bool match_grid();
  bool compute_homography();
  bool compute_manual_homography(vsol_point_2d_sptr ul,
                                 vsol_point_2d_sptr ur,
                                 vsol_point_2d_sptr lr,
                                 vsol_point_2d_sptr ll);
  void clear();

  // Accessors

  //: if there are less than 2 dominant groups then return false
  bool set_lines(const float xsize, const float ysize,
                 vcl_vector<vsol_line_2d_sptr> const& lines);

  bool get_homography(vgl_h_matrix_2d<double>& homog);

  bool get_debug_lines(vcl_vector<vsol_line_2d_sptr> & lines);
  bool get_debug_grid_lines(vcl_vector<vsol_line_2d_sptr> & lines);

  bool get_affine_lines(vcl_vector<vsol_line_2d_sptr> & lines);
  bool get_matched_lines(vcl_vector<vsol_line_2d_sptr> & lines);
  bool get_mapped_lines(vcl_vector<vsol_line_2d_sptr> & lines);

  bool get_backprojected_grid(vcl_vector<vsol_line_2d_sptr> & lines);
  void set_verbose() { verbose_=true; } //non-params interface
  void unset_verbose() { verbose_=false; }


  //:test camera parameter matrices
  bool transform_grid_points(vnl_matrix_fixed<double,3,3> & K,
                             vnl_matrix_fixed<double,3,4> & M,
                             vcl_vector<vsol_point_2d_sptr> & points);
  //:write transformed grid points to a file
  bool init_output_file(vcl_ofstream & outstream);
  bool write_image_points(vcl_ofstream & outstream);

  //:make sure homography and image correspond with each other
  bool check_grid_match(vil1_image img);

 protected:
  // protected methods


  //:transform a vsol line by transforming the end points
  vsol_line_2d_sptr transform_line(vgl_h_matrix_2d<double> const& h,
                                   vsol_line_2d_sptr const & l);

  //:the vanishing point of a line bundle

  bool get_vanishing_point(vcl_vector<vsol_line_2d_sptr> const & para_lines,
                           vgl_homg_point_2d<double>& vp);

  bool scale_transform(const double max_distance,
                       vcl_vector<vsol_line_2d_sptr> const& gh,
                       vcl_vector<vsol_line_2d_sptr> const& gv,
                       vnl_matrix_fixed<double, 3, 3>& S);


  //:vanishing points of the grid lines
  bool compute_vanishing_points();
  bool compute_projective_homography();
  bool compute_affine_homography();
  bool compute_homography_linear_chamfer(vgl_h_matrix_2d<double> & H);

  // for checking homography match with original image
  bool get_square_pixel_stats(vil1_image img,
                              int x,int y,
                              double & mean_intensity,
                              double & intensity_sigma);

  //:members
  bool groups_valid_;
  bool vanishing_points_valid_;
  bool projective_homography_valid_;      //process state flag
  bool affine_homography_valid_;      //process state flag
  bool homography_valid_;      //process state flag
  float xmax_;
  float ymax_;
  vcl_vector<vsol_line_2d_sptr> lines_;
  vcl_vector<vsol_line_2d_sptr> display_lines_;
  vcl_vector<vsol_line_2d_sptr> matched_lines_;
  vcl_vector<vsol_line_2d_sptr> debug_lines_;
  vcl_vector<vsol_line_2d_sptr> debug_grid_lines_;
  vcl_vector<vsol_line_2d_sptr> group0_;
  vcl_vector<vsol_line_2d_sptr> group1_;
  vcl_vector<vsol_line_2d_sptr> afgroup0_;
  vcl_vector<vsol_line_2d_sptr> afgroup1_;
  bsol_hough_line_index_sptr index_;
  //line_chamfer_1d chamf0_;
  //line_chamfer_1d chamf90_;
  grid_profile_matcher chamf0_;
  grid_profile_matcher chamf90_;
  vgl_homg_point_2d<double> vp0_;
  vgl_homg_point_2d<double> vp90_;
  vgl_h_matrix_2d<double> projective_homography_;
  vgl_h_matrix_2d<double> affine_homography_;
  vgl_h_matrix_2d<double> homography_;
};

#endif // sdet_grid_finder_h_
