//-*- c++ -*-------------------------------------------------------------------
//
// Module: test
// Purpose: None, totally pointless
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 10 Jul 00
// Compile with:
/*
   egcs -I$IUEROOT/vxl -I$IUEROOT/vxl/config.solaris-egcs -I$IUEROOT/v3p test-compute-vrml-centroid.cxx \
      -L$IUEROOT/v3p/lib/solaris-egcs -L$IUEROOT/vxl/lib/solaris-egcs \
      -R $IUEROOT/v3p/lib/solaris-egcs:$IUEROOT/vxl/lib/solaris-egcs \
     -lQv -lvcl
*/
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
