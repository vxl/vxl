#ifndef vgel_kl_h_
#define vgel_kl_h_

//:
// \file
// \brief Interface to Kanade-Lucas-Tomasi (KLT) feature point tracker
// \author L. Guichard

extern "C" {
#include <vgel/kl/klt.h>
}
#include <vgel/vgel_kl_params.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vil1/vil1_image.h>
#include <vil/vil_image_resource.h>
#include <vcl_vector.h>
#include <vgel/vgel_multi_view_data_vertex_sptr.h>
#include <vidl_vil1/vidl_vil1_movie_sptr.h>
#include <vidl/vidl_movie_sptr.h>
#include <vxl_config.h>
#include <vil/vil_image_view.h>

//: An interface to the C language library implementation of the
// Kanade-Lucas-Tomasi (KLT) feature point tracker by Birchfield,
// which is included.
//
// The Birchfield library was last seen at
// http://vision.stanford.edu/~birch/klt/klt1.1.5.tar.gz
//
// The KLT algorithm is described in:
//
// Shi, Jianbo and Tomasi, Carlo, "Good Features to Track", IEEE
// Conf. on Computer Vision and Pattern Recognition, 1994.

class vgel_kl
{
 protected:
    KLT_TrackingContext seq_tc_;

    KLT_PixelType*      prev_frame_;

    KLT_FeatureList     fl_;

 public:

  //: Default constructor. Parameters set to defaults
  explicit vgel_kl(const vgel_kl_params & params);

  //: Destructor.
  virtual ~vgel_kl();

  virtual void reset_prev_frame();

  //: get matching points from 2 images
  //
  // \deprecated Use other vil(2) version
  // May be removed after VXL 1.1.1
  virtual void match_sequence(vil1_image&, vil1_image&,
                              vgel_multi_view_data_vertex_sptr, bool);
  //: get matching points from 2 images
  virtual void match_sequence(vil_image_resource_sptr&,
                              vil_image_resource_sptr&,
                              vgel_multi_view_data_vertex_sptr, bool);
  //: get matching points from 2 image views
  virtual void match_sequence(vil_image_view<vxl_byte>*,
                              vil_image_view<vxl_byte>*,
                              vgel_multi_view_data_vertex_sptr, bool);

  //: get matching points from a sequence of images
  //
  // \deprecated Use other vil(2) version
  // May be removed after VXL 1.1.1
  virtual void match_sequence(vcl_vector<vil1_image> &,
                              vgel_multi_view_data_vertex_sptr);
  //: get matching points from a sequence of images
  virtual void match_sequence(vcl_vector<vil_image_resource_sptr> &,
                              vgel_multi_view_data_vertex_sptr);

  //: get matching points from a sequence of video frames
  //
  // \deprecated Use other vil(2) version
  // May be removed after VXL 1.1.1
  virtual void match_sequence(vidl_vil1_movie_sptr,
                              vgel_multi_view_data_vertex_sptr);
  //: get matching points from a sequence of video frames
  virtual void match_sequence(vidl_movie_sptr,
                              vgel_multi_view_data_vertex_sptr);

  //: extract feature points from a single image
  //
  // \deprecated Use other vil(2) version
  // May be removed after VXL 1.1.1
  virtual vcl_vector<vtol_vertex_2d_sptr> *
  extract_points(vil1_image &);
  //: extract feature points from a single image
  virtual vcl_vector<vtol_vertex_2d_sptr> *
  extract_points(vil_image_resource_sptr &);

 private:
  vgel_kl_params params_;

  virtual void match_sequence_base(KLT_PixelType *, KLT_PixelType *,
                                   int, int,
                                   vgel_multi_view_data_vertex_sptr, bool);

  virtual void match_sequence_base(vcl_vector<KLT_PixelType *> &,
                                   int, int,
                                   vgel_multi_view_data_vertex_sptr);

  virtual vcl_vector<vtol_vertex_2d_sptr> *
  extract_points_base(KLT_PixelType *, int, int);

  //: Convert a vil1_image to an array of grey scale
  //
  // \deprecated Use other vil(2) version
  // May be removed after VXL 1.1.1
  virtual KLT_PixelType* convert_to_gs_image(vil1_image &);
  //: Convert a vil_image_resource_sptr to an array of grey scale
  virtual KLT_PixelType* convert_to_gs_image(vil_image_resource_sptr &);
  //: Convert a vil_image_view<T> to an array of grey scale
  virtual KLT_PixelType* convert_to_gs_image(vil_image_view<vxl_byte> *);

  virtual void set_tracking_context( KLT_TrackingContext tc);

  virtual void matches_from_feature_table(KLT_FeatureTable  ft,
                          vgel_multi_view_data_vertex_sptr  matches);
};

#endif // vgel_kl_h_
