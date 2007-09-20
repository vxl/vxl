#ifndef vil_chord_h_
#define vil_chord_h_
//:
// \file
// \brief Object to store information about position of a row of pixels.
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vcl_vector.h>

//: Store information about position of a row of pixels in an image
//  Pixels are ([ilo,ihi],y)
struct vil_chord
{
  unsigned ilo;
  unsigned ihi;
  unsigned j;

  //: Default constructor
  vil_chord() : ilo(1),ihi(0),j(0) {}

  //: Construct
  vil_chord(unsigned ilo1, unsigned ihi1, unsigned j1)
    : ilo(ilo1), ihi(ihi1), j(j1) {};

  //: length == number of pixels
  unsigned length() const { return ihi+1-ilo; }
};

//: Print to stream
inline vcl_ostream& operator<<(vcl_ostream& os, vil_chord c)
{
  os<<"(["<<c.ilo<<","<<c.ihi<<"],"<<c.j<<")";
}

//: Compute area of region defined by (non-overlapping) chords
inline unsigned vil_area(const vcl_vector<vil_chord>& region)
{
  unsigned A=0;
  for (unsigned i=0;i<region.size();++i) A+=region[i].length();
  return A;
}

#endif 
