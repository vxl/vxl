//this-sets-emacs-to-*-c++-*-mode
#ifndef segv_vtol_soview2D_h_
#define segv_vtol_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A collection of vgui_soview2D objects that form views of 
//        vtol topology objects
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 28, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vgui/vgui_soview2D.h>

//: vtol_vertex_2d 
// This view is essentially the same as a point the only difference
// would be style, which we will tackle later
class segv_vtol_soview2D_vertex : public vgui_soview2D_point
{
 public:
  //: Constructor - creates a default vertex_2d view
  segv_vtol_soview2D_vertex() {}

  //: Print details about this vtol_vertex_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('segv_vtol_soview2D_vertex').
  vcl_string type_name() const { return "segv_vtol_soview2D_vertex"; }

};

#endif // segv_vtol_soview2D_h_
