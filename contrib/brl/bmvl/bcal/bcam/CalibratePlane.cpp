// CalibratePlane.cpp: implementation of the CalibratePlane class.
//
//////////////////////////////////////////////////////////////////////

#include "CalibratePlane.h"
#include <vcl_fstream.h>
#include <vcl_iostream.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CalibratePlane::CalibratePlane()
{

}

CalibratePlane::~CalibratePlane()
{

}


int CalibratePlane::readData(char *fname)
{
  vcl_ifstream  in(fname);

  if(!in){
    vcl_cerr<<"cannot open the file: "<<fname;
    return 1;
  }
  if(pts_.size() != 0){
    pts_.clear();
  }

  while(!in.eof()){
    double u, v;
    in>>u>>v;
    vgl_homg_point_2d<double> pt(u, v);
    pts_.push_back(pt);
  }

  return 0;
}
