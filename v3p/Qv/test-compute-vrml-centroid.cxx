//-*- c++ -*-------------------------------------------------------------------
//
// Module: test
// Purpose: None, totally pointless
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 10 Jul 00
//
// Modifications:
//
//-----------------------------------------------------------------------------

#include "QvVrmlFile.h"

int main(int argc, char ** argv)
{
  QvVrmlFile f((argc>1) ? argv[1] : "test.wrl");
  f.compute_centroid_radius();
  return -1;
}
