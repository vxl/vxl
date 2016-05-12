#ifndef mbl_read_str_h_
#define mbl_read_str_h_
//:
// \file
// \brief  Asks question and waits for an answer.
// \author Tim
// \date Based on code written 7-May-92
//
// \verbatim
//  Modifications
//   Converted to vxl by gvw, soblamemewhydontcha!
//   1.1   Tim   New version 4-Nov-98
// \endverbatim

//: Asks question and waits for an answer from standard input.
//       If the answer is a non-empty string, returns it.
//       If the answer is an empty std::string (return)
//       then returns default.
// \param reply Space for reply
// \param max_str_len Length of space for reply
// \param q_str A question
// \param default_str Default answer
// \return reply: The answer or a default
//
//   Example:
// a)
// \code
// const int N_max_len = 20;
// char* Name[N_max_len];
// mbl_read_str(Name,N_max_len,"Enter Name","Fred");
// \endcode
//
// b) If default_str = current reply, then reply
//   is only overwritten if a different
//   std::string given.
//
// \code
// const int N_max_len = 20;
// char* Name[N_max_len];
// strcpy(Name,"Freddy");
// mbl_read_str(Name,N_max_len,"Enter Name",Name);
// \endcode

char* mbl_read_str(char *reply, int max_str_len,
                   const char *q_str, const char *default_str);

#endif // mbl_read_str_h_
