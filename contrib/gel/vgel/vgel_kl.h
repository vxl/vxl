#ifndef vgel_kl_h_
#define vgel_kl_h_
//--------------------------------------------------------------------------------
// .NAME        vgel_kl
// .INCLUDE     vgel/vgel_kl.h
// .FILE        vgel_kl.cxx
// .SECTION     Description:
//   Interface to use Kanade-Lucas algorithm
// .SECTION Author
//   L. Guichard
// .SECTION Modifications:
//--------------------------------------------------------------------------------
extern "C" {
#include <vgel/kl/klt.h>
}
#include <vgel/vgel_kl_params.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vil/vil_image.h>
#include <vcl_vector.h>
#include <vgel/vgel_multi_view_data_vertex_sptr.h>
#include <vidl/vidl_movie_sptr.h>


class vgel_kl {

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

  virtual void match_sequence(vcl_vector<vil_image> &,vgel_multi_view_data_vertex_sptr);

  virtual void match_sequence(vidl_movie_sptr,vgel_multi_view_data_vertex_sptr);

  virtual vcl_vector<vtol_vertex_2d_sptr> * extract_points(vil_image &);

private:
  vgel_kl_params _params;

  virtual KLT_PixelType* convert_to_gs_image(vil_image &);

  virtual void set_tracking_context( KLT_TrackingContext tc);
};

#endif // vgel_kl_
