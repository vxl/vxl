//this-sets-emacs-to-*-c++-*-mode
#ifndef bxml_vtol_io_h_
#define bxml_vtol_io_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A xml process class for converting aggregates of objects
//        
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 8, 2002    Initial version.
//   Based on the original TargetJr design 
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vtol/vtol_edge_2d_sptr.h>
class bxml_vtol_io 
{
 public:

  ~bxml_vtol_io();

  // input methods
  static bool read_edges(vcl_string& xml_file_name,
                        vcl_vector<vtol_edge_2d_sptr>& edges );

 protected:
  bxml_vtol_io();
};

#endif // bxml_vtol_io_h_
