#include <vcl_vector.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vbl/vbl_array_2d.txx>
typedef vcl_vector<vcl_vector<vsol_point_3d_sptr> >* vptr;
VBL_ARRAY_2D_INSTANTIATE(vptr);
