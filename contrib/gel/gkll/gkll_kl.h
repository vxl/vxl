#ifndef gkll_kl_h_
#define gkll_kl_h_
//--------------------------------------------------------------------------------
// .NAME        gkll_kl
// .INCLUDE     gkll/gkll_kl.h
// .FILE        gkll_kl.cxx
// .SECTION     Description:
//   Interface to use Kanade-Lucas algorithm
// .SECTION Author
//   L. Guichard
// .SECTION Modifications:
//--------------------------------------------------------------------------------
extern "C" {
#include <vgel/kl/klt.h>
}
#include <gkll/gkll_kl_params.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vil/vil_image.h>
#include <vcl_vector.h>
#include <gkll/gkll_multi_view_data_vertex_sptr.h>
#include <vidl/vidl_movie_sptr.h>


class gkll_kl {

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

  virtual void match_sequence(vcl_vector<vil_image> &,gkll_multi_view_data_vertex_sptr);

  virtual void match_sequence(vidl_movie_sptr,gkll_multi_view_data_vertex_sptr);

  virtual vcl_vector<vtol_vertex_2d_sptr> * extract_points(vil_image &);

private:
  gkll_kl_params _params;

  virtual KLT_PixelType* convert_to_gs_image(vil_image &);

  virtual void set_tracking_context( KLT_TrackingContext tc);
};

#endif // gkll_kl_
