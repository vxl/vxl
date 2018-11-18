// This is mul/mbl/mbl_read_double.cxx
//:
// \file
// \brief Asks question and waits for an answer
// \author tim
// hand crafted into vxl by gvw
//
// - Function Name: mbl_read_double
// - Synopsis:      double mbl_read_double(char* q_str, double default_d)
// - Inputs:        q_str: A question
//                  default_d: Default answer
//                  min_d: Min allowed value (optional)
//                  max_d: Max allowed value (optional)
// - Outputs:       -
// - Returns:       The answer or a default
// - Description:   Asks question and waits for an answer.
//                  If the answer is a double, returns it.
//                  If the answer is an empty std::string (return)
//                  then returns default.
//                  Otherwise waits for another input.
// - References:    -
// - Example:
// \code
//    double new_scale = mbl_read_double("Scale?",1.00);
//    double new_scale = mbl_read_double("Scale?",1.00,min_scale,max_scale);
// \endcode

#include <iostream>
#include <cstdio>
#include "mbl_read_double.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

constexpr int MAX_LEN = 40;

// If min_d != 0 or max_d != 0 then prints range but doesn't check that reply is in range
double RD_ReadDouble1(const char *q_str, double default_d,
                      double min_d, double max_d)
{
  char reply[MAX_LEN];

  while (true)
  {
    if (min_d==0 && max_d==0)
      std::cout<<q_str<<" ("<<default_d<<") :";
    else
    std::cout<<q_str<<" ["<<min_d<<".."<<max_d<<"] ("<<default_d<<") :";
    std::cout.flush();

    if (std::fgets(reply,MAX_LEN,stdin)!=nullptr)
    {
      double r = default_d;
      if (reply[0]=='\n' || std::sscanf(reply,"%lf",&r)>0)
        return r;
    }
  }
}

double mbl_read_double(const char *q_str, double default_d)
{
  return RD_ReadDouble1(q_str,default_d,0,0);
}

double mbl_read_double( const char *q_str, double default_d,
                        double min_d, double max_d)
{
  while (true)
  {
    double R = RD_ReadDouble1(q_str,default_d,min_d,max_d);
    if (R<min_d)
      std::cout<<R<<": must be at least "<<min_d<<"\n";
    else if (R>max_d)
      std::cout<<R<<": must be no more than "<<max_d<<"\n";
    else
      return R; // acceptable
  }
}
