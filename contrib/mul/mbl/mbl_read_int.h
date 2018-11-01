// This is mul/mbl/mbl_read_int.h
#ifndef mbl_read_int_h_
#define mbl_read_int_h_
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

int mbl_read_int(const char* q_str, int default_int);
int mbl_read_int(const char* q_str, int default_int, int min_int, int max_int);

#endif //mbl_read_int_h_
