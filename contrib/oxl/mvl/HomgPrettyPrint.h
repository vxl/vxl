#ifndef HomgPrettyPrint_h_
#define HomgPrettyPrint_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	HomgPrettyPrint
// .LIBRARY	MViewBasics
// .HEADER	MultiView Package
// .INCLUDE	mvl/HomgPrettyPrint.h
// .FILE	HomgPrettyPrint.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 12 Oct 97
//
// .SECTION Modifications:
//   971012 AWF Initial version.
//
//-----------------------------------------------------------------------------

class HomgPrettyPrint {
public:
  // Constructors/Destructors--------------------------------------------------

  HomgPrettyPrint(bool new_value) {
    last_pretty_ = pretty;
    pretty = new_value;
  }
  ~HomgPrettyPrint() {
    pretty = last_pretty_;
  }

  static bool pretty;

protected:
  // Data Members--------------------------------------------------------------
  bool last_pretty_;

  // Helpers-------------------------------------------------------------------
};

#endif // HomgPrettyPrint_h_
