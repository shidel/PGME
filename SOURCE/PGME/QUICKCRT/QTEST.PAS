{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I QCRT.DEF}
{$M 50000,0,655350} (* Memory: Largest Stack, No Minimum Free, Use All Ram *)
program Program_Manager_Eternity_Main_Application; { X Series }


uses QCrt, QErrors, QStrings, QDos, QFiles, QLog, QObjects, 
	QControls, QMore, QFileDlg, QFmtStr, QTimer, QBios, QSpeaker;


type 
	TTest = object (TApplication)
			Prog : PProgressBar;
		constructor Create;
		destructor Destroy; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
	end;   
	
{ --- Global Variables --- }
var
	Test : TTest;
	PChars : PDialog;
	PPath, POpen, PSave : PDialog;
	
{ --- Events --- }

{ --- TPGME --- }
procedure TTest.CheckLocal(var AEvent : TEvent); 
var
	T : integer;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmTab : begin 
				Test.Prog^.SetProgress(Test.Prog^.GetProgress + 1);
				ClearEvent(AEvent);
			end;
			cmUnTab : begin 
				Test.Prog^.SetProgress(Test.Prog^.GetProgress - 1);
				ClearEvent(AEvent);
			end;
			cmScrollBar : begin
			{	if AEvent.InfoPtr = HS then 
					PutCOmmand(cmCloseWindow, nil); }
			end;
			cmCut : begin
				ClearEvent(AEvent);
				T := PPath^.ShowModal;
			end;
			cmCopy : begin
				ClearEvent(AEvent);
				T := POpen^.ShowModal;
			end;
			cmPaste : begin
				ClearEvent(AEvent);
				T := PSave^.ShowModal;
			end;
			cmCharMap : begin
				T := PChars^.ShowModal;
				if T > mrNoResult then begin
					AEvent.what := evKeyboard;
					AEvent.ShiftCode := $FFFF;
					AEvent.Original := T;
					AEvent.KeyCode := T;
					PutEvent(AEvent);
				end;
				ClearEvent(AEvent);
			end;
		end;
	inherited CheckLocal(AEvent);
end;

constructor TTest.Create;
var
	P, T : PControl;
	B : TBounds;
	L : PListItem;
	I : integer;
begin
	inherited Create(nil, 'QTEST');
	AddHelpText('0,1;Help for %0 (%1) is here.');
	PChars := New(PCharacterMap, Create(@Self, 'Characters')); 

	PPath := New(PPathDialog, Create(@Self, 'Path')); 
	TextAttr := $17;
	
	POpen := New(POpenDialog, Create(@Self, 'Open')); 
	TextAttr := $17;
	with POpenDialog(POpen)^ do begin		
		AddFilter('Font Files (*.FNT)');
		AddFilter('Executables (*.EXE;*.COM;*.BAT)');
	end;
	
	PSave := New(PSaveDialog, Create(@Self, 'Save')); 
	TextAttr := $17;
	with PSaveDialog(PSave)^ do begin		
		AddFilter('Font Files (*.FNT)');
		AddFilter('Executables (*.EXE;*.COM;*.BAT)');
	end;
	
	P := New(PWindow, Create(@Self, 'test'));
	P^.SetAnchors(alNone);
	Bounds(3,2,30, 10, B);
	P^.SetBounds(B);
	TextAttr := $1E;
	PWindow(P)^.SetTitle('Test');
	
	TextAttr := $1E;
	T := New(PListView, Create(P, 'LIST'));
	with PListView(T)^ do begin
		SetAnchors(alClient);
		SetColumns(3);
		FColumns^[0].Width := 5;
		FColumns^[0].Name := StrPtr('FILE');
		FColumns^[1].Width := 5;
		FColumns^[1].Name := StrPtr('SIZE');
		FColumns^[2].Width := 25;
		FColumns^[2].Name := StrPtr('INFO_ABOUT_THIS_THING');
		for I := 1 to 4 do begin
			L := New(PListItem, Create('Line'));
			FItems.Add(L);
			L^.Add(New(PListItem, Create(IntStr(I))));
			L^.Add(New(PListItem, Create(HexStr(I))));
		end;
	end;

	TextAttr := $1E;
	P := New(PKeyStroke, Create(@Self, 'KEYSTROKE'));
	P^.SetAnchors(alTop or alRight);
	P^.SetVisible(True);

	TextAttr := $0B;
	P := New(PWindow, Create(@Self, 'Window'));
	P^.SetAnchors(0);
	Bounds(5,12,60,10, B);
	P^.SetBounds(B);
	TextAttr := $1E;
	PWindow(P)^.SetTitle('Hello');
	PWindow(P)^.SetMainWindow(True); 
	
	TextAttr := $1E;
	T := New(PDropList, Create(P, 'DLIST'));
	with PDropList(T)^ do begin
		Bounds(2,5,20, 9, B);
		SetBounds(B);
		SetSorted(True);
		AddItem(StrHex('One', False));
		for I := 0 to 15 do 
			AddItem('Item ' + IntStr(I));
		AddItem(StrHex('Hello', False));
		AddItem(StrHex('Two', False));
	end;

	TextAttr := $1E;
	T := New(PButton, Create(P, 'Button'));
	PButton(T)^.SetCaption('Really Big Button');
	T^.SetCommand(cmCharMap); 
	Bounds(5,1,22,3, B);
	T^.SetBounds(B);
	T^.SetAnchors(abRight); 
		
	T := New(PEdit, Create(P, 'Edit Box'));
	PEdit(T)^.SetCaption('Edit Text');
	PEdit(T)^.SetRespond(cmEnter);
	T^.AddHelpText('0,1,2,3;Help for %0(%1) as %2 %3.');
	Bounds(1,1,15,1, B);
	T^.SetBounds(B);
	T^.SetAnchors(alTop or alRight); 

	TextAttr := $1B;
	T := New(PProgressBar, Create(P, 'Progress'));
	T^.SetAnchors(alBottom or alRight or alLeft); 
	Bounds(5,5,22,1, B);
	T^.SetBounds(B);
	Prog := PProgressBar( T );
	
	TextAttr := $7F;
	T := New(PToggle, Create(P, 'Toggle'));
	T^.SetAnchors(alRight); 
	Bounds(5,3,10,1, B);
	T^.SetBounds(B);

	TextAttr := $2F;
	T := New(PCheckbox, Create(P, 'Check'));
	PCheckbox(T)^.SetCaption('Hello');
	T^.SetAnchors(alRight); 
	Bounds(5,5,10,1, B);
	T^.SetBounds(B); 
	
end;

destructor TTest.Destroy;
begin
	inherited Destroy;
end;

procedure DoTest;
begin
	Test.Create; 
	TextAttr := $07;
	Test.Execute; 
	Test.Destroy; 
	WriteLn;   
end;

var
	Event : TEvent;

begin
	FWrite(HexToStr(StrHex('Test Program Starting', False))); 
	ErrorsOnExit := False;
		
	DoTest;  
end.
