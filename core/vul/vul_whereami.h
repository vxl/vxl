// This is core/vul/vul_whereami.h
#ifndef vul_whereami_h_
#define vul_whereami_h_

//:
// \file
// \author fsm

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define vul_whereami (std::cerr << __FILE__ ", line " << __LINE__ << " : ")

#endif // vul_whereami_h_
