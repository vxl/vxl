// This is oxl/mvl/MatchSet.h
#ifndef MatchSet_h_
#define MatchSet_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base class for correspondences
//
//    MatchSet is a base class for classes that store correspondences
//    between tuples of geometric entities.
//
//    Currently it does no more than define the constant MatchSet::NoMatch
//    which is used by the subclasses as a dummy index to signify that an
//    entity has no matching feature.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim
//-----------------------------------------------------------------------------

class MatchSet
{
 public:
  enum { NoMatch = -1 };

//: Return true if i is not the "NoMatch" value.
  static inline bool matchp(int i) { return i != NoMatch; }

  MatchSet() {}
  virtual ~MatchSet() {}
};

#endif // MatchSet_h_
