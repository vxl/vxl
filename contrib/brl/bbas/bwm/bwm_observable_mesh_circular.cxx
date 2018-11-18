#include "bwm_observable_mesh_circular.h"

void bwm_observable_mesh_circular::decrease_radius(double p)
{
  // get the vertices
  std::map<int, bmsh3d_vertex* > vertices = object_->vertexmap();
  std::vector<vgl_point_3d<double> > new_vertices;
  std::map<int, bmsh3d_vertex* >::iterator v_it = vertices.begin();
  while (v_it != vertices.end()) {
    bmsh3d_vertex* vertex = (bmsh3d_vertex*) v_it->second;
    vgl_point_3d<double> point(vertex->get_pt());
    double x,y;
    x = center_.x() + (point.x() - center_.x())*(1-p);
    y = center_.y() + (point.y() - center_.y())*(1-p);
    vertex->set_pt(vgl_point_3d<double>(x,y,point.z()));
    v_it++;
  }
  r_ *= (1-p);
  this->notify_observers("update");
}


void bwm_observable_mesh_circular::increase_radius(double p)
{
  // get the vertices
  std::map<int, bmsh3d_vertex* > vertices = object_->vertexmap();
  std::vector<vgl_point_3d<double> > new_vertices;
  std::map<int, bmsh3d_vertex* >::iterator v_it = vertices.begin();
  while (v_it != vertices.end()) {
    bmsh3d_vertex* vertex = (bmsh3d_vertex*) v_it->second;
    vgl_point_3d<double> point(vertex->get_pt());
    double x,y;
    x = center_.x() + (point.x() - center_.x())*(1+p);
    y = center_.y() + (point.y() - center_.y())*(1+p);
    vertex->set_pt(vgl_point_3d<double>(x,y,point.z()));
    v_it++;
  }
  r_ *= (1+p);
  this->notify_observers("update");
}
