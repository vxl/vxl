// This is mul/mbl/mbl_read_str.cxx
#include "mbl_read_str.h"
//:
// \file
// \brief Ask String question.

//  Copyright: (C) 1994 Victoria University of Manchester
//
// ********************************************************************
//
// HISTORY:
//    Based on code written 7-May-92
//  1.1   Tim   TC_ version 4-Nov-98
// ********************************************************************

#include <vcl_cstdio.h>
#include <vcl_cstring.h>

//: Asks question and waits for an answer from standard input.
//       If the answer is a non-empty string, returns it.
//       If the answer is an empty vcl_string (return)
//       then returns default.
// \param reply Space for reply
// \param max_str_len Length of space for reply
// \param q_str A question 
// \param default_str Default answer
// \return reply: The answer or a default

//   Example: 
// \verbatim 
// a)
// const int N_max_len = 20;
// char* Name[N_max_len];
// mbl_read_str(Name,N_max_len,"Enter Name","Fred");
// 
// b) If default_str = current reply, then reply
//   is only overwritten if a different
//   vcl_string given.
// 
// const int N_max_len = 20;
// char* Name[N_max_len];
// strcpy(Name,"Freddy");
// mbl_read_str(Name,N_max_len,"Enter Name",Name);
// \endverbatim
//
// Modifications
// \verbatim 
//   1.1   Tim   New version 4-Nov-98
//
// Converted to vxl by gvw, soblamemewhydontcha!
// \endverbatim
char* mbl_read_str(char *reply, int max_str_len, const char *q_str, const char *default_str)
{
  char *new_reply = new char[max_str_len];

  vcl_printf("%s (%s) :",q_str,default_str);

  // Now read in a line of text
  vcl_fgets(new_reply,max_str_len,stdin);
  if (new_reply[0]=='\n')
  {
    if (reply!=default_str)
      vcl_strncpy(reply,default_str,max_str_len);
  }
  else
  {
    int i=0;
    while (new_reply[i]!='\n')  i++;
    new_reply[i]='\0';
    // Replace the '\n' by a '\0' to remove it from end of new_reply
    vcl_strncpy(reply,new_reply,max_str_len);
  }

  delete [] new_reply;
  return reply;
}

