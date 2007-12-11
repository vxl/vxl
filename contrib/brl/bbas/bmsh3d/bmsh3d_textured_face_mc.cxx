// This is brl/bbas/bmsh3d/bmsh3d_textured_face_mc.cxx
//---------------------------------------------------------------------
#include "bmsh3d_textured_face_mc.h"
//:
// \file
// \brief A textured mesh
//
// \author
//  Daniel Crispell  March 14, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include "bmsh3d_vertex.h"
#include <vgl/vgl_point_2d.h>

#include <vcl_iostream.h>

bmsh3d_textured_face_mc::bmsh3d_textured_face_mc(const bmsh3d_face* face, vcl_string tex_map_uri)
: bmsh3d_face_mc(face->id())
{
  tex_map_uri_ = tex_map_uri;

  // copy vertices
  vcl_vector<bmsh3d_vertex*> old_verts = face->vertices();
  for (unsigned v = 0; v < old_verts.size(); v++) {
    bmsh3d_vertex* vert = (bmsh3d_vertex*)old_verts[v];
    _add_vertex(vert);
  }
}


void bmsh3d_textured_face_mc::print()
{
  unsigned i = 0;
  while (i < set_he_.size()) {
    bmsh3d_halfedge* HE = set_he_[i++];
    bmsh3d_halfedge* he = HE;
    do {
      bmsh3d_vertex* s =  HE->edge()->sV();
      bmsh3d_vertex* e =  HE->edge()->eV();
      vgl_point_2d<double> s_tex = tex_coords_[s->id()];
      vgl_point_2d<double> e_tex = tex_coords_[e->id()];
      vcl_cout << "S=" << HE->edge()->sV() << ' ' << s->get_pt() << "tex= " << s_tex << vcl_endl
               << "E=" << HE->edge()->eV() << ' ' << e->get_pt() << "tex= " << e_tex << vcl_endl;
      HE = HE->next();
    } while (HE != he);
  }
}

