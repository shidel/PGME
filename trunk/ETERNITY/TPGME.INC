{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TMemo --- }

{$IFDEF INTERFACE}
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
			FDate, FTime : PLabel;
			FMenuCache : PObject;
		procedure BeforeShow; virtual;
		procedure Configure; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;		
		procedure UpdateButtons; virtual;
		procedure UpdateMenuCache; virtual;
		procedure OpenCurrentMenu; virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
		procedure ApplyConfig( AConfig : PConfigFile ); virtual;
		procedure Idle; virtual;
	end;   

{ Some Global Variables }	
var
	PGME : TPGME;
	OldIdleProc : procedure;

{$ENDIF}

{$IFDEF IMPLEMENTATION}
procedure FormatClock (AVisual : PControl; var AValue : String); far;
var
	FD : array[0..4] of TFormatData;
	Year, Month, Day, DayOfWeek,
	Hour, Min, Sec, Sec100 : word;
begin
	if not Assigned(AVisual) then exit;
	if AVisual = PControl(PGME.FDate) then begin
		GetDate(Year, Month, Day, DayOfWeek);
		FD[0].ID := FmtInteger;
		FD[0].IntegerValue := Year;
		FD[1].ID := FmtInteger;
		FD[1].IntegerValue := Month;
		FD[2].ID := FmtInteger;
		FD[2].IntegerValue := Day;
		FD[3].ID := fmtInteger;
		FD[3].IntegerValue := DayOfWeek;
		AValue := FormatStr(PGME.FDate^.GetCaption, FD, 4);
	end else if AVisual = PControl(PGME.FTime) then begin
		GetTime(Hour, Min, Sec, Sec100);
		FD[0].ID := FmtInteger;
		FD[0].IntegerValue := Hour;
		FD[2].ID := FmtInteger;
		FD[2].IntegerValue := Min;
		FD[3].ID := FmtInteger;
		FD[3].IntegerValue := Sec;
		FD[4].ID := FmtBoolean;
		FD[4].BooleanValue := Hour >= 12;
		if Hour >= 12 then Hour := Hour - 12;
		if Hour = 0 then Hour := 12;
		FD[1].ID := FmtInteger;
		FD[1].IntegerValue := Hour;
		AValue := FormatStr(PGME.FTime^.GetCaption, FD, 5); 
	end;
end;

procedure PGMIdleProc; far;
begin
	OldIdleProc;
	if Assigned(Application) then Application^.Idle;
end;

{ --- TPGME --- }

constructor TPGME.Create;
const 
	Split = 27;
var
	P : PControl;
	B : TBounds;
begin	
	inherited Create(nil, 'PGM');
	FReadOnly := False;
	FCanQuit := True;
	FEditing := False;
	OldIdleProc := QCrt.IdleProc;
	QCrt.IdleProc := PGMIdleProc;
	FMenuCache := New(PMenuCache, Create);
	
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
			SetCaption('no menu');
			SetEnabled(False);
		end;

		TextAttr := $03;		
		Bounds(2, 4, FProgramWindow^.Width - 4, FProgramWindow^.Height - 8, B);
		FPrograms := New(PListView, Create(FProgramWindow, 'PROGRAMS'));
		with FPrograms^ do begin
			SetBounds(B);
			SetAnchors(abAll);
			SetScrollEdge(True);
			SetSorted(True);
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
	
		{ Clock }
		TextAttr := $1F;
		Bounds(1,1,P^.Width - 2, 1, B);
		FTime := New(PLabel, Create(P, 'CLOCK.TIME'));
		with FTime^ do begin
			OnFormat := FormatClock;
			SetBounds(B);
			SetAnchors(alLeft or alRight);
			SetCaption('0[Z2],1[L2],2[Z2],3[Z2],4{am,pm};%1:%2:%3 %4');
		end;
		TextAttr := $1F;
		Inc(B.Top);
		FDate := New(PLabel, Create(P, 'CLOCK.DATE'));
		with FDate^ do begin
			OnFormat := FormatClock;
			SetBounds(B);
			SetAnchors(alLeft or alRight);
			{ Short Day }
			(*  SetCaption('0[X3.2],1[L2],2[Z2];%1/%2/%0'); *)
			{ Medium Day }
			SetCaption('0[Z4],1{Null,Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec},2,' +
			'3{Sun,Mon,Tues,Wed,Thurs,Fri,Sat};%3 %1 %2, %0'); 
			
		end;

		TextAttr := $06;
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
			SetAllowEmpty(False);
			SetSorted(True);
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
	QCrt.IdleProc := OldIdleProc;
	if Assigned(FMenuCache) then 
		Dispose(FMenuCache, Destroy);
	inherited Destroy;
