// This is gel/vdgl/vdgl_edgel_chain.h
#ifndef vdgl_edgel_chain_h
#define vdgl_edgel_chain_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Represents an edgel list which is a 2d set of edgels
// \author Geoff Cross
//
// \verbatim
//  Modifications:
//   10-Apr-2002 Peter Vanroose - Added & implemented split(), extract_subchain()
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>

#include "vdgl_edgel_chain_sptr.h"
#include <vdgl/vdgl_edgel.h>

class vdgl_edgel_chain : public vul_timestamp,
                         public vbl_ref_count
{
   // PUBLIC INTERFACE----------------------------------------------------------
 public:

  // Constructors/Destructors--------------------------------------------------
  vdgl_edgel_chain();
  vdgl_edgel_chain( const vcl_vector<vdgl_edgel> edgels);
  //: make a straight edgel chain from p0 to p1.
  //  Useful for boundary filling and debugging
  vdgl_edgel_chain( const double x0, const double y0, 
                    const double x1, const double y1);
  ~vdgl_edgel_chain();

  // Operators----------------------------------------------------------------

  bool add_edgel( const vdgl_edgel &e);
  bool add_edgels( const vcl_vector<vdgl_edgel> &es, const int index);
  bool set_edgel( const int index, const vdgl_edgel &e);
  //: return a new edgel_chain, containing the edgels numbered start up to end.
  vdgl_edgel_chain_sptr extract_subchain(int start, int end);
  //: split the current edgel_chain at or around the given point (x,y)
  bool split( double x, double y,
              vdgl_edgel_chain_sptr &ec1, vdgl_edgel_chain_sptr &ec2);

  friend vcl_ostream& operator<<(vcl_ostream& s, const vdgl_edgel_chain& p);

  // Data Access---------------------------------------------------------------

  int size() const { return es_.size(); }
  vdgl_edgel& edgel( int i) { return es_[i]; }
  vdgl_edgel const& edgel( int i) const { return es_[i]; }
  vdgl_edgel& operator[]( int i) { return es_[i]; }
  vdgl_edgel const& operator[]( int i) const { return es_[i]; }

  //: should call this if one of the edgels is likely to have changed
  void notify_change();

  // INTERNALS-----------------------------------------------------------------
 protected:

  // Utility functions

  //: Advance along a line and generate contiguous pixels on the line.
  bool line_gen(double xs, double ys, double xe, double ye,
                bool& init, bool& done, double& x, double& y);

  // Data Members--------------------------------------------------------------

  vcl_vector<vdgl_edgel> es_;
};

vcl_ostream& operator<<(vcl_ostream& s, const vdgl_edgel_chain& p);

#endif // vdgl_edgel_chain_h
