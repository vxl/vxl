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
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <bsol/bsol_hough_line_index_sptr.h>
#include <sdet/sdet_grid_finder_params.h>

//this helper class does a distance transform on the perpendicular
//distance of parallel lines from the origin (-l.c());
//offsets into the distance array corresponding to each line
//are provided.
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

class sdet_grid_finder : public sdet_grid_finder_params
{
 public:
  // Constructors/destructor
  sdet_grid_finder(sdet_grid_finder_params& gfp);

  ~sdet_grid_finder();
  // Process methods
  bool match_grid();
  bool compute_homography();
  void clear();

  // Accessors

  //: if there are less than 2 dominant groups then return false
  bool set_lines(const float xsize, const float ysize,
                 vcl_vector<vsol_line_2d_sptr> const& lines);

  bool get_homography(vgl_h_matrix_2d<double>& homog);
  bool get_debug_lines(vcl_vector<vsol_line_2d_sptr> & lines);
  bool get_affine_lines(vcl_vector<vsol_line_2d_sptr> & lines);
  bool get_matched_lines(vcl_vector<vsol_line_2d_sptr> & lines);
  bool get_mapped_lines(vcl_vector<vsol_line_2d_sptr> & lines);

  bool get_backprojected_grid(vcl_vector<vsol_line_2d_sptr> & lines);
  void set_verbose(){verbose_=true;}//non-params interface
  void unset_verbose(){verbose_=false;}

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

  bool find_translation(line_chamfer_1d const& chamf, int ngrid,
                        double& trans);

  //:vanishing points of the grid lines
  bool compute_vanishing_points();
  bool compute_projective_homography();
  bool compute_affine_homography();
  bool compute_homography_linear_chamfer(vgl_h_matrix_2d<double> & H);


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
  vcl_vector<vsol_line_2d_sptr> group0_;
  vcl_vector<vsol_line_2d_sptr> group1_;
  vcl_vector<vsol_line_2d_sptr> afgroup0_;
  vcl_vector<vsol_line_2d_sptr> afgroup1_;
  bsol_hough_line_index_sptr index_;
  line_chamfer_1d chamf0_;
  line_chamfer_1d chamf90_;
  vgl_homg_point_2d<double> vp0_;
  vgl_homg_point_2d<double> vp90_;
  vgl_h_matrix_2d<double> projective_homography_;
  vgl_h_matrix_2d<double> affine_homography_;
  vgl_h_matrix_2d<double> homography_;
};

#endif // sdet_grid_finder_h_