end;

procedure TPGME.BeforeShow;
begin
	inherited BeforeShow;
	{ if ParamStr(2) = '/INIT' then begin }
	GotoXY(1, WhereY);
	TextAttr := $07;
	ClrEOL;
end;

procedure TPGME.Configure;
begin
{	GotoXY(1, WhereY);
	TextAttr := $0B;
	Write('Initializing...');
	ClrEol; }
	inherited Configure;
	PMenuCache(FMenuCache)^.ApplyLanguage(@FLanguage, GetPathID);
	FEditTitle^.SetCaption(PMenuCache(FMenuCache)^.Description); {Set Initially to No Title}
	GotoXY(1, WhereY);
	TextAttr := $0E;
	Write(FLanguage.GetValue(GetPathID + '.BOOT.CACHE.UPDATE', 'Updating Menu Cache...'));
	ClrEol;
	FConfig.OpenSection(GetPathID);
	PMenuCache(FMenuCache)^.Load;
	PMenuCache(FMenuCache)^.SetPaths(FConfig.GetValue('PATH.MENU', ''));
	UpdateMenuCache;
	GotoXY(1, WhereY);
	TextAttr := $07;
	ClrEol;		
	GotoXY(1, WhereY);
	TextAttr := $0B;
	Write(FLanguage.GetValue(GetPathID + '.BOOT.STARTING', 'Beginning Eternity...'));
	ClrEol;
end;

procedure TPGME.ApplyConfig( AConfig : PConfigFile );
begin
	inherited ApplyConfig(AConfig);
	AConfig^.OpenSection(GetPathID);
	FReadOnly := StrBool(AConfig^.GetValue('READONLY', BoolStr(FReadOnly)));
	FCanQuit := StrBool(AConfig^.GetValue('CANQUIT', BoolStr(FCanQuit)));
	FBtnQuit^.SetVisible(FCanQuit);
	FProgramWindow^.SetCanClose(FCanQuit);
	UpdateButtons;
end;

procedure TPGME.Idle;
const
	S : word = 0;
	L : longInt = 0;
var
	Ticks : LongInt absolute $0040:$006c;
	Hour, Min, Sec, Sec100 : word;
begin
	inherited Idle;
	if (Not Assigned(Application)) or (Not Application^.GetVisible) then exit;
	if Ticks = L then Exit;
	if Assigned(FHelp) and FHelp^.GetVisible then Exit;
	
	L := Ticks;
	GetTime(Hour, Min, Sec, Sec100);
	if Sec <> S then begin
		S := Sec;
		FTime^.Update;
		FDate^.Update;
	end;
end;

procedure TPGME.UpdateMenuCache;
begin
	PMenuCache(FMenuCache)^.Update;
	if PMenuCache(FMenuCache)^.Modified and (not FReadOnly) then
		PMenuCache(FMenuCache)^.Save;
end;

procedure TPGME.OpenCurrentMenu;
var
	T, P :PListItem;
	S : String;
begin
	StartDrawing;
	FPrograms^.ClearItems;
	UpdateButtons;
	FinishDrawing;
end;

procedure TPGME.UpdateButtons;
begin
	FBtnEdit^.SetVisible((Not FReadOnly) and PMenuCache(FMenuCache)^.Writable);
end;

procedure TPGME.CheckLocal(var AEvent : TEvent); 
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmNextMenu : begin
				ClearEvent(AEvent);
				FMenus^.Down(True);
				OpenCurrentMenu;
			end;
			cmPrevMenu : begin
				ClearEvent(AEvent);
				FMenus^.Up(True);
				OpenCurrentMenu;
			end;
		end; 
	inherited CheckLocal(AEvent);
end;

{$ENDIF}
