
// bcal_camera_node.h: interface for the bcal_camera_node class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_
#define AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_

#if defined(_MSC_VER) && ( _MSC_VER > 1000 )
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_vector.h>
#include <vcsl/vcsl_spatial.h>
#include "bcal_camera.h"

class bcal_camera_node : public vcsl_spatial
{
protected:
  bcal_camera* cam_;
  int num_views_;
public:
  bcal_camera_node(int id=0);
  virtual ~bcal_camera_node();
public:
  virtual void set_beat(vcl_vector<double> const & new_beat);
  vnl_double_3x3 get_intrinsic() const { return cam_->get_intrisic_matrix();} 
  void set_intrinsic(vnl_double_3x3 k)  { cam_->set_intrisic_matrix(k);}
  int get_id() const { return cam_->getID();} 
  int num_views() { return num_views_;}
};


#endif // AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_
