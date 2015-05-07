{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETERNITY.DEF}
program Program_Manager_Eternity_Main_Application; { X Series }

uses QCrt, QErrors, QStrings, QDos, QFiles, QObjects, QCOntrols, PGM_Call;

{$I LICENSE.INC}

const
	cmExecute 	= cmExecuteFile;

type 
	TPGME = object (TApplication)
	public { protected }
			FReadOnly : boolean;
			FEditing : boolean;
			FEditTitle : PEdit;
			FBtnQuit,
			FBtnRun,
			FBtnSearch,
			FBtnHelp,
			FBtnEdit : PButton;
			FProgramWindow : PWindow;
			FPrograms,
			FMenus : PListView;
			FFind : record
				FLabel : PLabel;
				FEdit : PEdit;
			end;
		procedure BeforeShow; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
		procedure ApplyConfig( AConfig : PConfigFile ); virtual;
	end;   
	
{ --- Global Variables --- }
var
	PGME : TPGME;

{ --- TPGME --- }
constructor TPGME.Create;
const 
	Split = 27;
	
var
	P : PControl;
	B : TBounds;
begin
	FReadOnly := False;
	FCanQuit := True;
	FEditing := False;
	
	inherited Create(nil, 'PGM');
	FInfo.SetFileName('PGM.EXE');
	FInfo.SetTitle('Program Manager Eternity');
	
	{ Left Pane }
	TextAttr := $06;
	Bounds(1,1,Application^.Width - Split,5, B);
	FProgramWindow := New(PWindow, Create(@Self, 'PROGRAMS'));
	with PWindow(FProgramWindow)^ do begin
		SetAnchors(alTop or alLeft or alBottom or abRight);
		SetBounds(B);
		SetMainWindow(True);
		SetShadow(False);
		SetTitle(FInfo.Title);
	end;
	
		{ Top Line }
		Bounds(0, 2, FProgramWindow^.Width, 1, B);
		with PLine(New(PLine, Create(FProgramWindow, 'LINE.TOP')))^ do begin
			SetStyle(lnDouble or lnHorizontal);
			SetAllowedOutside(True);
			SetAnchors(abTop or abLeft or abRight); 
			SetBounds(B);
		end;

		{ Bottom Line }
		Bounds(0,FProgramWindow^.Height - 3, FProgramWindow^.Width, 1, B);
		with PLine(New(PLine, Create(FProgramWindow, 'LINE.BOTTOM')))^ do begin
			SetStyle(lnDouble or lnHorizontal);
			SetAllowedOutside(True);
			SetAnchors(abLeft or abBottom or abRight);
			SetBounds(B);
		end;
			
		{  Quit Button }
		TextAttr := $1E;
		Bounds(1, 1, 8, 1, B);
		FBtnQuit := New(PButton, Create(FProgramWindow, 'BUTTON.QUIT'));
		with FBtnQuit^ do begin
			SetCaption('Quit');
			SetCommand(cmQuit);
			SetAnchors(alBottom or alRight);
			SetBounds(B);	
		end;
		
		{  Execute Button }
		Bounds(1, 1, 11, 1, B);
		FBtnRun := New(PButton, Create(FProgramWindow, 'BUTTON.RUN'));
		with FBtnRun^ do begin
			SetCaption('Execute');
			SetCommand(cmExecute);
			SetAnchors(alBottom or alLeft);
			SetBounds(B);	
		end;
		
		{  Help Button }
		TextAttr := $1A;
		Bounds(B.Left + B.Width + 2, 1, 8, 1, B);
		FBtnHelp := New(PButton, Create(FProgramWindow, 'BUTTON.HELP'));
		with FBtnHelp^ do begin
			SetCaption('Help');
			SetCommand(cmHelp);
			SetAnchors(alBottom);
			SetBounds(B);	
		end;
		
		TextAttr := $1B;
		{  Search Button }
		Bounds(B.Left + B.Width + 1, 1, 10, 1, B);
		FBtnSearch := New(PButton, Create(FProgramWindow, 'BUTTON.SEARCH'));
		with FBtnSearch^ do begin
			SetCaption('Search');
			SetCommand(cmSearch);
			SetAnchors(alBottom);
			SetBounds(B);	
{			SetVisible(False);}
		end;
		
		TextAttr := $1C;
		{  Edit Button }
		Bounds(B.Left + B.Width + 1, 1, 8, 1, B);
		FBtnEdit := New(PButton, Create(FProgramWindow, 'BUTTON.EDIT'));
		with FBtnEdit^ do begin
			SetCaption('Edit');
			SetCommand(cmEditFile);
			SetAnchors(alBottom);
			SetBounds(B);	
		end;
		
		TextAttr := $02;
		Bounds(1, 1, 13, 1, B);
		with PLabel(New(PLabel, Create(FProgramWindow, 'TITLE.LABEL')))^ do begin
			SetBounds(B);
			SetCaption('Menu Title:');
		end;

		TextAttr := $0B;
		Bounds(15, 1, Application^.Width - Split - 16, 1, B);
		FEditTitle := New(PEdit, Create(FProgramWindow, 'TITLE.EDIT'));
		with FEditTitle^  do begin
			FHAlign := AlignLeft;
			SetBounds(B);
			SetCaption('no name');
			SetEnabled(False);
		end;

		TextAttr := $0B;		
		Bounds(2, 4, FProgramWindow^.Width - 4, FProgramWindow^.Height - 8, B);
		FPrograms := New(PListView, Create(FProgramWindow, 'PROGRAMS'));
		with FPrograms^ do begin
			SetBounds(B);
			SetAnchors(abAll);
			SetScrollEdge(True);
		end;
		
	{ Title Pane }
	TextAttr := $06;
	Bounds(1,1, Split,3,B);
	P := New(PBox, Create(@Self, 'TITLE'));
	with PWindow(P)^ do begin
		SetAnchors(alTop or alRight);
		SetBounds(B);
		SetShadow(False);
	end;

		{ Title Display }
		TextAttr := $71;
		with PLabel(New(PLabel, Create(P, 'SAM_I_AM')))^ do begin
			SetCaption('PGM Eternity');
			SetAnchors(alClient);
		end;
	
	{ Right Pane }
	TextAttr := $06;
 	Bounds(1, P^.Height + 1, P^.Width, Application^.Height - P^.Height, B);
	P := New(PBox, Create(@Self, 'MENUS'));
	with PWindow(P)^ do begin
		SetAnchors(alBottom or alRight or abTop);
		SetBounds(B);
		SetShadow(False);
	end;
		
		{ Top Line }
		Bounds(0, 3,P^.Width, 1, B);
		with PLine(New(PLine, Create(P, 'LINE.TOP')))^ do begin
			SetStyle(lnDouble or lnHorizontal);
			SetAllowedOutside(True);
			SetAnchors(abLeft or abRight or abTop);
			SetBounds(B);
		end;

		{ Bottom Line }
		Bounds(0,P^.Height - 3,P^.Width, 1, B);
		with PLine(New(PLine, Create(P, 'LINE.BOTTOM')))^ do begin
			SetStyle(lnDouble or lnHorizontal);
			SetAllowedOutside(True);
			SetAnchors(abLeft or abRight or abBottom);
			SetBounds(B);
		end;

		TextAttr := $70;
		Bounds(1, 4, P^.Width - 2, 1, B);
		with PLabel(New(PLabel, Create(P, 'LABEL')))^ do begin
			SetBounds(B);
			SetCaption('Available Menus:');
		end;

		TextAttr := $04;		
		Bounds(2, 6, P^.Width - 4, P^.Height - 10, B);
		FMenus := New(PListView, Create(P, 'MENUS'));
		with FMenus^ do begin
			SetBounds(B);
			SetAnchors(abAll);
			SetScrollEdge(True);
		end;
		
		TextAttr := $08;		
		Bounds(1, 1, 6, 1, B);
		FFind.FLabel := New(PLabel, Create(P, 'SEARCH.LABEL'));
		with FFind.FLabel^ do begin
			SetBounds(B);
			SetAnchors(abLeft or alBottom);
			SetCaption(' Find>');
{			SetVisible(False) }
		end;
		TextAttr := $0F;		
		Bounds(7, 1, P^.Width - B.Width - B.Left - 1, 1, B);
		FFind.FEdit := New(PEdit, Create(P, 'SEARCH.EDIT'));
		with FFind.FEdit^ do begin
			SetBounds(B);
			SetAnchors(alRight or alBottom);
{			SetVisible(False)}
		end;
		
	FClrScr := True;
end;

destructor TPGME.Destroy;
begin
	inherited Destroy;
end;

procedure TPGME.BeforeShow;
begin
	inherited BeforeShow;
	if ParamStr(2) = '/INIT' then begin
		GotoXY(1, WhereY);
		TextAttr := $07;
		ClrEOL;
	end;
end;

procedure TPGME.ApplyConfig( AConfig : PConfigFile );
begin
	inherited ApplyConfig(AConfig);
	FReadOnly := StrBool(AConfig^.GetValue('READONLY', BoolStr(FReadOnly)));
	FCanQuit := StrBool(AConfig^.GetValue('CANQUIT', BoolStr(FCanQuit)));
	
	FBtnEdit^.SetVisible(Not FReadOnly);
	FBtnQuit^.SetVisible(FCanQuit);
	FProgramWindow^.SetCanClose(FCanQuit);
end;

procedure TPGME.CheckLocal(var AEvent : TEvent); 
var
	T : integer;
	B : TBounds;
begin
{	if AEvent.What = evCommand then
		case AEvent.Command of
		end; }
	inherited CheckLocal(AEvent);
end;

begin
	PGME.Create;
	PGME.Execute;
	PGME.Destroy;  
	GotoXY(1, WhereY);
end.
