#ifndef gmvl_node_h_
#define gmvl_node_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>

class gmvl_node_cache;
class gmvl_connection_cache;

//:
// Types of node:
//      images
//      image features (points, lines, conics...)
//      transformations (homographies, projection matrices...)
//      coordinate systems (?)

class gmvl_node : public vbl_ref_count
{
public:

  gmvl_node();
  virtual ~gmvl_node();

  // getter
  vcl_string &type() { return type_; }

  // input output
  friend ostream &operator<<( ostream &os, gmvl_node &node);

  // to allow the cache to access ref 
  friend class gmvl_node_cache;
  friend class gmvl_connection_cache;

protected:

  // type name (name of class)
  vcl_string type_;

  // reference number (used by caches)
  int ref_;
};

ostream &operator<<( ostream &os, gmvl_node &node);

#endif
