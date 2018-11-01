// This is mul/mbl/mbl_read_int.cxx
//:
// \file
// \brief Asks question and waits for an answer
// \author tim, updated by me apparently!!!
//  updated to vxl caringly in the way one can only do on a Friday afternoon by gvw
//
// - Function Name: mbl_read_int
// - Synopsis:      int mbl_read_int(char* q_str, int default_int)
// - Inputs:        q_str: A question
//                  default_int: Default answer
//                  min_int: Min allowed value (optional)
//                  max_int: Max allowed value (optional)
// - Outputs:       -
// - Returns:       The answer or a default
// - Description:   Asks question and waits for an answer.
//                  If the answer is an integer, returns it.
//                  If the answer is an empty std::string (return)
//                  then returns default.
//                  Otherwise waits for another input.
// - References:    -
// - Example:
// \code
//    int new_scale = mbl_read_int("Scale?",5);
//    int new_scale = mbl_read_int("Scale?",5,min_scale,max_scale);
// \endcode

#include <iostream>
#include <cstdio>
#include "mbl_read_int.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

constexpr int MAX_LEN = 20;

// If min_int != 0 or max_int != 0 then prints range but doesn't check that reply is in range
int RD_ReadInt1(const char* q_str, int default_int,
                int min_int, int max_int)
{
  char reply[MAX_LEN];

  while (true)
  {
    if (min_int==0 && max_int==0)
      std::cout<<q_str<<" ("<<default_int<<") :";
    else
      std::cout<<q_str<<" ["<<min_int<<".."<<max_int<<"] ("<<default_int<<") :";
    std::cout.flush();

    if (std::fgets(reply,MAX_LEN,stdin)!=nullptr)
    {
      int r = default_int;
      if (reply[0]=='\n' || std::sscanf(reply,"%d",&r)>0)
        return r;
    }
  }
}

int mbl_read_int(const char* q_str, int default_int)
{
  return RD_ReadInt1(q_str,default_int,0,0);
}

int mbl_read_int(const char* q_str, int default_int,
                 int min_int, int max_int)
{
  while (true)
  {
    int R = RD_ReadInt1(q_str,default_int,min_int,max_int);
    if (R<min_int)
      std::cout<<R<<": must be at least "<<min_int<<"\n";
    else if (R>max_int)
      std::cout<<R<<": must be no more than "<<max_int<<"\n";
    else
      return R; // acceptable
  }
}
