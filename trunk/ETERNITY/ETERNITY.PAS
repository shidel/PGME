{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETERNITY.DEF}
program Program_Manager_Eternity_Main_Application; { X Series }

uses QCrt, QErrors, QStrings, QDos, QFiles, QObjects, QCOntrols, PGM_Call;

{$I LICENSE.INC}

type 
	TPGME = object (TApplication)
	public { protected }
		procedure BeforeShow; virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
	
	end;   
	
{ --- Global Variables --- }
var
	PGME : TPGME;

{ --- TPGME --- }
constructor TPGME.Create;
var
	P, T : PControl;
	B : TBounds;
begin
	inherited Create(nil, 'PGM');
	FInfo.SetFileName('PGM.EXE');
	FInfo.SetTitle('Program Manager Eternity');
	
	{ Left Pane }
	TextAttr := $06;
	Bounds(1,1,Application^.Width - 26,5, B);
	P := New(PWindow, Create(@Self, 'PROGRAMS'));
	with PWindow(P)^ do begin
		SetAnchors(alTop or alLeft or alBottom or abRight);
		SetBounds(B);
		SetMainWindow(True);
		SetShadow(False);
	end;
	
		{ Top Line }
		Bounds(0, 2, P^.Width, 1, B);
		T := New(PLine, Create(P, 'LINE.TOP'));
		PLine(T)^.SetStyle(lnDouble or lnHorizontal);
		with PLine(T)^ do begin
			SetAllowedOutside(True);
			SetAnchors(abTop or abLeft or abRight); 
			SetBounds(B);
		end;

		{ Bottom Line }
		Bounds(0,P^.Height - 3, P^.Width, 1, B);
		T := New(PLine, Create(P, 'LINE.BOTTOM'));
		PLine(T)^.SetStyle(lnDouble or lnHorizontal);
		with PLine(T)^ do begin
			SetAllowedOutside(True);
			SetAnchors(abLeft or abBottom or abRight);
			SetBounds(B);
		end;
			
		{  Quit Button }
		TextAttr := $1E;
		Bounds(1, 1, 6, 1, B);
		T := New(PButton, Create(P, 'BUTTON.QUIT'));
		with pButton(T)^ do begin
			SetCaption('Quit');
			SetCommand(cmQuit);
			SetAnchors(alBottom or alRight);
			SetBounds(B);	
		end;

	{ Title Pane }
	TextAttr := $06;
	Bounds(1,1,26,3,B);
	P := New(PBox, Create(@Self, 'TITLE'));
	with PWindow(P)^ do begin
		SetAnchors(alTop or alRight);
		SetBounds(B);
		SetShadow(False);
	end;

	{ Title Display }
	TextAttr := $71;
	T := New(PLabel, Create(P, 'SAM_I_AM'));
	with PLabel(T)^ do begin
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
		Bounds(0, 3, P^.Width, 1, B);
		T := New(PLine, Create(P, 'LINE.TOP'));
		PLine(T)^.SetStyle(lnDouble or lnHorizontal);
		with PLine(T)^ do begin
			SetAllowedOutside(True);
			SetAnchors(abLeft or abRight or abTop);
			SetBounds(B);
		end;

		{ Bottom Line }
		Bounds(0,P^.Height - 3,P^.Width, 1, B);
		T := New(PLine, Create(P, 'LINE.BOTTOM'));
		PLine(T)^.SetStyle(lnDouble or lnHorizontal);
		with PLine(T)^ do begin
			SetAllowedOutside(True);
			SetAnchors(abLeft or abRight or abBottom);
			SetBounds(B);
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

begin
	PGME.Create;
	PGME.Execute;
	PGME.Destroy;  
	GotoXY(1, WhereY);
end.
