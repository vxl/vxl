// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a cartesian 3D box.


//--------------------------------------------------------------
//
// Class : vgl_box_3d
//
//--------------------------------------------------------------

#include <vgl/vgl_box_3d.h> 

// Constructors/Destructors--------------------------------------------------

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const Type min_position[3], 
                             const Type max_position[3] )
{
  _min_pos[0]=min_position[0];
  _min_pos[1]=min_position[1];
  _min_pos[2]=min_position[2];

  _max_pos[0]=max_position[0];
  _max_pos[1]=max_position[1];
  _max_pos[2]=max_position[2];
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const vgl_point_3d<Type>& min_position, 
                             const vgl_point_3d<Type>& max_pos)
{
  _min_pos[0]=min_position.x();
  _min_pos[1]=min_position.y();
  _min_pos[2]=min_position.z();

  _max_pos[0]=max_position.x();
  _max_pos[1]=max_position.y();
  _max_pos[2]=max_position.z();
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const Type xmin, const Type xmax,
                             const Type ymin, const Type ymax,
                             const Type zmin, const Type zmax)
{
  _min_pos[0]=xmin;
  _min_pos[1]=ymin;
  _min_pos[2]=zmin;
  
  _max_pos[0]=xmax;
  _max_pos[1]=ymax;
  _max_pos[2]=zmax;
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const Type min_position[3],
                             const Type width, const Type height,
                             const Type depth)
{
  _min_pos[0]=min_position[0];
  _min_pos[1]=min_position[1];
  _min_pos[2]=min_position[2];

  _max_pos[0]=min_position[0]+width;
  _max_pos[1]=min_position[1]+height;
  _max_pos[2]=min_position[2]+depth;
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const vgl_point_3d<Type>& min_position,
                             const Type width, const Type height,
                             const Type depth)
{
  _min_pos[0]=min_position.x();
  _min_pos[1]=min_position.y();
  _min_pos[2]=min_position.z();

  _max_pos[0]=min_position.x()+width;
  _max_pos[1]=min_position.y()+height;
  _max_pos[2]=min_position.z()+depth;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_x(const Type centroid_x)
{
  Type delta = centroid_x - get_centroid_x();
  _min_pos[0]= _min_pos[0] + delta;
  _max_pos[0]= _max_pos[0] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_y(const Type centroid_y)
{
  Type delta = centroid_y - get_centroid_y();
  _min_pos[1]= _min_pos[1] + delta;
  _max_pos[1]= _max_pos[1] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_z(const Type centroid_z)
{
  Type delta = centroid_z - get_centroid_z();
  _min_pos[2]= _min_pos[2] + delta;
  _max_pos[2]= _max_pos[2] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_width(const Type width)
{
  Type x = get_centroid_x();
  _min_pos[0] = x-width/2;
  _max_pos[0] = x+width/2;
}

template <class Type> 
void vgl_box_3d<Type>::set_height(const Type height)
{
  Type y = get_centroid_y();
  _min_pos[1] = y-width/2;
  _max_pos[1] = y+width/2;
}

template <class Type>
void vgl_box_3d<Type>::set_depth(const Type depth)
{
  Type z = get_centroid_z();
  _min_pos[2] = z-depth/2;
  _max_pos[2] = z+depth/2;
}

template <class Type>
void vgl_box_3d<Type>::set_min_position(const Type min_position[3])
{
  _min_pos[0]=min_position[0];
  _min_pos[1]=min_position[1];
  _min_pos[2]=min_position[2];

  if(_max_pos[0] < _min_pos[0]){
    _max_pos[0]=_min_pos[0];
  }
  if(_max_pos[1] < _min_pos[1]){
    _max_pos[1]=_min_pos[1];
  }
 if(_max_pos[2] < _min_pos[2]){
    _max_pos[2]=_min_pos[2];
  }
}
 
template <class Type>
void vgl_box_3d<Type>::set_max_position(const Type max_position[3])
{
  _max_pos[0]=max_position[0];
  _max_pos[1]=max_position[1];
  _max_pos[2]=max_position[2];

  if(_max_pos[0] < _min_pos[0]){
    _min_pos[0]=_max_pos[0];
  }
  if(_max_pos[1] < _min_pos[1]){
    _min_pos[1]=_max_pos[1];
  }
 if(_max_pos[2] < _min_pos[2]){
    _min_pos[2]=_max_pos[2];
  }
}

template <class Type>
void vgl_box_3d<Type>::set_min_point(vgl_point_3d<Type>& min_point)
{
  _min_pos[0]=min_point.x();
  _min_pos[1]=min_point.y();
  _min_pos[2]=min_point.Z();
 
  if(_max_pos[0] < _min_pos[0]){
    _max_pos[0]=_min_pos[0];
  }
  if(_max_pos[1] < _min_pos[1]){
    _max_pos[1]=_min_pos[1];
  }
  if(_max_pos[2] < _min_pos[2]){
    _max_pos[2]=_min_pos[2];
  }
}

template <class Type>
void vgl_box_3d<Type>::set_max_point(vgl_point_3d<Type>& max_point)
{
  _max_pos[0]=max_point.x();
  _max_pos[1]=max_point.y();
  _max_pos[2]=max_point.Z();

  if(_max_pos[0] < _min_pos[0]){
    _min_pos[0]=_max_pos[0];
  }
  if(_max_pos[1] < _min_pos[1]){
    _min_pos[1]=_max_pos[1];
  }
  if(_max_pos[2] < _min_pos[2]){
    _min_pos[2]=_max_pos[2];
  }
}

template <class Type>
void vgl_box_3d<Type>::set_centroid(Type centroid[3])
{
  set_centroid_x(centroid[0]);
  set_centroid_y(centroid[1]);
  set_centroid_Z(centroid[2]);
}

template <class Type>
void vgl_box_3d<Type>::set_centroid(vgl_point_3d<Type>& centroid)
{
  set_centroid_x(centroid.x());
  set_centroid_y(centroid.y());
  set_centroid_Z(centroid.Z());
}
