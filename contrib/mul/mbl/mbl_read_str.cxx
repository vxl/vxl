/*
  Copyright: (C) 1994 Victoria University of Manchester
*/
/* ********************************************************************/
/*
  NAME : mbl_read_str 
  DESCRIPTION :
char* mbl_read_str(char *reply, int max_str_len, const char *q_str, const char *default_str)
    Asks q_str question.
    Returns the entered text, or the default if return hit.
     
  INPUTS : q_str        - Question
           default_str  - Default answer
  OUTPUTS :
    Returns the entered text, or the default if return hit.
  HISTORY:
      Based on code written 7-May-92
  1.1   Tim   TC_ version 4-Nov-98
*/
/* ********************************************************************/

#include <mbl/mbl_read_str.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>

char* mbl_read_str(char *reply, int max_str_len, const char *q_str, const char *default_str)
{
  char *new_reply = new char[max_str_len];
  int i;

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
    i=0; 
    while (new_reply[i]!='\n')  i++; 
    new_reply[i]='\0';
     // Replace the '/n' by a '/0' to remove it from end of new_reply
    vcl_strncpy(reply,new_reply,max_str_len);
  }

  delete [] new_reply;
  return reply;
}

