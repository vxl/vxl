// This is gel/vmal/vmal_rectifier.h
#ifndef vmal_rectifier_h_
#define vmal_rectifier_h_
//--------------------------------------------------------------------------------
//:
//  \file
//
//   Take a set of matched points and matched lines into 2 images
//   and transform them so that the epipoles of the 2 images are at infinity.
//   This transformation is called a rectification of the images.
//   It is used in dense matching.
//   The code, here, has been ported from Richard Harttley's code in Target Junior.
//   It also now tranform lines.
//   The method 'factor_Q_matrix_SR' should be removed from here and put in a more
//   suitable place (for example, a class which would handle the fundamental matrix).
//
// \author
//   L. Guichard 17-MAY-2001
//--------------------------------------------------------------------------------


#include <vcl_vector.h>

#include <vmal/vmal_multi_view_data_vertex_sptr.h>
#include <vmal/vmal_multi_view_data_edge_sptr.h>

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
  vmal_rectifier(vmal_multi_view_data_vertex_sptr mvd_vertex,
           vmal_multi_view_data_edge_sptr mvd_edge,
            int ima_height, int ima_width);

  ~vmal_rectifier();

  void rectification_matrix(vnl_double_3x3& H0,
                vnl_double_3x3& H1);

  void set_tritensor(TriTensor &tri);

  void compute_joint_epipolar_transform_new (
    vnl_double_3* points0,  /* Points in one view */
    vnl_double_3* points1,  /* Points in the other view */
    int numpoints,          /* Number of matched points */
    vnl_double_3x3 &H0, vnl_double_3x3 &H1,  /* The matrices to be returned */
    int in_height, int in_width, /*Dimensions of the input images*/
    int &out_height, int &out_width,
    double sweeti, double sweetj,/* Sweet spot in the first image */
    bool affine /*- = FALSE -*/);

  void compute_initial_joint_epipolar_transforms (
    vnl_double_3* points0,  /* Points in one view */
    vnl_double_3* points1,  /* Points in the other view */
    int numpoints,          /* Number of matched points */
    vnl_double_3x3 &H0, vnl_double_3x3 &H1,  /* The matrices to be returned */
    double sweeti, double sweetj,/* Sweet spot in the first image */
    bool affine /*- = FALSE -*/);

  int compute_initial_joint_epipolar_transforms (
    const vnl_double_3x3 &Q,
    double ci, double cj,   /* Position of reference point in first image */
    vnl_double_3x3 &H0,     /* The first transformation matrix computed */
    vnl_double_3x3 &H1      /* second transfrmtion matrix to be computed */);

  vnl_double_3x3 matching_transform (
    const vnl_double_3x3 &Q,
    const vnl_double_3x3 &H1);

  void factor_Q_matrix_SR (
        const vnl_double_3x3 &Q,/* 3 x 3 matrix */
        vnl_double_3x3 &R,      /* non-singular matrix */
        vnl_double_3x3 &S);     /* Skew-symmetric part */

  vnl_double_3x3 affine_correction (
    vnl_double_3* points0,
    vnl_double_3* points1,
    int numpoints,
    const vnl_double_3x3 &H0,
    const vnl_double_3x3 &H1);

  void apply_affine_correction (
    vnl_double_3* points0,  /* Points in one view */
    vnl_double_3* points1,  /* Points in the other view */
    int numpoints,          /* Number of matched points */
    vnl_double_3x3 &H0, vnl_double_3x3 &H1   /* The matrices to be returned */);

  void rectify_rotate90 (
    int &height, int &width,
    vnl_double_3x3 &H0,
    vnl_double_3x3 &H1);

 private:

  vnl_double_3* _lines0_p;
  vnl_double_3* _lines0_q;
  vnl_double_3* _lines1_p;
  vnl_double_3* _lines1_q;

  vnl_double_3* _points0;
  vnl_double_3* _points1;
  int _numpoints;

  int _height;
  int _width;
  TriTensor _tritensor;
  vnl_double_3x3 _F12;
  vcl_vector<vnl_double_3> _epipoles;
  bool _is_f_compute;
  vnl_double_3x3 _H0,_H1;
};

#endif //vmal_rectifier_h_
