// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997-2003 TargetJr Consortium
//                            GE Global Research
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

#ifndef vpgl_rational_camera_h_
#define vpgl_rational_camera_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief The camera class for a generic rational camera.
//  Ported from TargetJr by M. Laymon
//
// \author mlaymon
//     GE Global Research
// \date: 2003/12/11 19:23:35
//
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>

#include <vbl/vbl_smart_ptr.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include <vpgl/vpgl_basic_camera.h>

class vpgl_rational_camera : public vpgl_basic_camera
{
 public:

  static const int O_SCALE_;  //!< Positions in the scale vectors of the offset value
  static const int O_OFFSET_; //!< Position in the scale vectors of the scale value

  static int O_SCALE();
  static int O_OFFSET();

  //-------------------------------------------------------------------
  // Constructors and initialization routines

  vpgl_rational_camera(vnl_matrix<double> const & rational_matrix);

  // New constructor for use with pre and post offsets
  vpgl_rational_camera(
    vnl_matrix<double> const & rational_matrix,
    vnl_vector<double> const & scalex,
    vnl_vector<double> const & scaley,
    vnl_vector<double> const & scalez,
    vnl_vector<double> const & scaleu,
    vnl_vector<double> const & scalev
   );

  vpgl_rational_camera();

  vpgl_rational_camera(double *Un, double *Ud, double *Vn, double *Vd);

  virtual void SetTranslation (double u, double v);

  virtual void SetWorldScale(
    vnl_vector<double>& scalex,
    vnl_vector<double>& scaley,
    vnl_vector<double>& scalez
   );

  virtual void SetImageScale(
    vnl_vector<double>& scalex,
    vnl_vector<double>& scaley
   );

  virtual void SetGlobal(bool const& new_val) { global_ = new_val; }

  //-----------------------------------------------------------------
  // Mapping functions for the cubic camera

  // New vpgl_basic_camera method
  virtual void world_to_image(vnl_vector<double> const& world_3d_pt,
                              double& image_u, double& image_v, double time);

  // New vpgl_basic_camera method
  virtual void world_to_image(double world_x, double world_y, double world_z,
                              double& image_u, double& image_v, double time);

  //-----------------------------------------------------------------
  // Inverse mapping function

  virtual void image_to_world(
    vnl_vector<double> & ray3d_origin,
    vnl_vector<double> &world,
    double u, double v);

// NOT IMPLEMENTED. NO METHOD ImageToSurface IN NEW BASE CLASS vpgl_basic_camera.
#if 0
  virtual bool ImageToSurface(
    float ix, float iy,
    vnl_vector<float> &surface_pt, Surface *surf);
#endif

  //----------------------------------------------------------------
  //: Return information about the rational camera
  virtual vpgl_rational_camera*  GetRationalCamera() { return this; }

  // NOTE: verify_camera should really be const, but it invokes world_to_image.
  //       world_to_image should also be const, but base class vpgl_basic_camera
  //       does not declare world_to_image as const.  Let it go for now.
  //       Fix in base class later ? TBD: mal 10dec2003
  void verify_camera(vnl_vector<double> UL,
                     vnl_vector<double> UR,
                     vnl_vector<double> LR,
                     vnl_vector<double> LL);

#if 0 // NOT IMPLEMENTED
  virtual BasicCamera::Type CameraType() const { return BasicCamera::RATIONAL; }
#endif

  virtual void GetMatrix(vnl_matrix<double>&);
  virtual void GetScaleX(vnl_vector<double>&);
  virtual void GetScaleY(vnl_vector<double>&);
  virtual void GetScaleZ(vnl_vector<double>&);
  virtual void GetScaleU(vnl_vector<double>&);
  virtual void GetScaleV(vnl_vector<double>&);

  virtual double GetTU();
  virtual double GetTV();

  virtual bool IsGlobal() { return global_; }

  //: Adjust the image scale
  virtual vnl_vector<double> scale_inverse(vnl_vector<double> &trans);

  virtual vnl_vector<double> scale_product(
    vnl_vector<double> &sc1,
    vnl_vector<double> &sc2);

  virtual void rescale_image(
    vnl_vector<double> &scu,
    vnl_vector<double> &scv);

  virtual void RescaleWorld(
    vnl_vector<double> &scx,
    vnl_vector<double> &scy,
    vnl_vector<double> &scz);

#if 0
  // VIRTUAL METHOD FROM BASE CLASS BasicCameraInterface
  // Skip for now.  vpgl_basic_camera has no such method.
  virtual BasicCameraInterface* Copy() const;
#endif

  void PrintData(vcl_ostream& strm) const;

  // REMOVE THESE METHODS FOR NOW.  SUPER-CLASS vpgl_basic_camera
  // IMPLEMENTS WITH vnl_vector<double> PARAMETER.
  // init_pt_ ATTRIBUTE IN THIS CLASS SEEMS UNNECESSARY.
#if 0
  virtual void get_init_pt(IUE_vector<float>& p);
  void set_init_pt(IUE_vector<float> * init_pt);
#endif
  virtual void set_init_pt(double x, double y, double z);

 protected:

  static const double DEFAULT_SCALE_VALUE;
  static const double DEFAULT_OFFSET_VALUE;

  void init_scale_vectors(double scale = DEFAULT_SCALE_VALUE,
                          double offset = DEFAULT_OFFSET_VALUE);

  // Defining data for the camera
  vnl_matrix<double> rational_matrix_;

  // Scaling factor for x, y, z coordinates
  // SHOULD THESE BE vnl_vector_2 ?  MAL 6nov2003
  vnl_vector<double> scale_x_;
  vnl_vector<double> scale_y_;
  vnl_vector<double> scale_z_;

  // Scaling factor for affine coordinates
  vnl_vector<double> scale_u_;
  vnl_vector<double> scale_v_;

  bool world_scale_;  //!< flag indicating if we ought to scale the world
  bool image_scale_;  //!< flag indicating if we ought to scale the image

  bool global_; //!< indicates whether camera is global or local

 private:

  virtual vnl_vector<double> PowerVector (const vnl_vector<double>& point3d);
  void ScaleWorldPoint (vnl_vector<double> &xyz);
  void ScaleImagePoint (double &u, double &v);
#if 0
  // init_pt_ ATTRIBUTE IN THIS CLASS SEEMS UNNECESSARY.
  // SUPER-CLASS vpgl_basic_camera HAS ATTRIBUTE vnl_vector<float> init_pt
  // FROM PORT OF TARGETJR CLASS BasicCamera.
  vnl_vector<float> * init_pt_;  // DUPLICATES init_pt in vpgl_basic_camera
#endif // 0
};

typedef vbl_smart_ptr<vpgl_rational_camera> vpgl_rational_camera_sptr;

#endif // vpgl_rational_camera_h_
