#ifndef vgel_kl_h_
#define vgel_kl_h_
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
#include <vgel/vgel_kl_params.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vil1/vil1_image.h>
#include <vcl_vector.h>
#include <vgel/vgel_multi_view_data_vertex_sptr.h>
#include <vidl_vil1/vidl_vil1_movie_sptr.h>


class vgel_kl
{
 public:

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default constructor. Parametres set to defaults
//---------------------------------------------------------------------------
  explicit vgel_kl(const vgel_kl_params & params);

//---------------------------------------------------------------------------
//: Destructor.
//---------------------------------------------------------------------------
  virtual ~vgel_kl();

  virtual void match_sequence(vcl_vector<vil1_image> &,vgel_multi_view_data_vertex_sptr);

  virtual void match_sequence(vidl_vil1_movie_sptr,vgel_multi_view_data_vertex_sptr);

  virtual vcl_vector<vtol_vertex_2d_sptr> * extract_points(vil1_image &);

 private:
  vgel_kl_params params_;

  virtual KLT_PixelType* convert_to_gs_image(vil1_image &);

  virtual void set_tracking_context( KLT_TrackingContext tc);
};

#endif // vgel_kl_h_
