#include <vgl/vgl_homg_point_3d.h>

//---------------------------------------------------------------------------
// -- Default constructor with (0,0,0,1)
//---------------------------------------------------------------------------
template <class Type>
vgl_homg_point_3d<Type>::vgl_homg_point_3d(void)
{
  data_[0]=0;
  data_[1]=0;
  data_[2]=0;
  data_[3]=1;
}
