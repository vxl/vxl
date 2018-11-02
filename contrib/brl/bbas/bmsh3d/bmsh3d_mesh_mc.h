// This is brl/bbas/bmsh3d/bmsh3d_mesh_mc.h
//---------------------------------------------------------------------
#ifndef bmsh3d_mesh_mc_h_
#define bmsh3d_mesh_mc_h_
//:
// \file
// \brief multiple connected (mc) mesh for representing mesh face with internal holes or curves.
//
// \author
//  Gamze Tunali  Jan 2, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "bmsh3d_mesh.h"
#include "bmsh3d_face_mc.h"
class bmsh3d_edge;

//: The mesh class that handles indexed face set.
class bmsh3d_mesh_mc : public bmsh3d_mesh
{
 public:
  bmsh3d_mesh_mc() : bmsh3d_mesh(), id_counter_(0) {}

  bmsh3d_mesh_mc(bmsh3d_mesh*);

  ~bmsh3d_mesh_mc() override;

  //: new/delete function of the class hierarchy
  bmsh3d_face_mc* _new_face () override {
    return new bmsh3d_face_mc (face_id_counter_++);
  }

  bmsh3d_mesh_mc* clone() const override;

  void orient_face_normals();

  void add_inner_block(bmsh3d_face_mc* face) { set_face_[id_counter_++]=face; }

  std::map<int, bmsh3d_face_mc*> get_inner_blocks() { return set_face_; }

 private:
  std::map<int, bmsh3d_face_mc*> set_face_;
  int id_counter_;
};

//: breaks a face into two on edges E1 and E2 and returns the two new faces F1 and F2
void mesh_break_face(bmsh3d_mesh_mc* M, bmsh3d_face_mc* F,
                     bmsh3d_edge* E1, bmsh3d_edge* E2,
                     bmsh3d_vertex* V1, bmsh3d_vertex* V2,
                     bmsh3d_face_mc* &F1, bmsh3d_face_mc* &F2);

//: merges two meshes M1 amd M2 into M1. M1 and M2 do not have any intersection
void merge_mesh (bmsh3d_mesh_mc* M1, bmsh3d_mesh_mc* M2);
bmsh3d_face* copy_inner_face(std::vector<bmsh3d_edge*> incident_edges,
                             bmsh3d_mesh_mc* mesh);

#endif // bmsh3d_mesh_mc_h_
