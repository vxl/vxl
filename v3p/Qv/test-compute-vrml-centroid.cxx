//-*- c++ -*-------------------------------------------------------------------
//
// Module: test
// Purpose: None, totally pointless
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 10 Jul 00
// Compile with:
//
// $CXX -I$IUEROOT/vxl -I$IUEROOT/vxl/config.$TJ_BUILD_TAG -I$IUEROOT/v3p test-compute-vrml-centroid.cxx \
//    -L$IUEROOT/v3p/lib/$TJ_BUILD_TAG -L$IUEROOT/vxl/lib/$TJ_BUILD_TAG \
//    -Wl,-rpath,$IUEROOT/v3p/lib/$TJ_BUILD_TAG:$IUEROOT/vxl/lib/$TJ_BUILD_TAG \
//    -lQv -lvcl
//
// Modifications:
//
//-----------------------------------------------------------------------------

#include "QvVrmlFile.h"

int main(int argc, char ** argv)
{
  QvVrmlFile f(argv[1]);
  f.compute_centroid_radius();
  return -1;
}
