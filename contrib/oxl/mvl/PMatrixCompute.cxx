#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cmath.h>
//#include <vcl/vcl_memory.h>
#include <vcl/vcl_cstdlib.h>
#include <mvl/HomgMatchPoint3D2D.h>
#include <mvl/PMatrix.h>
#include <mvl/PMatrixCompute.h>

//-----------------------------------------------------------------------------
//
// -- Constructor.
PMatrixCompute::PMatrixCompute ()
{ 
  // zero length list.
}

//-----------------------------------------------------------------------------
//
// -- Destructor.
PMatrixCompute::~PMatrixCompute()
{
}

//-----------------------------------------------------------------------------
//
// -- The pure virtual compute method.
bool 
PMatrixCompute::compute (PMatrix *)
{
  return false;
}







