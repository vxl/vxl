#ifndef mbl_read_str_h_
#define mbl_read_str_h_

//:
// \file
// \brief  Asks question and waits for an answer.
//         If the answer is a non-empty strint, returns it.
//         If the answer is an empty vcl_string (return)
//         then returns default.
// \author Tim
// Converted to vxl by gvw, soblamemewhydontcha!

/*===========================================================================

  Copyright: (C) 1994 Victoria University of Manchester

  Function Name:  mbl_read_str
  Synopsis: char* mbl_read_str(char *reply, int max_str_len, 
          const char *q_str, const char *default_str)
  Inputs:   reply: Space for reply
      max_str_len: length of space for reply
      q_str: A question 
      default_str: Default answer
  Outputs:  reply: The answer or a default
  Returns:   reply.
  Description:  Asks question and waits for an answer.
      If the answer is a non-empty strint, returns it.
      If the answer is an empty vcl_string (return)
      then returns default.
  References: -
  Example: 
a)

const int N_max_len = 20;
char* Name[N_max_len];
mbl_read_str(Name,N_max_len,"Enter Name","Fred");

b) If default_str = current reply, then reply
  is only overwritten if a different
  vcl_string given.

const int N_max_len = 20;
char* Name[N_max_len];
strcpy(Name,"Freddy");
mbl_read_str(Name,N_max_len,"Enter Name",Name);

  1.1   Tim   New version 4-Nov-98

=================================================================*/

char* mbl_read_str(char *reply, int max_str_len, 
    const char *q_str, const char *default_str);

#endif // mbl_read_str_h_
