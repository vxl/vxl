#ifndef MatchSet_h_
#define MatchSet_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	MatchSet - Base class for correspondences
// .LIBRARY	MViewBasics
// .HEADER	MultiView Package
// .INCLUDE	mvl/MatchSet.h
// .FILE	MatchSet.cxx
//
// .SECTION Description
//    MatchSet is a base class for classes that store correspondences
//    between tuples of geometric entities.
//
//    Currently it does no more than define the constant MatchSet::NoMatch
//    which is used by the subclasses as a dummy index to signify that an
//    entity has no matching feature.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

class MatchSet {
public:
  enum { NoMatch = -1 };

// -- Return true if i is not the "NoMatch" value.
  static inline bool matchp(int i) { return i != NoMatch; }

  MatchSet() {}
  virtual ~MatchSet();
};

#endif // MatchSet_h_
