#ifndef gmvl_connection_h_
#define gmvl_connection_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <gmvl/gmvl_node_sptr.h>

class gmvl_connection : public vbl_ref_count
{
public:

  // constructors / destructors

  gmvl_connection( const gmvl_node_sptr node1, const gmvl_node_sptr node2);
  ~gmvl_connection();

  // accessors

  gmvl_node_sptr get_node1() const { return n1_; }
  gmvl_node_sptr get_node2() const { return n2_; }

  friend vcl_ostream &operator<<( vcl_ostream &os, const gmvl_connection &c);

protected:

  gmvl_node_sptr n1_, n2_;

};

vcl_ostream &operator<<( vcl_ostream &os, const gmvl_connection &c);

#endif
