// bcal_calibrate_plane.cpp: implementation of the bcal_calibrate_plane class.
//
//////////////////////////////////////////////////////////////////////

#include "bcal_calibrate_plane.h"
#include <vcl_fstream.h>
#include <vcl_iostream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bcal_calibrate_plane::bcal_calibrate_plane()
{
}

bcal_calibrate_plane::~bcal_calibrate_plane()
{
}


int bcal_calibrate_plane::readData(const char *fname)
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

int bcal_calibrate_plane::read_data(vcl_vector<vgl_homg_point_2d<double> > &pts)
{
  pts_ = pts;

  return 0;
}
