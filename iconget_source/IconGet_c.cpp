//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "IconGet_c.h"
#include "About.h"
#include "shellapi.h"
#include "winbase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ImgeditLibCtl_OCX"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Open1Click(TObject *Sender)
{
        if(OpenDialog1->Execute())
        {
                FileName = OpenDialog1->FileName;
                Button1->Enabled=true;
                Button2->Enabled=true;
                GetIcon();
        }
}
//---------------------------------------------------------------------------
void    TForm1::GetIcon()
{
        HICON hIcon;
        Total =(int) ExtractIcon( Form1->Handle, FileName.c_str(), -1);

        Icon = new TIcon();
        hIcon = ExtractIcon( Form1->Handle, FileName.c_str(), 0);
        Icon->Handle=hIcon;

        Icon->SaveToFile(TempFile);
        Image1->Picture->LoadFromFile(TempFile);

        Now = 0;
        UpdateStatus();
}
void    TForm1::ExtractIconFromFile(int NowPosition)
{
        HICON hIcon;
        Icon = new TIcon();
        hIcon = ExtractIcon( Form1->Handle, FileName.c_str(), NowPosition);
        Icon->Handle=hIcon;

        Icon->SaveToFile(TempFile);
        Image1->Picture->LoadFromFile(TempFile);

        UpdateStatus();
}
//---------------------------------------------------------------------------
void    TForm1::UpdateStatus()
{
        if(Now+1<=1)
        {
                Button4->Enabled=false;
                Button3->Enabled=true;
        }
        else if(Now+1>=Total)
        {
                Button4->Enabled=true;
                Button3->Enabled=false;
        }
        else
        {
                Button3->Enabled=true;
                Button4->Enabled=true;
        }

        if(Total==1)
        {
                Button3->Enabled=false;
                Button4->Enabled=false;
        }

        StatusBar1->Panels->Items[0]->Text= AnsiString("This is the ") + (int)(Now+1) + " icon.";
        StatusBar1->Panels->Items[1]->Text= AnsiString("It has ") + Total + " icon(s).";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
        char Buff[255];
        GetTempPath( 255 , Buff);
        CurrentPath = Buff;
        TempFile = CurrentPath + "\\IconGetTemp.ico";
        Button1->Enabled=false;
        Button2->Enabled=false;
        Button3->Enabled=false;
        Button4->Enabled=false;
        YESORNOT = -1;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button1Click(TObject *Sender)
{
        Now = 0;
        ExtractIconFromFile(Now);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
        Now = Total-1;
        ExtractIconFromFile(Now);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
        Now = Now+1;
        ExtractIconFromFile(Now);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
        Now = Now-1;
        ExtractIconFromFile(Now);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Save1Click(TObject *Sender)
{
        HICON hIcon;
        Icon = new TIcon();
        hIcon = ExtractIcon( Form1->Handle, FileName.c_str(), Now);
        Icon->Handle=hIcon;
        if(SaveDialog1->Execute())
        {
                if(FileExists(SaveDialog1->FileName))
                {
                        YESORNOT = Application->MessageBoxA("File exist, overwrite it or not?", "warnning", MB_YESNO);
                        if(YESORNOT == 6)
                                Icon->SaveToFile(SaveDialog1->FileName);
                }
                else
                        Icon->SaveToFile(SaveDialog1->FileName);
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        DeleteFile( TempFile );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::About1Click(TObject *Sender)
{
        OKBottomDlg->ShowModal();
}
//---------------------------------------------------------------------------


