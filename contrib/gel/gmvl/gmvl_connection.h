#ifndef gmvl_connection_h_
#define gmvl_connection_h_
#ifdef __GNUC__
#pragma interface "gmvl_connection"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <gmvl/gmvl_node_ref.h>

class gmvl_connection : public vbl_ref_count
{
public:

  gmvl_connection( const gmvl_node_ref node1, const gmvl_node_ref node2);
  ~gmvl_connection();

  friend ostream &operator<<( ostream &os, const gmvl_connection &c);

protected:

  gmvl_node_ref n1_, n2_;

};

ostream &operator<<( ostream &os, const gmvl_connection &c);

#endif
