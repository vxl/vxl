#ifndef mbl_read_yes_h_
#define mbl_read_yes_h_
//:
// \file
// \brief Get yes or no response from keyboard
// \author Tim Cootes
// Converted to VXL by Louise Butcher

//: True if 'y' or 'Y' pressed
//Waits for returns to be pressed.
//Returns true if last character pressed is a Y.
bool mbl_read_yes();

//:True if 'n' or 'N' pressed
//Waits for returns to be pressed.
//Returns true if last character pressed is a N.
bool mbl_read_no();

#endif // mbl_read_yes_h_
