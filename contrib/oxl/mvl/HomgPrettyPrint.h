// This is oxl/mvl/HomgPrettyPrint.h
#ifndef HomgPrettyPrint_h_
#define HomgPrettyPrint_h_
//:
// \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 12 Oct 97
//
// \verbatim
// Modifications:
//   971012 AWF Initial version.
// \endverbatim
//
//-----------------------------------------------------------------------------

class HomgPrettyPrint
{
  // Data Members--------------------------------------------------------------
  bool last_pretty_;

 public:
  // Constructors/Destructors--------------------------------------------------

  HomgPrettyPrint(bool new_value) { last_pretty_ = pretty; pretty = new_value; }
  ~HomgPrettyPrint() { pretty = last_pretty_; }

  static bool pretty;
};

#endif // HomgPrettyPrint_h_
