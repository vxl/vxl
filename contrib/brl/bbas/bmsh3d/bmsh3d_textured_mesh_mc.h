// This is brl/bbas/bmsh3d/bmsh3d_textured_mesh_mc.h
//---------------------------------------------------------------------
#ifndef _bmsh3d_textured_mesh_mc_h_
#define _bmsh3d_textured_mesh_mc_h_
//:
// \file
// \brief multiple connected (mc) mesh for representing textured mesh faces with internal holes or curves.
//
// \author
//  Daniel Crispell  March 20, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

//#include <vcl_map.h>
//#include <vcl_utility.h>

#include "bmsh3d_mesh.h"
#include "bmsh3d_mesh_mc.h"
#include "bmsh3d_textured_face_mc.h"

//: The mesh class that handles indexed face set.
class bmsh3d_textured_mesh_mc : public bmsh3d_mesh_mc
{
 public:
  bmsh3d_textured_mesh_mc() : bmsh3d_mesh_mc(){}

  bmsh3d_textured_mesh_mc(bmsh3d_mesh_mc* mesh);

  //: new/delete function of the class hierarchy
  virtual bmsh3d_textured_face_mc* _new_face () {
    return new bmsh3d_textured_face_mc (face_id_counter_++);
  }

  virtual bmsh3d_textured_mesh_mc* clone() const;
};


#endif

