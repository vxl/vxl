//:
// \file

#include "gevd_param_mixin.h"

//------------------------------------------------------------
//: Check if parameters are valid.  The default does nothing.
bool  gevd_param_mixin::SanityCheck() { return true; }

//------------------------------------------------------------
//: Set the error message for this parameter block.
//  If the message is not empty, this sets the valid flag to be false.
//  An empty message resets the valid flag to be true.
void gevd_param_mixin::SetErrorMsg(const char* msg)
{
  if (msg && msg[0])
  {
    error_msg_ = msg;
    valid_ = false;
  }
  else
  {
    error_msg_ = "";
    valid_ = true;
  }
}
