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

  bool get_mapped_lines(vcl_vector<vsol_line_2d_sptr> & lines);
  bool get_backprojected_grid(vcl_vector<vsol_line_2d_sptr> & lines);
  void set_verbose(){verbose_=true;}
  void unset_verbose(){verbose_=false;}

 protected:
  // protected methods

  //:transform a vsol line by transforming the end points
  vsol_line_2d_sptr transform_line(vgl_h_matrix_2d<double> const& h,
                                   vsol_line_2d_sptr const & l);

  //:the vanishing point of a line bundle
  vgl_homg_point_2d<double>
  get_vanishing_point(vcl_vector<vsol_line_2d_sptr> const & para_lines);

  //:vanishing points of the grid lines
  void compute_vanishing_points();
  bool compute_projective_homography();
  bool compute_affine_homography();
  bool compute_homography_linear(vgl_h_matrix_2d<double> & H);

  // members
  bool verbose_;
  bool groups_valid_;
  bool vanishing_points_valid_;
  bool projective_homography_valid_;      //process state flag
  bool affine_homography_valid_;      //process state flag
  bool homography_valid_;      //process state flag
  float xmax_;
  float ymax_;
  vcl_vector<vsol_line_2d_sptr> lines_;
  vcl_vector<vsol_line_2d_sptr> group0_;
  vcl_vector<vsol_line_2d_sptr> group1_;
  vcl_vector<vsol_line_2d_sptr> afgroup0_;
  vcl_vector<vsol_line_2d_sptr> afgroup1_;
  bsol_hough_line_index_sptr index_;
  vcl_vector<vcl_vector<vsol_line_2d_sptr> > dindex0_;
  vcl_vector<vcl_vector<vsol_line_2d_sptr> > dindex90_;
  vgl_homg_point_2d<double> vp0_;
  vgl_homg_point_2d<double> vp90_;
  vgl_h_matrix_2d<double> projective_homography_;
  vgl_h_matrix_2d<double> affine_homography_;
  vgl_h_matrix_2d<double> homography_;
};

#endif // sdet_grid_finder_h_
