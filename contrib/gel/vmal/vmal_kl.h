#ifndef vmal_kl_h_
#define vmal_kl_h_
//--------------------------------------------------------------------------------
// .NAME        vmal_kl
// .INCLUDE     vmal/vmal_kl.h
// .FILE        vmal_kl.cxx
// .SECTION Description:
//   Interface to use Kanade-Lucas algorithm
// .SECTION Author
//   L. Guichard
// .SECTION Modifications:
//--------------------------------------------------------------------------------
extern "C" {
#include <vgel/kl/klt.h>
}
#include <vmal/vmal_kl_params.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vil/vil_image.h>
#include <vcl_vector.h>
#include <vmal/vmal_multi_view_data_vertex_sptr.h>
#include <vidl/vidl_movie_sptr.h>


class vmal_kl {

public:

//---------------------------------------------------------------------------
//: Default constructor. Parametres set to defaults
//---------------------------------------------------------------------------
  explicit vmal_kl(const vmal_kl_params & params);

//---------------------------------------------------------------------------
//: Destructor.
//---------------------------------------------------------------------------
  ~vmal_kl();

  void match_sequence(vcl_vector<vil_image> &,vmal_multi_view_data_vertex_sptr);

  void match_sequence(vidl_movie_sptr,vmal_multi_view_data_vertex_sptr);

  vcl_vector<vtol_vertex_2d_sptr> * extract_points(vil_image &);

private:
  vmal_kl_params _params;

  KLT_PixelType* convert_to_gs_image(vil_image &);

  void set_tracking_context( KLT_TrackingContext tc);
};

#endif // vmal_kl_
