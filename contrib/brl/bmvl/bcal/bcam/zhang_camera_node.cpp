// ZhangCamera.cpp: implementation of the ZhangCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "zhang_camera_node.h"
#include <vcl_iostream.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

zhang_camera_node::zhang_camera_node(int id) : camera_node(id)
{
  // build lens distortion model
  vcl_vector<bool> flags(7, false);
  flags[0] = true;
  flags[1] = true;
  pCam_ -> setLensModel(flags);

  // allocate space to store features.
  pPointLists_ = 0;
}

zhang_camera_node::~zhang_camera_node()
{
  removeData();  
}

void zhang_camera_node::set_beat(vcl_vector<double> const& new_beat)
{
  camera_node::set_beat(new_beat);

  // allocate space to store data
  if(pPointLists_)
    this->removeData();

  pPointLists_ = new vcl_vector< vgl_homg_point_2d<double> > [nViews_];
}

int zhang_camera_node::readData(char *fname, int iView)
{
  vcl_ifstream  in(fname);

  if(!in){
    vcl_cout<<" cannot open the file: "<<fname;
    return 1;
  }

  if(nViews_<=0){
    vcl_cerr<<" not memory allocated for storing\n";
    return 2;
  }

  if(nViews_<iView){
    vcl_cerr<<"view index out of range of beat \n";
    return 3;
  }

  if(pPointLists_[iView].size() != 0){
    pPointLists_[iView].clear();
  }

  while(!in.eof()){
    double u, v;
    in>>u>>v;
    vgl_homg_point_2d<double> pt(u, v);
    pPointLists_[iView].push_back(pt);
  }
  return 0;  
}

int zhang_camera_node::removeData()
{
  if (pPointLists_)
    delete [] pPointLists_;
  pPointLists_ = 0;

  return 0;
}
