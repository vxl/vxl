/********************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
** This document contains proprietary source code of
** Earth Resource Mapping Ltd, and can only be used under
** one of the three licenses as described in the 
** license.txt file supplied with this distribution. 
** See separate license.txt file for license details 
** and conditions.
**
** This software is covered by US patent #6,442,298,
** #6,102,897 and #6,633,688.  Rights to use these patents 
** is included in the license agreements.
**
** FILE:   	NCScnet3\NCSGetPasswordDlg
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

#include "NCSTypes.h"

#include <string>

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifdef WIN32
#include <windows.h>
#include "resource.h"
#endif // WIN32

#if defined(MACOSX)
#include "Carbon/Carbon.h"
#include "NCSThread.h"
#endif

using namespace std;

//globals for storing username and password
#ifdef WIN32
char szUsername[255]="";
char szPassword[255]="";
char szSite[255]="";
char szRealm[255]="";

BOOL CALLBACK PasswordDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	static HWND hCtrlBlock;
	static int iColor, iFigure;

	switch( iMsg ) {
	case WM_INITDIALOG:
		SetDlgItemTextA( hDlg, IDC_STATIC_SITE, szSite );
		SetDlgItemTextA( hDlg, IDC_STATIC_REALM, szRealm );
		
		return TRUE;
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) ) {
		case IDOK:

            if(GetDlgItemTextA(hDlg, IDC_EDIT_USERNAME, szUsername, 255)) { // get username
				if(!GetDlgItemTextA(hDlg, IDC_EDIT_PASSWORD, szPassword, 255)) { // get password
					szPassword[0] = 0;
				}
				EndDialog(hDlg, 1);
				return TRUE;
			}
			// else fallthrough
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
			break;
		}
		break;

	case WM_PAINT:
		break;
	}
	return FALSE;
}

#ifdef _DEBUG
#define DLLNAME "NCScnetd.dll"
#else
#define DLLNAME "NCScnet.dll"
#endif

#elif defined(MACOSX)

typedef struct {
    WindowRef theWindow;
    bool bOkSelected;
    string sUsername;
    string sPassword;
    bool bRemember;
} WindowEventData;

static pascal OSStatus MyWindowEventHandler (
                            EventHandlerCallRef myHandler,  
                            EventRef theEvent,  
                            void* userData) 
{

    #pragma unused (myHandler)

    OSStatus result = eventNotHandledErr;
    WindowEventData *pEventData = (WindowEventData *)userData;
    WindowRef theWindow = pEventData->theWindow;
    UInt32 whatHappened;

    whatHappened = GetEventKind (theEvent); 
    switch (whatHappened)
    {
        case kEventWindowShown: 
            
            result = noErr; 
            break;
            
        case kEventWindowHidden: 
            
            result = noErr;
            break;
            
        case kEventControlHit:
            // grab the controlRef out of the event
            ControlRef controlHitRef;
            result = GetEventParameter( theEvent, kEventParamDirectObject, typeControlRef,
                    NULL, sizeof( ControlRef ), NULL, (void *) &controlHitRef );
            require_noerr( result, HandleControlEvent_err );
            
            // grab the id out of the event
            ControlID controlHitID;
            result = GetControlID( controlHitRef, &controlHitID );
            require_noerr( result, HandleControlEvent_err );
    
            switch( controlHitID.id ) {
                case 1: { //OK
                    ControlID controlIDText;
                    ControlRef controlRefText;
                    char  szUsername[255];
                    char  szPassword[255];
                    Size  actualSize=0;
                    
                    pEventData->bOkSelected = true;
                    
                    //Get username
                    controlIDText.id = 3;
                    controlIDText.signature = 'txtc';
                    result = GetControlByID( theWindow, &controlIDText, &controlRefText);
                    require_noerr( result, HandleControlEvent_err );
                    result = GetControlData( controlRefText, 0, kControlEditTextTextTag, 255, szUsername, &actualSize );
                    require_noerr( result, HandleControlEvent_err );
                    szUsername[actualSize] = 0;
                    pEventData->sUsername = szUsername;
                    
                    //Get password
                    controlIDText.id = 4;
                    result = GetControlByID( theWindow, &controlIDText, &controlRefText);
                    require_noerr( result, HandleControlEvent_err );
                    result = GetControlData( controlRefText, 0, kControlEditTextPasswordTag, 255, szPassword, &actualSize );
                    require_noerr( result, HandleControlEvent_err );
                    szPassword[actualSize] = 0;
                    pEventData->sPassword = szPassword;
                    
                    //Get remember status
                    controlIDText.id = 7;
                    controlIDText.signature = 'chkb';
                    result = GetControlByID( theWindow, &controlIDText, &controlRefText);
                    require_noerr( result, HandleControlEvent_err );
                    SInt32 value = GetControl32BitValue( controlRefText );
                    pEventData->bRemember = value;
                    
                    QuitAppModalLoopForWindow( theWindow );
                    break;
                }
                case 2: //Cancel
                    pEventData->bOkSelected = false;
                    QuitAppModalLoopForWindow( theWindow );
                    break;
            
                default:
                    break;
            }
            
            result = noErr;
            
HandleControlEvent_err:
            break;
    }
    
    return (result);
}

#endif // WIN32

bool NCSGetPasswordDlg( const char *pszSite, const char *pszRealm, string &sUsername, string &sPassword, bool &bRememberAuth ) 
{
#ifdef WIN32	
	HINSTANCE	hInstance=0;
	HWND		hwnd=0;
	INT			nResult=0;

	strcpy(szSite, pszSite);
	strcpy(szRealm, pszRealm);

	hwnd = GetActiveWindow();
	hInstance = GetModuleHandleA(DLLNAME);

	nResult = DialogBox(hInstance, (LPCTSTR)IDD_GETPASSWORD, hwnd, PasswordDlgProc);

	if( nResult == 1 ) {
		sUsername = szUsername;
		sPassword = szPassword;
		return true;
	}
	else {
		sUsername = "";
		sPassword = "";
		return false;
	}
#elif defined(MACOSX)
    OSStatus err;
    IBNibRef theNib;
    WindowRef theWindow;
    CFBundleRef mainBundle;

    mainBundle = CFBundleGetBundleWithIdentifier( CFSTR("com.erm.NCSNetscapePlugin6") );
    if( mainBundle ) {    
        err = CreateNibReferenceWithCFBundle (mainBundle, CFSTR("MacOSXPasswordDialog"), &theNib);   
        if (!err) {
            EventHandlerUPP myHandlerUPP;
            ControlID controlIDText;
            ControlRef controlRefText;
            OSStatus status = noErr;
            WindowEventData eventData;
            SInt32 value = (bRememberAuth?1:0);
            EventTypeSpec eventList[] = {            
                                {kEventClassWindow, kEventWindowShown},
                                {kEventClassWindow, kEventWindowHidden},
                                {kEventClassControl, kEventControlHit}};
                                 
            CreateWindowFromNib(theNib, CFSTR("AuthenticationDLG"), &theWindow);

            //Set the site name
            controlIDText.id = 5;
            controlIDText.signature = 'txts';
            status = GetControlByID( theWindow, &controlIDText, &controlRefText);
            require_noerr( status, HandleControlEventA_err );
            status = SetControlData( controlRefText, 0, kControlStaticTextTextTag, strlen(pszSite), pszSite );
            require_noerr( status, HandleControlEventA_err );
                        
            //Set the realm name
            controlIDText.id = 6;
            status = GetControlByID( theWindow, &controlIDText, &controlRefText);
            require_noerr( status, HandleControlEventA_err );
            status = SetControlData( controlRefText, 0, kControlStaticTextTextTag, strlen(pszRealm), pszRealm );
            require_noerr( status, HandleControlEventA_err );
 
            //Set username
            controlIDText.id = 3;
            controlIDText.signature = 'txtc';
            status = GetControlByID( theWindow, &controlIDText, &controlRefText);
            require_noerr( status, HandleControlEventA_err );
            status = SetControlData( controlRefText, 0, kControlEditTextTextTag, strlen(sUsername.c_str()), sUsername.c_str() );
            require_noerr( status, HandleControlEventA_err );
            
            //Set password
            controlIDText.id = 4;
            status = GetControlByID( theWindow, &controlIDText, &controlRefText);
            require_noerr( status, HandleControlEventA_err );
            status = SetControlData( controlRefText, 0, kControlEditTextPasswordTag, strlen(sPassword.c_str()), sPassword.c_str() );
            require_noerr( status, HandleControlEventA_err );
        
            //Set remember status
            controlIDText.id = 7;
            controlIDText.signature = 'chkb';
            status = GetControlByID( theWindow, &controlIDText, &controlRefText);
            require_noerr( status, HandleControlEventA_err );
            SetControl32BitValue( controlRefText, value );

            eventData.theWindow = theWindow;
            eventData.bOkSelected = false;
            eventData.bRemember = bRememberAuth;
            eventData.sUsername = sUsername;
            eventData.sPassword = sPassword;
                        
            myHandlerUPP = NewEventHandlerUPP(MyWindowEventHandler);
            
            status = InstallWindowEventHandler(theWindow,
                                    myHandlerUPP,            
                                    GetEventTypeCount(eventList),            
                                    eventList, &eventData, NULL);
            
            ShowWindow(theWindow);
            
            status = RunAppModalLoopForWindow(theWindow);
            
            HideWindow(theWindow);
            
HandleControlEventA_err:
            
            // Get the status
            if( eventData.bOkSelected ) {
                // If ok selected get the username and password
                sUsername = eventData.sUsername;
                sPassword = eventData.sPassword;
                bRememberAuth = eventData.bRemember;
                //printf("%s, %s\n", sUsername.c_str(), sPassword.c_str());
                return true;
            } else {
                return false;
            }
        }
    }
    
#elif defined( MACINTOSH )
    short           itemHit;
    short           itemType;
    Handle          item;
    Rect            box;
    DialogPtr       dlg = GetNewDialog( 2000, (void*)nil, (WindowPtr)-1 );
    char            str255[256];
    bool			bResult = false;

	//InitCursor ();
	
    if(dlg)
    {
        do {
            ModalDialog( NULL, &itemHit );
            switch(itemHit)
            {
            	case 2: //Cancel Pressed
            		bResult = false;
					sUsername = "";
					sPassword = "";
            		break;
            	case 1: //OK pressed

            		//Get Username 5
	                GetDialogItem(dlg, 5, &itemType, &item, &box);
	                if(item)
	                {
	                    GetDialogItemText( item, (unsigned char *)str255 );
	                    str255[str255[0]+1] = 0;
	                    sUsername = &str255[1];
	                }
	                
	                //Get Password 6
 	                GetDialogItem(dlg, 6, &itemType, &item, &box);
	                if(item)
	                {
	                    GetDialogItemText( item, (unsigned char *)str255 );
	                    str255[str255[0]+1] = 0;
	                    sPassword = &str255[1];
	                }
           	
	            	bResult = true;
	            	break;
            	case 6: //Type password
	            default:
	            	NCSThreadYield();
	            	break;
            }
                
        } while( itemHit != 1 && itemHit !=2 );

        DisposeDialog( dlg );
    }

	return bResult;
#else
	// FIXME!
	sUsername = "";
	sPassword = "";
	return false;
#endif // WIN32
}
