#ifndef mmn_parse_arcs_h_
#define mmn_parse_arcs_h_
//:
// \file
// \brief Parse in a set of arc definitions using names of nodes
// \author Martin Roberts

#include <mmn/mmn_arc.h>
#include <vcl_istream.h>
#include <vcl_vector.h>

// Expects "{ arc: { name1 name2 } arc: { name3 name 4 } }"
//Format
// \code
// arcs:
// {
//   arc: { LeftEye RightEye }
//   arc: { Nostrils Mouth }
//   arc: { LeftEyeBrow LeftEye }
//   arc: { RightEyeBrow RightEye }
// }
// \endcode
 //Note separating white space is needed after { and before } in arc definitions
 //Assumes names in node_names are in order of associated node numbers in
 //graph defined by these arcs
 //
void  mmn_parse_arcs(vcl_istream& is,
                     const vcl_vector<vcl_string>& node_names,
                     vcl_vector<mmn_arc>& arcs);

#endif // mmn_parse_arcs_h_

