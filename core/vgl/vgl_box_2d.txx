// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
// Modifications:
//  Peter Vanroose, Feb 28 2000: lots of minor corrections
//: Represents a cartesian 2D box.


//--------------------------------------------------------------
//
// Class : vgl_box_2d
//
//--------------------------------------------------------------

#include <vgl/vgl_box_2d.h>

// Constructors/Destructors--------------------------------------------------

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const Type min_position[2],
                             const Type max_position[2] )
{
  _min_pos[0]=min_position[0];
  _min_pos[1]=min_position[1];
  _max_pos[0]=max_position[0];
  _max_pos[1]=max_position[1];
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const vgl_point_2d<Type>& min_pos,
                             const vgl_point_2d<Type>& max_pos)
{
  _min_pos[0]=min_pos.x();
  _min_pos[1]=min_pos.y();
  _max_pos[0]=max_pos.x();
  _max_pos[1]=max_pos.y();
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(Type xmin, Type ymin, Type xmax, Type ymax)
{
  _min_pos[0]=xmin;
  _min_pos[1]=ymin;
  _max_pos[0]=xmax;
  _max_pos[1]=ymax;
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const Type centroid[2],
                             Type width, Type height)
{
  _min_pos[0]=centroid[0]-0.5*width;
  _min_pos[1]=centroid[1]-0.5*height;
  _max_pos[0]=centroid[0]+0.5*width;
  _max_pos[1]=centroid[1]+0.5*height;
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const vgl_point_2d<Type>& centroid,
                             Type width, Type height)
{
  _min_pos[0]=centroid.x()-0.5*width;
  _min_pos[1]=centroid.y()-0.5*height;
  _max_pos[0]=centroid.x()+0.5*width;
  _max_pos[1]=centroid.y()+0.5*height;
}

template <class Type>
Type vgl_box_2d<Type>::get_centroid_x() const
{
  return (_min_pos[0] + _max_pos[0])/2;
}

template <class Type>
Type vgl_box_2d<Type>::get_centroid_y() const
{
  return (_min_pos[1] + _max_pos[1])/2;
}

template <class Type>
Type vgl_box_2d<Type>::get_width() const
{
  return (_max_pos[0] - _min_pos[0]);
}

template <class Type>
Type vgl_box_2d<Type>::get_height() const
{
  return (_max_pos[1] - _min_pos[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::get_min_point() const
{
  return vgl_point_2d<Type>(_min_pos[0],_min_pos[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::get_max_point() const
{
  return vgl_point_2d<Type>(_max_pos[0],_max_pos[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::get_centroid_point() const
{
  return vgl_point_2d<Type>(get_centroid_x(),get_centroid_y());
}

template <class Type>
void vgl_box_2d<Type>::set_centroid_x(Type centroid_x)
{
  Type delta = centroid_x - get_centroid_x();
  _min_pos[0] += delta;
  _max_pos[0] += delta;
}

template <class Type>
void vgl_box_2d<Type>::set_centroid_y(Type centroid_y)
{
  Type delta = centroid_y - get_centroid_y();
  _min_pos[1] += delta;
  _max_pos[1] += delta;
}

template <class Type>
void vgl_box_2d<Type>::set_width(Type width)
{
  Type x = get_centroid_x();
  _min_pos[0] = x-0.5*width;
  _max_pos[0] = x+0.5*width;
}

template <class Type>
void vgl_box_2d<Type>::set_height(Type height)
{
  Type y = get_centroid_y();
  _min_pos[1] = y-0.5*width;
  _max_pos[1] = y+0.5*width;
}

template <class Type>
void vgl_box_2d<Type>::set_min_point(vgl_point_2d<Type> const& min_point)
{
  _min_pos[0]=min_point.x();
  _min_pos[1]=min_point.y();
  if(_max_pos[0] < _min_pos[0]){
    _max_pos[0]=_min_pos[0];
  }
  if(_max_pos[1] < _min_pos[1]){
    _max_pos[1]=_min_pos[1];
  }
}


template <class Type>
void vgl_box_2d<Type>::set_max_point(vgl_point_2d<Type> const& max_point)
{
  _max_pos[0]=max_point.x();
  _max_pos[1]=max_point.y();
  if(_max_pos[0] < _min_pos[0]){
    _min_pos[0]=_max_pos[0];
  }
  if(_max_pos[1] < _min_pos[1]){
    _min_pos[1]=_max_pos[1];
  }
}

template <class Type>
void vgl_box_2d<Type>::set_centroid(vgl_point_2d<Type> const& centroid)
{
  set_centroid_x(centroid.x());
  set_centroid_y(centroid.y());
}

