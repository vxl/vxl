
 /*  KBLayout.c  */

 #include "KBLayout.h"

 Boolean IsKBGetLayoutTypeAvailable (void);

 /*  Returns the keyboard layout type.  */

 OSType GetKeyboardLayoutType (short deviceID)
 {
     OSType keyboardLayoutType;

     switch (deviceID) {
         case 0x01:
         case 0x02:
         case 0x03:
         case 0x06:
         case 0x08:
         case 0x0C:
         case 0x10:
         case 0x18:
         case 0x1B:
         case 0x1C:
         case 0xC0:
         case 0xC3:
         case 0xC6:
             keyboardLayoutType = kKeyboardANSI;
             break;
         case 0x12:
         case 0x15:
         case 0x16:
         case 0x17:
         case 0x1A:
         case 0x1E:
         case 0xC2:
         case 0xC5:
         case 0xC8:
         case 0xC9:
             keyboardLayoutType = kKeyboardJIS;
             break;
         case 0x04:
         case 0x05:
         case 0x07:
         case 0x09:
         case 0x0D:
         case 0x11:
         case 0x14:
         case 0x19:
         case 0x1D:
         case 0xC1:
         case 0xC4:
         case 0xC7:
             keyboardLayoutType = kKeyboardISO;
             break;
         default:
             if (IsKBGetLayoutTypeAvailable ())
                 keyboardLayoutType = KBGetLayoutType (deviceID);
             else
                 keyboardLayoutType = kKeyboardUnknown;
             break;
     }
     return keyboardLayoutType;
 }

 /*  Returns true if KBGetLayoutType is available.  */

 Boolean IsKBGetLayoutTypeAvailable (void)
 {
     long response;
return true;
     if (Gestalt (gestaltKeyboardsLib, &response) == noErr)
         return true;
     else
         return false;
 }
