// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
// Modifications:
//  Peter Vanroose, Feb 28 2000: lots of minor corrections
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
vgl_box_3d<Type>::vgl_box_3d(const vgl_point_3d<Type>& min_pos,
                             const vgl_point_3d<Type>& max_pos)
{
  _min_pos[0]=min_pos.x();
  _min_pos[1]=min_pos.y();
  _min_pos[2]=min_pos.z();

  _max_pos[0]=max_pos.x();
  _max_pos[1]=max_pos.y();
  _max_pos[2]=max_pos.z();
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(Type xmin, Type ymin, Type zmin,
                             Type xmax, Type ymax, Type zmax)
{
  _min_pos[0]=xmin;
  _min_pos[1]=ymin;
  _min_pos[2]=zmin;
  
  _max_pos[0]=xmax;
  _max_pos[1]=ymax;
  _max_pos[2]=zmax;
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const Type centroid[3],
                             Type width, Type height, Type depth)
{
  _min_pos[0]=centroid[0]-0.5*width;
  _min_pos[1]=centroid[1]-0.5*height;
  _min_pos[2]=centroid[2]-0.5*height;

  _max_pos[0]=centroid[0]+0.5*width;
  _max_pos[1]=centroid[1]+0.5*height;
  _max_pos[2]=centroid[2]+0.5*depth;
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const vgl_point_3d<Type>& centroid,
                             Type width, Type height, Type depth)
{
  _min_pos[0]=centroid.x()-0.5*width;
  _min_pos[1]=centroid.y()-0.5*height;
  _max_pos[1]=centroid.z()-0.5*depth;

  _max_pos[0]=centroid.x()+0.5*width;
  _max_pos[1]=centroid.y()+0.5*height;
  _max_pos[1]=centroid.z()+0.5*depth;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_x(Type centroid_x)
{
  Type delta = centroid_x - get_centroid_x();
  _min_pos[0]= _min_pos[0] + delta;
  _max_pos[0]= _max_pos[0] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_y(Type centroid_y)
{
  Type delta = centroid_y - get_centroid_y();
  _min_pos[1]= _min_pos[1] + delta;
  _max_pos[1]= _max_pos[1] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_z(Type centroid_z)
{
  Type delta = centroid_z - get_centroid_z();
  _min_pos[2]= _min_pos[2] + delta;
  _max_pos[2]= _max_pos[2] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_width(const Type width)
{
  Type x = get_centroid_x();
  _min_pos[0] = x-0.5*width;
  _max_pos[0] = x+0.5*width;
}

template <class Type> 
void vgl_box_3d<Type>::set_height(const Type height)
{
  Type y = get_centroid_y();
  _min_pos[1] = y-0.5*height;
  _max_pos[1] = y+0.5*height;
}

template <class Type>
void vgl_box_3d<Type>::set_depth(const Type depth)
{
  Type z = get_centroid_z();
  _min_pos[2] = z-0.5*depth;
  _max_pos[2] = z+0.5*depth;
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
void vgl_box_3d<Type>::set_centroid(vgl_point_3d<Type>& centroid)
{
  set_centroid_x(centroid.x());
  set_centroid_y(centroid.y());
  set_centroid_z(centroid.z());
}
