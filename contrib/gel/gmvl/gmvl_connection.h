// This is gel/gmvl/gmvl_connection.h
#ifndef gmvl_connection_h_
#define gmvl_connection_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim

#include <vcl_iosfwd.h>
#include <vbl/vbl_ref_count.h>
#include <gmvl/gmvl_node_sptr.h>

class gmvl_connection : public vbl_ref_count
{
 public:
  // constructors / destructors

  gmvl_connection( const gmvl_node_sptr node1, const gmvl_node_sptr node2);
  gmvl_connection(gmvl_connection const& x)
    : vbl_ref_count(), n1_(x.n1_), n2_(x.n2_) {}
  ~gmvl_connection();

  // accessors

  gmvl_node_sptr get_node1() const { return n1_; }
  gmvl_node_sptr get_node2() const { return n2_; }

  friend vcl_ostream &operator<<( vcl_ostream &os, const gmvl_connection &c);

 protected:
  gmvl_node_sptr n1_, n2_;
};

vcl_ostream &operator<<( vcl_ostream &os, const gmvl_connection &c);

#endif // gmvl_connection_h_
