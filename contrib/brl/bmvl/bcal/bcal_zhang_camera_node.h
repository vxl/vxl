//------------------------------------------------------------------------------
// FileName        : bcal_zhang_camera_node.h
// Author          : Kongbin Kang (kk@lems.brown.edu)
// Company         : Brown University
// Purpose         : Zhang Camera Node
// Date Of Creation: 3/23/2003
// Modification History :
// Date             Modifications
//------------------------------------------------------------------------------

#ifndef AFX_ZHANGCAMERA_H__EB787129_58FA_4195_A386_71D7CC0C9546__INCLUDED_
#define AFX_ZHANGCAMERA_H__EB787129_58FA_4195_A386_71D7CC0C9546__INCLUDED_

#if defined(_MSC_VER) && ( _MSC_VER > 1000 )
#pragma once
#endif // _MSC_VER > 1000

#include "bcal_camera_node.h"
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>

class bcal_zhang_camera_node : public bcal_camera_node
{
 private:
  vcl_vector< vgl_homg_point_2d<double> > *point_lists_ptr_;

 public: // constructor and deconstructor
  bcal_zhang_camera_node(int id=0);
  virtual ~bcal_zhang_camera_node();
 public:
  int read_data(vcl_vector< vgl_homg_point_2d<double> > &ppts, int iframe);
  int removeData();

  // set get function
  vcl_vector< vgl_homg_point_2d<double> >& getPoints(int iview)
  {
    return point_lists_ptr_[iview];
  }

  int readData(const char* fname, int iview);  // for debugging

  virtual void set_beat(vcl_vector<double> const& new_beats);
};

#endif // AFX_ZHANGCAMERA_H__EB787129_58FA_4195_A386_71D7CC0C9546__INCLUDED_
