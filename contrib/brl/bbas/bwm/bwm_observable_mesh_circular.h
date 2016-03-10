#ifndef bwm_observable_mesh_circular_h_
#define bwm_observable_mesh_circular_h_

#include "bwm_observable_mesh.h"

class bwm_observable_mesh_circular : public bwm_observable_mesh
{
 public:
  virtual std::string type_name() const {return "bwm_observable_mesh_circular"; }

  bwm_observable_mesh_circular(bwm_observable_mesh const &mesh,
                               double r, vgl_point_2d<double> center)
    : bwm_observable_mesh(mesh), r_(r), center_(center) {}

  bwm_observable_mesh_circular(bwm_observable_mesh const &mesh)
    : bwm_observable_mesh(mesh) {}

  bwm_observable_mesh_circular(double r, vgl_point_2d<double> center)
    : r_(r), center_(center) {}

  void set(double r, vgl_point_2d<double> center) { r_= r, center_ = center;}

  ~bwm_observable_mesh_circular() {}

  void decrease_radius(double p);
  void increase_radius(double p);

 private:
  double r_;
  vgl_point_2d<double> center_;
};

#endif
