// ZhangCamera.cpp: implementation of the ZhangCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "zhang_camera_node.h"
#include <vcl_iostream.h>
#include <vcl_fstream.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

zhang_camera_node::zhang_camera_node(int id) : camera_node(id)
{
  // build lens distortion model
  vcl_vector<bool> flags(7, false);
  flags[0] = true;
  flags[1] = true;
  cam_ -> set_lens_model(flags);

  // allocate space to store features.
  point_lists_ptr_ = 0;
}

zhang_camera_node::~zhang_camera_node()
{
  removeData();  
}

void zhang_camera_node::set_beat(vcl_vector<double> const& new_beat)
{
  camera_node::set_beat(new_beat);

  // allocate space to store data
  if(point_lists_ptr_)
    this->removeData();

  point_lists_ptr_ = new vcl_vector< vgl_homg_point_2d<double> > [num_views_];
}

int zhang_camera_node::readData(char *fname, int iView)
{
  vcl_ifstream  in(fname);

  if(!in){
    vcl_cout<<" cannot open the file: "<<fname;
    return 1;
  }

  if(num_views_<=0){
    vcl_cerr<<" not memory allocated for storing\n";
    return 2;
  }

  if(num_views_<iView){
    vcl_cerr<<"view index out of range of beat \n";
    return 3;
  }

  if(point_lists_ptr_[iView].size() != 0){
    point_lists_ptr_[iView].clear();
  }

  while(!in.eof()){
    double u, v;
    in>>u>>v;
    vgl_homg_point_2d<double> pt(u, v);
    point_lists_ptr_[iView].push_back(pt);
  }
  return 0;  
}

int zhang_camera_node::removeData()
{
  if (point_lists_ptr_)
    delete [] point_lists_ptr_;
  point_lists_ptr_ = 0;

  return 0;
}
