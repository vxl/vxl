#ifndef vcl_iso_iosfwd_h_
#define vcl_iso_iosfwd_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <iosfwd>

// istream
#ifndef vcl_istream
#define vcl_istream std::istream
#endif
// ostream
#ifndef vcl_ostream
#define vcl_ostream std::ostream
#endif
// iostream
#ifndef vcl_iostream
#define vcl_iostream std::iostream
#endif
// fstream
#ifndef vcl_fstream
#define vcl_fstream std::fstream
#endif
// ifstream
#ifndef vcl_ifstream
#define vcl_ifstream std::ifstream
#endif
// ofstream
#ifndef vcl_ofstream
#define vcl_ofstream std::ofstream
#endif

// 
using std::istream;
using std::ostream;
using std::iostream;
using std::fstream;
using std::ifstream;
using std::ofstream;

#endif
