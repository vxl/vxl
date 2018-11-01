//:
// \file
// \author Tim Cootes
// \brief Get yes or no response from keyboard

#include <iostream>
#include <cstdio>
#include "mbl_read_yes.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool mbl_read_yes()
{
   std::cout<<" (N) ";
   std::cout.flush();  // This forces display of any previous text.
      // and allows something like
      //   std::cout<<"Continue?";
      //   if (RD_GetYes()) ...
      // to work properly. Without this, the
      // "Continue?" std::string isn't displayed
      // until return pressed.
   int c;
   bool r = false;
   while ((c=std::getchar())!='\n') {   /* Loop till return pressed */
      if (c=='y' || c=='Y')
         r = true;  /* If a Y is entered, return true */
   }
   return r;
}

bool mbl_read_no()
{
   std::cout<<" (Y) ";
   std::cout.flush();  // This forces display of any previous text.
   int c;
   bool r = false;
   while ((c=std::getchar())!='\n') {   /* Loop till return pressed */
      if (c=='n' || c=='N')
         r = true;  /* If a N is entered, return true */
   }
   return r;
}
