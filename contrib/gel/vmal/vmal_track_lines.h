// This is gel/vmal/vmal_track_lines.h
#ifndef vmal_track_lines_h_
#define vmal_track_lines_h_
//--------------------------------------------------------------------------------
//:
//  \file
//
//   Match lines that have been detected in a sequence of images. To achieve
//   this, it uses the homographies between the images.
//
// \author
//   L. Guichard
// \verbatim
// Modifications:
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vxl_config.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vmal/vmal_multi_view_data_edge_sptr.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>

#include <vnl/vnl_double_3x3.h>

class vmal_track_lines
{
 public:

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor.
//---------------------------------------------------------------------------
  vmal_track_lines();
//---------------------------------------------------------------------------
//: Destructor.
//---------------------------------------------------------------------------
  virtual ~vmal_track_lines();

//---------------------------------------------------------------------------
//: Main function that computes the matches between lines in the images.
//---------------------------------------------------------------------------
  void track_lines(const vcl_vector<vcl_vector<vtol_edge_2d_sptr>*>* fit_lines,
                   const vcl_vector<vcl_vector<vtol_edge_2d_sptr>*>* transformed_lines,
                   const vcl_vector<vil1_image> &images, const vcl_vector<vnl_double_3x3> &homo,
                   vmal_multi_view_data_edge_sptr matches);

//---------------------------------------------------------------------------
//: Sort the input matches.
// The first end-point of the first line of a match
// must be linked to the corresponding end-point of the second line.
//---------------------------------------------------------------------------
  void sort_lines(vmal_multi_view_data_edge_sptr matches,
                  vmal_multi_view_data_edge_sptr sorted_matches);

//---------------------------------------------------------------------------
//: Compute the correlation between two lines using the homography.
// It also computes a translation vector to correct the error due to
// the homography.
//---------------------------------------------------------------------------
  double lines_correlation(vtol_edge_2d_sptr line0,
                           vtol_edge_2d_sptr line1,
                           const vnl_double_3x3 & H,
                           vil1_memory_image_of<vxl_byte> &image0,
                           vil1_memory_image_of<vxl_byte> &image1);

 private:

  double seg_angle(vtol_edge_2d_sptr, vtol_edge_2d_sptr);
  bool belong(vtol_edge_2d_sptr, vtol_edge_2d_sptr);
  double dist(vtol_edge_2d_sptr, vtol_edge_2d_sptr);
//-----------------------------------------------------------------------------
//: Project the point (x0,y0) on the line ((ax,ay),(bx,by)).
// The resulting projected point is (x,y).
// It also returns the distance between the line and the projected point.
// If (x,y) does not belong to the segment [(ax,ay),(bx,by)], sets (x,y) to (-1,-1).
//-----------------------------------------------------------------------------
  double project_point(double x0,double y0,
                       double ax,double ay,
                       double bx,double by,
                       double *x,double *y);

  int is_cur_best(vtol_edge_2d_sptr trans_line,vtol_edge_2d_sptr fitted_line,vtol_edge_2d_sptr other_line);
//-----------------------------------------------------------------------------
//: Find the transformed of line.
//-----------------------------------------------------------------------------
  vtol_edge_2d_sptr find_transfo(vtol_edge_2d_sptr line,
                                 vcl_vector<vtol_edge_2d_sptr>& fit_lines,
                                 const vcl_vector<vtol_edge_2d_sptr>& transformed_lines);
//-----------------------------------------------------------------------------
//: Compute two new lines by comparing the input lines and keeping their common part through the homography
//-----------------------------------------------------------------------------
  void refine_line_at_min(vtol_edge_2d_sptr line0, vtol_edge_2d_sptr line1,
                          vtol_edge_2d_sptr &new_line0, vtol_edge_2d_sptr &new_line1,
                          const vnl_double_3x3 &H);

  void sort_a_pair_of_line(vtol_edge_2d_sptr line0,
                           vtol_edge_2d_sptr line1,
                           vtol_edge_2d_sptr &new_line0,
                           vtol_edge_2d_sptr &new_line1);


  void cost_function(vtol_edge_2d_sptr line0,
                     vtol_edge_2d_sptr t_line0,
                     vtol_edge_2d_sptr line1,
                     const vil1_image &image0, const vil1_image &image1,
                     const vnl_double_3x3 homo,
                     double &result);

  double theta_;
  double radius_;
};

#endif // vmal_track_lines_h_
