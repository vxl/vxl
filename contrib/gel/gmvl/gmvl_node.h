#ifndef gmvl_node_h_
#define gmvl_node_h_
#ifdef __GNUC__
#pragma interface "gmvl_node"
#endif
/*
  crossge@crd.ge.com
*/

/* Types of node:
        images
        image features (points, lines, conics...)
        transformations (homographies, projection matrices...)
        coordinate systems (?)
*/

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_string.h>
#include <vbl/vbl_ref_count.h>

class gmvl_node_cache;

class gmvl_node : public vbl_ref_count
{
public:

  gmvl_node( const vcl_string name);
  virtual ~gmvl_node();

  vcl_string get_name() const { return name_; }

  friend ostream &operator<<( ostream &os, gmvl_node &node);

  // to allow the cache to access ref 
  friend gmvl_node_cache;

protected:

  // type name (name of class)
  vcl_string type_;

  // name of object (for instance "input image")
  vcl_string name_;
  
  // reference number (used by caches)
  int ref_;
};

ostream &operator<<( ostream &os, gmvl_node &node);

#endif
