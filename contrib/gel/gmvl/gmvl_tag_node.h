#ifndef gmvl_tag_node_h_
#define gmvl_tag_node_h_
#ifdef __GNUC__
#pragma interface "gmvl_tag_node"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_string.h>
#include <gmvl/gmvl_node.h>

class gmvl_tag_node : public gmvl_node
{
public:

  gmvl_tag_node( const vcl_string name);
  ~gmvl_tag_node();

protected:

  vcl_string name_;

};

#endif
