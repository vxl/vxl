//-*- c++ -*-------------------------------------------------------------------
#ifndef VoxmapImagePoints_h_
#define VoxmapImagePoints_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : VoxmapImagePoints
//
// .SECTION Description
//    VoxmapImagePoints is a class that Geoff hasn't documented properly. FIXME
//
// .NAME        VoxmapImagePoints - Undocumented class FIXME
// .LIBRARY     new
// .HEADER	octree Package
// .INCLUDE     new/VoxmapImagePoints.h
// .FILE        VoxmapImagePoints.h
// .FILE        VoxmapImagePoints.C
// .SECTION Author
//     Geoffrey Cross, Oxford RRG, 20 May 99
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_sparse_array_3d.h>
#include <vnl/vnl_double_2.h>
#include <mvl/PMatrix.h>

#include "VoxmapPoints.h"
#include "BigSparseArray3D.h"

class VoxmapImagePoints : public VoxmapPoints{
public:
  // Constructors/Destructors--------------------------------------------------
  
  VoxmapImagePoints( int, vnl_double_3, double, int);

  // Operations----------------------------------------------------------------
  
  void SetPMatrix( PMatrix &P, int im);

  // Computations--------------------------------------------------------------

  vnl_double_2 *GetCentreImage( int x, int y, int z, int d, int im);
  vnl_double_2 *GetCornerImage( int x, int y, int z, int dx, int dy, int dz, int d, int im);

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------

  PMatrix                  **pmatrices;
  BigSparseArray3D<vnl_double_2 *> **imagecorners;
  BigSparseArray3D<vnl_double_2 *> **imagecentres;
  
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS VoxmapImagePoints.

