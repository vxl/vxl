// ZhangCamera.cpp: implementation of the ZhangCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "ZhangCameraNode.h"
#include <vcl_iostream.h>
#include <vcl_fstream.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZhangCameraNode::ZhangCameraNode(int id, int nViews) : CameraNode(id)
{
  // build lens distortion model
  vcl_vector<bool> flags(7, false);
  flags[0] = true;
  flags[1] = true;
  pCam_ -> setLensModel(flags);

  // allocate space to store features.
  nViews_ = nViews;
  pPointLists_ = new vcl_vector< vgl_homg_point_2d<double> > [nViews];
}

ZhangCameraNode::~ZhangCameraNode()
{
  if (pPointLists_)
    delete [] pPointLists_;
}

int ZhangCameraNode::readData(char *fname, int iView)
{
  vcl_ifstream  in(fname);

  if (!in){
    vcl_cout<<" cannot open the file: "<<fname;
    return 1;
  }

  if (nViews_<=0){
    vcl_cerr<<" not memory allocated for storing\n";
    return 2;
  }

  if (pPointLists_[iView].size() != 0){
    pPointLists_[iView].clear();
  }

  while (!in.eof()){
    double u, v;
    in>>u>>v;
    vgl_homg_point_2d<double> pt(u, v);
    pPointLists_[iView].push_back(pt);
  }
  return 0;
}
