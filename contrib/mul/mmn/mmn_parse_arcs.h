#ifndef mmn_parse_arcs_h_
#define mmn_parse_arcs_h_
//:
// \file
// \brief Parse in a set of arc definitions using names of nodes
// \author Martin Roberts

#include <mmn/mmn_arc.h>
#include <vcl_compiler.h>
#include <iostream>
#include <istream>
#include <vector>

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
void  mmn_parse_arcs(std::istream& is,
                     const std::vector<std::string>& node_names,
                     std::vector<mmn_arc>& arcs);

#endif // mmn_parse_arcs_h_

