#ifndef gkll_kl_h_
#define gkll_kl_h_
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
#include <gkll/gkll_kl_params.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vil1/vil1_image.h>
#include <vcl_vector.h>
#include <gkll/gkll_multi_view_data_vertex_sptr.h>
#include <vidl_vil1/vidl_vil1_movie_sptr.h>


class gkll_kl
{
 public:

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor. Parameters set to defaults
  //---------------------------------------------------------------------------
  explicit gkll_kl(const gkll_kl_params & params);

  //---------------------------------------------------------------------------
  //: Destructor.
  //---------------------------------------------------------------------------
  virtual ~gkll_kl();

  virtual void match_sequence(vcl_vector<vil1_image> &,gkll_multi_view_data_vertex_sptr);

  virtual void match_sequence(vidl_vil1_movie_sptr,gkll_multi_view_data_vertex_sptr);

  virtual vcl_vector<vtol_vertex_2d_sptr> * extract_points(vil1_image &);

 private:
  gkll_kl_params params_;

  virtual KLT_PixelType* convert_to_gs_image(vil1_image &);

  virtual void set_tracking_context( KLT_TrackingContext tc);
};

#endif // gkll_kl_h_
