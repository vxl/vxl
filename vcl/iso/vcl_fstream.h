#ifndef vcl_iso_fstream_h_
#define vcl_iso_fstream_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <fstream>

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

using std :: fstream;
using std :: ofstream;
using std :: ifstream;

#endif
