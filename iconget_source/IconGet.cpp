//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("IconGet.res");
USEFORM("IconGet_c.cpp", Form1);
USEFORM("..\About\About.cpp", OKBottomDlg);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(TOKBottomDlg), &OKBottomDlg);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
