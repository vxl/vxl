#ifndef vdgl_edgel_chain_h
#define vdgl_edgel_chain_h
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vdgl_edgel_chain - Represents an edgel list
// .INCLUDE vgl/vdgl_edgel_chain.h
// .FILE vdgl_edgel_chain.txx
//
// .SECTION Description
//  A 2d set of edgels
//
// .SECTION Author
//    Geoff Cross
// Created: xxx xx xxxx

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vbl/vbl_timestamp.h>
#include <vbl/vbl_ref_count.h>


#include "vdgl_edgel.h"

class vdgl_edgel_chain : public vbl_timestamp, 
			 public vbl_ref_count {
   // PUBLIC INTERFACE----------------------------------------------------------
public:
  
  // Constructors/Destructors--------------------------------------------------
  vdgl_edgel_chain();
  vdgl_edgel_chain( const vcl_vector<vdgl_edgel> edgels);

  ~vdgl_edgel_chain();

  // Operators----------------------------------------------------------------

  bool add_edgel( const vdgl_edgel &e);
  bool add_edgels( const vcl_vector<vdgl_edgel> &es, const int index);
  bool set_edgel( const int index, const vdgl_edgel &e);

  friend vcl_ostream& operator<<(vcl_ostream& s, const vdgl_edgel_chain& p);

  // Data Access---------------------------------------------------------------

  int size() const { return es_.size(); }
  vdgl_edgel edgel( int i) const { return es_[i]; }
  vdgl_edgel &operator[]( int i) { return es_[i]; }
  
  // should call this if one of the edgels is likely to have changed
  void notify_change();
  
  // Data Control--------------------------------------------------------------

  // Computations--------------------------------------------------------------

  
  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------
  
  vcl_vector<vdgl_edgel> es_;

private:
  // Helpers-------------------------------------------------------------------
};

vcl_ostream& operator<<(vcl_ostream& s, const vdgl_edgel_chain& p) ;


#endif // _blank_file_h
