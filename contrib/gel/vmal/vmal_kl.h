#ifndef vmal_kl_h_
#define vmal_kl_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Interface to use Kanade-Lucas algorithm
// \author
//   L. Guichard
//--------------------------------------------------------------------------------
extern "C" {
#include <vgel/kl/klt.h>
}
#include <vmal/vmal_kl_params.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vil1/vil1_image.h>
#include <vcl_vector.h>
#include <vmal/vmal_multi_view_data_vertex_sptr.h>
#include <vidl_vil1/vidl_vil1_movie_sptr.h>


class vmal_kl
{
 public:

  //---------------------------------------------------------------------------
  //: Default constructor. Parametres set to defaults
  //---------------------------------------------------------------------------
  explicit vmal_kl(const vmal_kl_params & params);

  //---------------------------------------------------------------------------
  //: Destructor.
  //---------------------------------------------------------------------------
  ~vmal_kl();

  void match_sequence(vcl_vector<vil1_image> &,vmal_multi_view_data_vertex_sptr);

  void match_sequence(vidl_vil1_movie_sptr,vmal_multi_view_data_vertex_sptr);

  vcl_vector<vtol_vertex_2d_sptr> * extract_points(vil1_image &);

 private:
  vmal_kl_params params_;

  KLT_PixelType* convert_to_gs_image(vil1_image &);

  void set_tracking_context( KLT_TrackingContext tc);
};

#endif // vmal_kl_h_
