// This is mul/mbl/mbl_read_str.cxx
#include <iostream>
#include <cstdio>
#include <cstring>
#include "mbl_read_str.h"
//:
// \file
// \brief Ask String question.
//
//  Copyright: (C) 1994 Victoria University of Manchester

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

char* mbl_read_str(char *reply, int max_str_len, const char *q_str, const char *default_str)
{
  char *new_reply = new char[max_str_len];

  std::printf("%s (%s) :",q_str,default_str);

  // Now read in a line of text
  if (!std::fgets(new_reply,max_str_len,stdin))
  {
    std::strncpy(reply,"*** Error from std::fgets() ***",max_str_len);
  }
  else if (new_reply[0]=='\n')
  {
    if (reply!=default_str)
      std::strncpy(reply,default_str,max_str_len);
  }
  else
  {
    int i=0;
    while (new_reply[i]!='\n')  i++;
    new_reply[i]='\0';
    // Replace the '\n' by a '\0' to remove it from end of new_reply
    std::strncpy(reply,new_reply,max_str_len);
  }

  delete [] new_reply;
  return reply;
}
