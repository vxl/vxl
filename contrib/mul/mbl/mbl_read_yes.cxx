#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Tim Cootes
// \brief Get yes or no response from keyboard

#include "mbl_read_yes.h"
#include <vcl_iostream.h>
#include <vcl_cstdio.h>

bool mbl_read_yes()
{
   vcl_cout<<" (N) ";
   vcl_cout.flush();  // This forces display of any previous text.
      // and allows something like
      //   vcl_cout<<"Continue?";
      //   if (RD_GetYes()) ...
      // to work properly. Without this, the
      // "Continue?" vcl_string isn't displayed
      // until return pressed.
   int c;
   bool r = false;
   while ((c=vcl_getchar())!='\n') {   /* Loop till return pressed */
      if (c=='y' || c=='Y')
         r = true;  /* If a Y is entered, return true */
   }
   return r;
}

bool mbl_read_no()
{
   vcl_cout<<" (Y) ";
   vcl_cout.flush();  // This forces display of any previous text.
   int c;
   bool r = false;
   while ((c=vcl_getchar())!='\n') {   /* Loop till return pressed */
      if (c=='n' || c=='N')
         r = true;  /* If a N is entered, return true */
   }
   return r;
}
