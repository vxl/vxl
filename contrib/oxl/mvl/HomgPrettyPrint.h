//-*- c++ -*-------------------------------------------------------------------
#ifndef HomgPrettyPrint_h_
#define HomgPrettyPrint_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : HomgPrettyPrint
//
// .SECTION Description
//    HomgPrettyPrint is a class that awf hasn't documented properly. FIXME
//
// .NAME        HomgPrettyPrint - Undocumented class FIXME
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/HomgPrettyPrint.h
// .FILE        HomgPrettyPrint.h
// .FILE        HomgPrettyPrint.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 12 Oct 97
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

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS HomgPrettyPrint.

