#ifndef bwm_observable_textured_mesh_h_
#define bwm_observable_textured_mesh_h_
//:
// \file

#include <iostream>
#include <iosfwd>
#include "bwm_observable_mesh.h"

#include <bmsh3d/bmsh3d_textured_mesh_mc.h>
#include <bmsh3d/bmsh3d_textured_face_mc.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bwm_observable_textured_mesh : public bwm_observable_mesh
{
 public:

  std::string type_name() const {return "bwm_observable_textured_mesh"; }

  //: constructors
  bwm_observable_textured_mesh()
    : object_(nullptr)/*, base_(0)*/ {}

  bwm_observable_textured_mesh(bmsh3d_textured_mesh_mc* object)
    : object_(object) {}

  virtual ~bwm_observable_textured_mesh();

  std::string tex_map_uri() { bmsh3d_textured_face_mc* face = static_cast<bmsh3d_textured_face_mc*>(object_->facemap(0));return face->tex_map_uri(); }

  void save_gml(std::ostream &os, int obj_count, vpgl_lvcs* lvcs);

  void save_kml(std::ostream &os, int obj_count, vpgl_lvcs* lvcs,
                double ground_height, double x_offset, double y_offset );

  void save_kml_collada(std::ostream &os, vpgl_lvcs* lvcs,
                        std::string geometry_id,
                        std::string geometry_position_id,
                        std::string geometry_position_array_id,
                        std::string geometry_uv_id,
                        std::string geometry_uv_array_id,
                        std::string geometry_vertex_id,
                        std::string material_name);

  void save_x3d(std::ostream &os, vpgl_lvcs* lvcs);

 private:

  bmsh3d_textured_mesh_mc* object_;
};

#endif
