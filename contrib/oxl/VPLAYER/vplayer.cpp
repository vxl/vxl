// vplayer.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include "vplayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
        int nRetCode = 0;

        // initialize MFC and print and error on failure
        if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
        {
                // TODO: change error code to suit your needs
                vcl_cerr << _T("Fatal Error: MFC initialization failed") << vcl_endl;
                nRetCode = 1;
        }
        else
        {
                // TODO: code your application's behavior here.
                CString strHello;
                strHello.LoadString(IDS_HELLO);
                vcl_cout << (LPCTSTR)strHello << vcl_endl;
        }

        return nRetCode;
}


