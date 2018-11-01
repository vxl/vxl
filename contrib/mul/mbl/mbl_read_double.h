// This is mul/mbl/mbl_read_double.h
#ifndef mbl_read_double_h_
#define mbl_read_double_h_
//:
// \file
// \brief Asks question and waits for an answer
// \author tim
// hand crafted into vxl by gvw
//
// - Function Name:  mbl_read_double
// - Synopsis: double mbl_read_double(char* q_str, double default_d)
// - Inputs:   q_str: A question
//      default_d: Default answer
//        min_d: Min allowed value (optional)
//        max_d: Max allowed reply (optional)
// - Outputs:  -
// - Returns:    The answer or a default
// - Description:  Asks question and waits for an answer.
//      If the answer is a double, returns it.
//      If the answer is an empty std::string (return)
//      then returns default.
//      Otherwise waits for another input.
// - References: -
// - Example:
// \code
//    new_scale = mbl_read_double("Scale?",1.00);
//    double new_scale = mbl_read_double("Scale?",1.00,min_scale,max_scale);
// \endcode

double mbl_read_double(const char* q_str, double default_d);
double mbl_read_double(const char* q_str, double default_d, double min_d, double max_d);

#endif //mbl_read_double_h_
