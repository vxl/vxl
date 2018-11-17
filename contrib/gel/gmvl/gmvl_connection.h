// This is gel/gmvl/gmvl_connection.h
#ifndef gmvl_connection_h_
#define gmvl_connection_h_
//:
// \file
// \author crossge@crd.ge.com
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <gmvl/gmvl_node_sptr.h>

class gmvl_connection : public vbl_ref_count
{
 public:
  // constructors / destructors

  gmvl_connection( const gmvl_node_sptr& node1, const gmvl_node_sptr& node2);
  gmvl_connection(gmvl_connection const& x)
    : vbl_ref_count(), n1_(x.n1_), n2_(x.n2_) {}
  ~gmvl_connection() override;

  // accessors

  gmvl_node_sptr get_node1() const { return n1_; }
  gmvl_node_sptr get_node2() const { return n2_; }

  friend std::ostream &operator<<( std::ostream &os, const gmvl_connection &c);

 protected:
  gmvl_node_sptr n1_, n2_;
};

std::ostream &operator<<( std::ostream &os, const gmvl_connection &c);

#endif // gmvl_connection_h_
