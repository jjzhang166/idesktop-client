//---------------------------------------------------------------------------
#ifndef IconGet_cH
#define IconGet_cH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *File1;
        TMenuItem *Open1;
        TMenuItem *Save1;
        TMenuItem *Exit1;
        TMenuItem *Help1;
        TMenuItem *About1;
        TOpenDialog *OpenDialog1;
        TStatusBar *StatusBar1;
        TGroupBox *GroupBox1;
        TGroupBox *GroupBox2;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TButton *Button4;
        TImage *Image1;
        TSaveDialog *SaveDialog1;
        TMenuItem *N1;
        void __fastcall Open1Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Save1Click(TObject *Sender);
        void __fastcall Exit1Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall About1Click(TObject *Sender);
private:	// User declarations
        AnsiString      FileName;
        void            GetIcon();
        void            UpdateStatus();
        int             Total,Now;
        int             YESORNOT;
        AnsiString      CurrentPath, TempFile;
        TIcon           *Icon;
public:		// User declarations
        void    ExtractIconFromFile(int NowPosition);
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
