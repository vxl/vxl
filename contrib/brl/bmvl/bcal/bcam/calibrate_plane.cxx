// calibrate_plane.cpp: implementation of the calibrate_plane class.
//
//////////////////////////////////////////////////////////////////////

#include "calibrate_plane.h"
#include <vcl_fstream.h>
#include <vcl_iostream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

calibrate_plane::calibrate_plane()
{
}

calibrate_plane::~calibrate_plane()
{
}


int calibrate_plane::readData(char *fname)
{
  vcl_ifstream  in(fname);

  if (!in){
    vcl_cerr<<"cannot open the file: "<<fname << vcl_endl;
    return 1;
  }
  if (pts_.size() != 0){
    pts_.clear();
  }

  while (!in.eof()){
    double u, v;
    in>>u>>v;
    vgl_homg_point_2d<double> pt(u, v);
    pts_.push_back(pt);
  }

  return 0;
}
