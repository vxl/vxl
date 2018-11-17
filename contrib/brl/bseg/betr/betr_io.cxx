#include "betr_io.h"
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>
bool betr_io::read_so_ply(const std::string& ply_path, vsol_spatial_object_3d_sptr& obj){
  auto* mesh = new bmsh3d_mesh_mc();
  bool good = bmsh3d_load_ply(mesh, ply_path.c_str());
  if(!good){
    std::cout << "Failed to load mesh_3d from ply file - fatal!\n";
    return false;
  }
  // determine if there is only one face, if so then create a vsol_polygon_3d instead of a mesh
  std::map<int, bmsh3d_face*>& fmap = mesh->facemap();
  if(fmap.size()==0){
    std::cout << " no faces in mesh - fatal!\n";
    return false;
  }
  if(fmap.size()==1){
    // not a mesh but a single polygon face
    bmsh3d_face* face = fmap[0];
    std::vector<vsol_point_3d_sptr> v_list;
    const std::vector<bmsh3d_vertex*>& verts = face->vertices();
        unsigned n = verts.size();
    for(unsigned i = 0; i<n; ++i){
      vgl_point_3d<double> p = verts[i]->get_pt();
      v_list.push_back(new vsol_point_3d (p.x(), p.y(), p.z()));
    }
    obj = new vsol_polygon_3d(v_list);
    return true;
  }
  auto* vmesh = new vsol_mesh_3d();
  vmesh->set_mesh(mesh);
  obj = vmesh;
  return true;
}
