//*****************************************************************************
// File name: vgl_homg_point_3d.h
// Description: Represents a homogeneous 3D plane
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/05| François BERTEL          | #include trouble fixed
//         |          |                          | val expression bug fixed
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/02/15| Don HAMILTON, Peter TU   |Creation
//*****************************************************************************
#include <vgl/vgl_homg_plane_3d.h> 

#include <vgl/vgl_homg_point_3d.h>

//-- Construct from Normal and a point
template <class Type>
vgl_homg_plane_3d<Type>::vgl_homg_plane_3d(const Type normal[3],
                                           const vgl_homg_point_3d<Type> &p)
{
  // find d given then x*nx + y*ny + z*nz + d*w = 0
  
  Type w=p.w();
  
  // TODO - use tolerance
  if(w != 0.0)
    {
      Type val=normal[0]*p.x()+normal[1]*p.y()+normal[2]*p.z(); 
      Type d=val/w;
      set(normal[0],normal[1],normal[2],d);
    }
  else
    // the point is at infinity so set the plane to infinity TODO
    set(0,0,0,1);
}



