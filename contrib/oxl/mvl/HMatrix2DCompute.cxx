//--------------------------------------------------------------
//
// Class : HMatrix2DCompute
//
// Modifications : see HMatrix2DCompute.h
//
//--------------------------------------------------------------

#include "HMatrix2DCompute.h"

#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_iostream.h>

#include <mvl/HMatrix2D.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/PairMatchSetCorner.h>
#include <vcl/vcl_vector.h>

//--------------------------------------------------------------------------------
//
//  misc. stuff
//
//--------------------------------------------------------------------------------

void HMatrix2DCompute::verbose(bool v) { verbose_=v; }

//--------------------------------------------------------------------------------
//
//  implementations of virtual functions :
//
//--------------------------------------------------------------------------------

bool
HMatrix2DCompute::compute_p(const PointArray&p1,
			    const PointArray&p2, 
			    HMatrix2D * H) 
{
  // default implementation :
  LineArray nolines;
  return compute_pl(p1,p2,nolines,nolines,H);
}

bool
HMatrix2DCompute::compute_l(const LineArray&l1,
			    const LineArray&l2,
			    HMatrix2D * H) 
{
  // default implementation :
  PointArray nopoints;
  return compute_pl(nopoints,nopoints,l1,l2,H);
}

bool
HMatrix2DCompute::compute_pl(const PointArray&,
			     const PointArray&, 
			     const LineArray&,
			     const LineArray&,
			     HMatrix2D *) 
{
  cerr << endl;
  cerr << "HMatrix2DCompute::compute_pl() :" << endl;
  cerr << "This is a virtual method which should have been" << endl;
  cerr << "overridden by a class derived from HMatrix2DCompute." << endl;
  cerr << "The derived class may have omitted to implement" << endl;
  cerr << "enough of the methods compute_p(),compute_l() and" << endl;
  cerr << "compute_pl()." << endl;
  abort();
  return false;
}

//--------------------------------------------------------------------------------
//
//  the remaining functions just call the above functions.
//
//--------------------------------------------------------------------------------

bool
HMatrix2DCompute::compute(const PointArray&p1,
			  const PointArray&p2,
			  HMatrix2D * H) 
{
  return compute_p(p1,p2,H);
}

HMatrix2D
HMatrix2DCompute::compute(const PointArray&p1,
			  const PointArray&p2) 
{
  HMatrix2D H;
  compute_p(p1,p2,&H);
  return H;
}

//--------------------------------------------------------------------------------

bool
HMatrix2DCompute::compute(const LineArray&l1, 
			  const LineArray&l2,
			  HMatrix2D * H) 
{
  return compute_l(l1,l2,H);
}

HMatrix2D
HMatrix2DCompute::compute(const LineArray&l1, 
			  const LineArray&l2) 
{
  HMatrix2D H;
  compute_l(l1,l2,&H);
  return H;
}

//--------------------------------------------------------------------------------

bool
HMatrix2DCompute::compute(const PointArray&p1,
			  const PointArray&p2,
			  const LineArray&l1,
			  const LineArray&l2,
			  HMatrix2D*H) 
{
  return compute_pl(p1,p2,l1,l2,H);
}

HMatrix2D
HMatrix2DCompute::compute(const PointArray&p1,
			  const PointArray&p2,
			  const LineArray&l1,
			  const LineArray &l2) 
{
  HMatrix2D H;
  compute_pl(p1,p2,l1,l2,&H);
  return H;
}

//--------------------------------------------------------------------------------
  
bool 
HMatrix2DCompute::compute(const PairMatchSetCorner &matches,
				   HMatrix2D *H) 
{
  vcl_vector<HomgPoint2D> pts1(matches.count());
  vcl_vector<HomgPoint2D> pts2(matches.count());
  matches.extract_matches(pts1, pts2);
  return compute(pts1,pts2,H);
}

HMatrix2D 
HMatrix2DCompute::compute(const PairMatchSetCorner &matches) 
{
  HMatrix2D H;
  compute(matches,&H);
  return H;
}

//--------------------------------------------------------------------------------
