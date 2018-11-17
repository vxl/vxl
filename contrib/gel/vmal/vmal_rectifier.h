// This is gel/vmal/vmal_rectifier.h
#ifndef vmal_rectifier_h_
#define vmal_rectifier_h_
//--------------------------------------------------------------------------------
//:
// \file
//
//  Take a set of matched points and matched lines into 2 images
//  and transform them so that the epipoles of the 2 images are at infinity.
//  This transformation is called a rectification of the images.
//  It is used in dense matching.
//  The code, here, has been ported from Richard Hartley's code in Target Junior.
//  It also now transforms lines.
//  The method 'factor_Q_matrix_SR' should be removed from here and put in a more
//  suitable place (for example, a class which would handle the fundamental matrix).
//
// \author
//   L. Guichard
// \date 17-MAY-2001
//--------------------------------------------------------------------------------


#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vmal/vmal_multi_view_data_vertex_sptr.h>
#include <vmal/vmal_multi_view_data_edge_sptr.h>
#include <vil/vil_image_view.h>

#include <mvl/TriTensor.h>

class vmal_rectifier
{
 public:

//---------------------------------------------------------------------------
//: Default constructor.
//---------------------------------------------------------------------------
  vmal_rectifier();

//---------------------------------------------------------------------------
//: Constructor.
//---------------------------------------------------------------------------
  vmal_rectifier(const vmal_multi_view_data_vertex_sptr& mvd_vertex,
                 const vmal_multi_view_data_edge_sptr& mvd_edge,
                 int ima_height, int ima_width);

  vmal_rectifier(std::vector< vnl_vector<double> >* pts0,
                 std::vector< vnl_vector<double> >* pts1,
                 int ima_height, int ima_width);

  ~vmal_rectifier();

  void rectification_matrix(vnl_double_3x3& H0,
                            vnl_double_3x3& H1);

  void set_tritensor(TriTensor &tri);

  void compute_joint_epipolar_transform_new (
    vnl_double_3* points0,  // Points in one view
    vnl_double_3* points1,  // Points in the other view
    int numpoints,          // Number of matched points
    vnl_double_3x3 &H0, vnl_double_3x3 &H1,  // The matrices to be returned
    int in_height, int in_width, //Dimensions of the input image
    int &out_height, int &out_width,
    double sweeti, double sweetj,// Sweet spot in the first image
    bool affine /* = false */);

  void compute_initial_joint_epipolar_transforms (
    vnl_double_3* points0,  // Points in one view
    vnl_double_3* points1,  // Points in the other view
    int numpoints,          // Number of matched points
    vnl_double_3x3 &H0, vnl_double_3x3 &H1,  // The matrices to be returned
    double sweeti, double sweetj,// Sweet spot in the first image
    bool affine /* = false */);

  int compute_initial_joint_epipolar_transforms (
    const vnl_double_3x3 &Q,
    double ci, double cj,   // Position of reference point in first image
    vnl_double_3x3 &H0,     // The first transformation matrix computed
    vnl_double_3x3 &H1);    // second transformation matrix to be computed

  vnl_double_3x3 matching_transform (
    const vnl_double_3x3 &Q,
    const vnl_double_3x3 &H1);

  void factor_Q_matrix_SR (
    const vnl_double_3x3 &Q,// 3 x 3 matrix
    vnl_double_3x3 &R,      // non-singular matrix
    vnl_double_3x3 &S);     // Skew-symmetric part

  vnl_double_3x3 affine_correction (
    vnl_double_3* points0,
    vnl_double_3* points1,
    int numpoints,
    const vnl_double_3x3 &H0,
    const vnl_double_3x3 &H1);

  void apply_affine_correction (
    vnl_double_3* points0,  // Points in one view
    vnl_double_3* points1,  // Points in the other view
    int numpoints,          // Number of matched points
    vnl_double_3x3 &H0, vnl_double_3x3 &H1); // The matrices to be returned

  void rectify_rotate90 (
    int &height, int &width,
    vnl_double_3x3 &H0,
    vnl_double_3x3 &H1);

  void conditional_rectify_rotate180 (
    vnl_double_3x3 &H0,
    vnl_double_3x3 &H1);

  void resample (vnl_double_3x3 H0, vnl_double_3x3 H1,
                 const vil_image_view<vxl_byte>& imgL,
                 const vil_image_view<vxl_byte>& imgR);

  vil_image_view<vxl_byte>* GetRectifiedImageLeft() {return rectL;}
  vil_image_view<vxl_byte>* GetRectifiedImageRight() {return rectR;}

 private:
  vnl_double_3* lines0_p_;
  vnl_double_3* lines0_q_;
  vnl_double_3* lines1_p_;
  vnl_double_3* lines1_q_;

  vnl_double_3* points0_;
  vnl_double_3* points1_;
  int numpoints_;

  int height_;
  int width_;
  TriTensor tritensor_;
  vnl_double_3x3 F12_;
  std::vector<vnl_double_3> epipoles_;
  bool is_f_compute_;
  vnl_double_3x3 H0_,H1_;

  vil_image_view<vxl_byte>* rectL;
  vil_image_view<vxl_byte>* rectR;

};

#endif // vmal_rectifier_h_
