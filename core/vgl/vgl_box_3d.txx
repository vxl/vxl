// Author: Don Hamilton, Peter Tu
// Created: Feb 15 2000


//--------------------------------------------------------------
//
// Class : vgl_box_3d
//
//--------------------------------------------------------------

#include <vgl/vgl_box_3d.h> 
#include <vgl/vgl_point_3d.h> 

// Constructors/Destructors--------------------------------------------------

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(Type const min_position[3], 
                             Type const max_position[3] )
{
  min_pos_[0]=min_position[0];
  min_pos_[1]=min_position[1];
  min_pos_[2]=min_position[2];

  max_pos_[0]=max_position[0];
  max_pos_[1]=max_position[1];
  max_pos_[2]=max_position[2];
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(vgl_point_3d<Type> const& min_pos,
                             vgl_point_3d<Type> const& max_pos)
{
  min_pos_[0]=min_pos.x();
  min_pos_[1]=min_pos.y();
  min_pos_[2]=min_pos.z();

  max_pos_[0]=max_pos.x();
  max_pos_[1]=max_pos.y();
  max_pos_[2]=max_pos.z();
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(Type xmin, Type ymin, Type zmin,
                             Type xmax, Type ymax, Type zmax)
{
  min_pos_[0]=xmin;
  min_pos_[1]=ymin;
  min_pos_[2]=zmin;
  
  max_pos_[0]=xmax;
  max_pos_[1]=ymax;
  max_pos_[2]=zmax;
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const Type centroid[3],
                             Type width, Type height, Type depth)
{
  min_pos_[0]=centroid[0]-0.5*width;
  min_pos_[1]=centroid[1]-0.5*height;
  min_pos_[2]=centroid[2]-0.5*height;

  max_pos_[0]=centroid[0]+0.5*width;
  max_pos_[1]=centroid[1]+0.5*height;
  max_pos_[2]=centroid[2]+0.5*depth;
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(vgl_point_3d<Type> const& centroid,
                             Type width, Type height, Type depth)
{
  min_pos_[0]=centroid.x()-0.5*width;
  min_pos_[1]=centroid.y()-0.5*height;
  max_pos_[1]=centroid.z()-0.5*depth;

  max_pos_[0]=centroid.x()+0.5*width;
  max_pos_[1]=centroid.y()+0.5*height;
  max_pos_[1]=centroid.z()+0.5*depth;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_x(Type centroid_x)
{
  Type delta = centroid_x - get_centroid_x();
  min_pos_[0]= min_pos_[0] + delta;
  max_pos_[0]= max_pos_[0] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_y(Type centroid_y)
{
  Type delta = centroid_y - get_centroid_y();
  min_pos_[1]= min_pos_[1] + delta;
  max_pos_[1]= max_pos_[1] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_z(Type centroid_z)
{
  Type delta = centroid_z - get_centroid_z();
  min_pos_[2]= min_pos_[2] + delta;
  max_pos_[2]= max_pos_[2] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_width(const Type width)
{
  Type x = get_centroid_x();
  min_pos_[0] = x-0.5*width;
  max_pos_[0] = x+0.5*width;
}

template <class Type> 
void vgl_box_3d<Type>::set_height(const Type height)
{
  Type y = get_centroid_y();
  min_pos_[1] = y-0.5*height;
  max_pos_[1] = y+0.5*height;
}

template <class Type>
void vgl_box_3d<Type>::set_depth(const Type depth)
{
  Type z = get_centroid_z();
  min_pos_[2] = z-0.5*depth;
  max_pos_[2] = z+0.5*depth;
}

template <class Type>
void vgl_box_3d<Type>::set_min_point(vgl_point_3d<Type> const& min_point)
{
  min_pos_[0]=min_point.x();
  min_pos_[1]=min_point.y();
  min_pos_[2]=min_point.z();
 
  if(max_pos_[0] < min_pos_[0]){
    max_pos_[0]=min_pos_[0];
  }
  if(max_pos_[1] < min_pos_[1]){
    max_pos_[1]=min_pos_[1];
  }
  if(max_pos_[2] < min_pos_[2]){
    max_pos_[2]=min_pos_[2];
  }
}

template <class Type>
void vgl_box_3d<Type>::set_max_point(vgl_point_3d<Type> const& max_point)
{
  max_pos_[0]=max_point.x();
  max_pos_[1]=max_point.y();
  max_pos_[2]=max_point.z();

  if(max_pos_[0] < min_pos_[0]){
    min_pos_[0]=max_pos_[0];
  }
  if(max_pos_[1] < min_pos_[1]){
    min_pos_[1]=max_pos_[1];
  }
  if(max_pos_[2] < min_pos_[2]){
    min_pos_[2]=max_pos_[2];
  }
}

template <class Type>
void vgl_box_3d<Type>::set_centroid(vgl_point_3d<Type> const& centroid)
{
  set_centroid_x(centroid.x());
  set_centroid_y(centroid.y());
  set_centroid_z(centroid.z());
}
