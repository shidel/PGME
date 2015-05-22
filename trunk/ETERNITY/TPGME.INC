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
			FEditing, 
			FWritable,
			FBatLoop, FInitRun : boolean;
			FReqMenu, FReqProg : String;
			FEditTitle : PEdit;
			FBtnQuit,
			FBtnRun,
			FBtnSearch,
			FBtnHelp,
			FBtnEdit : PButton;
			FProgramWindow : PWindow;
			FPrograms,
			FMenus : PListView;
			FLastMenu, FLastProgram : PListItem;
			FVersion : PLabel;
			FNoTempDir, FNoBatchLoop : PDialog;
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
		procedure ParseCommandLine; virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
		procedure ApplyConfig( AConfig : PConfigFile ); virtual;
		procedure Idle; virtual;
		procedure ExecuteItem(AItem : PListItem); virtual;
		function GetNoTempDir : PDialog; virtual;
		function GetNoBatchLoop : PDialog; virtual;
		procedure CreateAllDialogs(ConserveMemory : boolean); virtual;
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

procedure FormatVersion (AVisual : PControl; var AValue : String); far;
var
	FD : array[0..4] of TFormatData;
begin
	if not Assigned(AVisual) then exit;
	if AVisual = PControl(PGME.FVersion) then begin
		FD[0].ID := FmtInteger;
		FD[0].IntegerValue := Version.Year;
		FD[1].ID := FmtInteger;
		FD[1].IntegerValue := Version.Month;
		FD[2].ID := FmtInteger;
		FD[2].IntegerValue := Version.Day;
		FD[3].ID := fmtInteger;
		FD[3].IntegerValue := Version.Revision;
		FD[4].ID := fmtString;
		FD[4].StringValue := Version.URL;
		AValue := FormatStr(PGME.FVersion^.GetCaption, FD, 5);
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
	FBatLoop := False;
	FInitRun := False;
	FReqMenu := '';
	FReqProg := '';
	FReadOnly := False;
	FCanQuit := True;
	FEditing := False;
	FWritable := True;
	FLastMenu := nil;
	FLastProgram := nil;
	FNoTempDir := nil;
	FNoBatchLoop := nil;
	OldIdleProc := QCrt.IdleProc;
	QCrt.IdleProc := PGMIdleProc;
	
	ParseCommandLine;
	
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
		FVersion := PLabel(New(PLabel, Create(P, 'SAM_I_AM')));
		with FVersion^ do begin
			OnFormat := FormatVersion;
			SetCaption('0[Z4,X3.2],1,2[Z2];Version %1/%2/%0');
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

function TPGME.GetNoTempDir;
var
	B : TBounds;
begin
	if Not Assigned(FNoTempDir) then begin
		TextAttr := $4F;
		FNoTempDir := New(PMessageBox, Create(@Self, 'DIALOG.NOTEMPDIR'));
		with PMessageBox(FNoTempDir)^ do begin
			SetBackground(4);
			SetTextAttr($4F);
			Bounds(1,1,68,10, B);
			SetBounds(B);
			FAutoEndModal := True;
			SetTitle('Oh, NO! Me so sorry! :(');
			SetMessage(' ');
			AppendMessage('');
			AppendMessage('Unfortunately, the system has no TEMP directory defined.');
			AppendMessage('So, I am unable to execute that program at this time.');
			AppendMessage(' ');
			AppendMessage('Maybe, try adding SET TEMP=C:\TEMP to the AUTOEXEC.BAT file.'); 
			AppendMessage('(That assumes you have a directory called C:\TEMP somewhere)'); 
			SetHelpText('This is an error dialog box.');
			AddHelpText('');
			AddHelpText('It means what it says. You do not have a TEMP directory setup.');
			AddHelpText('You should really think about adding one. Then declare it');
			AddHelpText('in your AUTOEXEC batch file using the SET command.');
		end;
		ApplySettings(FNoTempDir);
	end;
	GetNoTempDir := FNoTempDir;
end;

function TPGME.GetNoBatchLoop;
var
	B : TBounds;
begin
	if Not Assigned(FNoBatchLoop) then begin
		TextAttr := $4F;
		FNoBatchLoop := New(PMessageBox, Create(@Self, 'DIALOG.NOBATCH'));
		with PMessageBox(FNoBatchLoop)^ do begin
			SetBackground(4);
			SetTextAttr($4F);
			Bounds(1,1,66,10, B);
			SetBounds(B);					
			FAutoEndModal := True;
			SetTitle('Oh, NO! Me so sorry! :(');
			SetMessage(' ');
			AppendMessage('');
			AppendMessage('Unfortunately, you did not use the launcher batch file');
			AppendMessage('PGM.BAT and PGME can only perform simple launching at');
			AppendMessage('this time.');
			AppendMessage(' ');
			AppendMessage('Maybe, next time start PGME using the PGM.BAT launcher.'); 
			SetHelpText('This is an error dialog box.');
			AddHelpText('');
			AddHelpText('It means what it says. You ran PGME directly. When you');
			AddHelpText('should be running it by using the PGM.BAT launcher file.');
			AddHelpText('PGME does not really care how you start it. But, it can');
			AddHelpText('only perform simple launching when you run it directly.');
			AddHelpText('Complex and Max Memory style launches are not available unless');
			AddHelpText('you start PGME by using PGM.BAT file.');
		end;
		ApplySettings(FNoBatchLoop);
	end;
	GetNoBatchLoop := FNoBatchLoop;
end;

procedure TPGME.CreateAllDialogs;
begin
	inherited CreateAllDialogs(ConserveMemory);
	GetNoTempDir;
	if ConserveMemory then FreeDialog(PDialog(FNoTempDir));
	GetNoBatchLoop;
	if ConserveMemory then FreeDialog(PDialog(FNoBatchLoop));
end;

procedure TPGME.ParseCommandLine;
var
	I : integer;
	O, V : String;
begin
	for I := 1 to ParamCount do begin
		V := Trim(ParamStr(I));
		if (Length(V) > 1) and (V[1] = '/') then begin
			O := UCase(Trim(PullStr('=', V)));
			if O = '/BAT' then 
				FBatLoop := True
			else if O = '/INIT' then 
				FInitRun := True
			else if O = '/M' then
				FReqMenu := V
			else if O = '/P' then
				FReqProg := V			
		end;
	end;
	if (FReqMenu <> '') and (FReqProg <> '') then
		FInitRun := False;
end;

procedure TPGME.BeforeShow;
{ var
	B : TBounds;
	I : integer; }
begin
	inherited BeforeShow;
	GotoXY(1, WhereY);
	TextAttr := $07;
	ClrEOL;
{	with PLabel(New(PLabel, Create(@Self, 'Label.PARMS')))^ do begin
		SetHelpText('PGME Startup Parameters:');
		AddHelpText('');
		for I := 1 to ParamCount do
			AddHelpText(ParamStr(I));
		Bounds(2,2,1,1, B);
		SetBounds(B);
		SetCaption('');
	end; }
end;

procedure TPGME.Configure;
begin
	if (Not FInitRun) then begin
		GotoXY(1, WhereY);
		TextAttr := $0B;
		Write('Resuming Eternity...');
		ClrEol; 
	end;
	inherited Configure;
	PMenuCache(FMenuCache)^.ApplyLanguage(@FLanguage, GetPathID);
	FEditTitle^.SetCaption(PMenuCache(FMenuCache)^.FNoMenuStr); {Set Initially to No Title}
	if FInitRun then begin
		GotoXY(1, WhereY);
		TextAttr := $0E;
		Write(FLanguage.GetValue(GetPathID + '.BOOT.CACHE.UPDATE', 'Updating Menu Cache...'));
		ClrEol;
	end;
	FConfig.OpenSection(GetPathID);
	PMenuCache(FMenuCache)^.Load;
	PMenuCache(FMenuCache)^.SetPaths(FConfig.GetValue('PATH.MENU', ''));
	UpdateMenuCache;
	OpenCurrentMenu; 
	{ Later add ability to return to a menu and program }
	if FInitRun then begin
		GotoXY(1, WhereY);
		TextAttr := $0B;
		Write(FLanguage.GetValue(GetPathID + '.BOOT.STARTING', 'Beginning Eternity...'));
		ClrEol;
	end;
end;

procedure TPGME.ApplyConfig( AConfig : PConfigFile );
begin
	inherited ApplyConfig(AConfig);
	AConfig^.OpenSection(GetPathID + '.VIEWERS');
	AConfig^.GetValue('TXT', 'QSCROLL.EXE');
	AConfig^.GetValue('DOC', 'QSCROLL.EXE');
	AConfig^.GetValue('ME', 'QSCROLL.EXE');
	AConfig^.OpenSection(GetPathID);
	FReadOnly := StrBool(AConfig^.GetValue('READONLY', BoolStr(FReadOnly)));
	FCanQuit := StrBool(AConfig^.GetValue('CANQUIT', BoolStr(FCanQuit)));
	if Not FCanQuit then 
		CallBack('', '', '', True);
	FBtnQuit^.SetEnabled(FCanQuit);
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
var
	D, M, I : PListItem;
begin
	FMenus^.BeginUpdate;
	FMenus^.ClearItems;
	FLastMenu := nil;
	PMenuCache(FMenuCache)^.Update;
	if PMenuCache(FMenuCache)^.Modified and (not FReadOnly) then
		PMenuCache(FMenuCache)^.Save;
	D := PMenuCache(FMenuCache)^.FCache^.First;
	while Assigned(D) do begin
		M := D^.First;
		while Assigned(M) do begin
			if PCacheMenu(M)^.Available then begin
				I := FMenus^.AddItem(PCacheMenu(M)^.ShortTitle);
				I^.Data := M;
			end;
			M := M^.Next;
		end;
		D := D^.Next;
	end;
	FMenus^.EndUpdate;
end;

procedure TPGME.OpenCurrentMenu;
var
	T, P :PListItem;
	S : String;
begin
	StartDrawing; 
	FLastProgram := nil;
	FPrograms^.BeginUpdate;
	FPrograms^.ClearItems;
	FWritable := False;
	if Assigned(FMenus^.FCurrent) and Assigned(FMenus^.FCurrent^.Data) then
		with PCacheMenu(FMenus^.FCurrent^.Data)^ do begin
			FWritable := Writable;
			FEditTitle^.SetCaption(LongTitle);
			P := First;
			while Assigned(P) do begin
				T := FPrograms^.AddItem(PCacheApp(P)^.Title);
				T^.Data := P;
				P := P^.Next;
			end;
		end;
	UpdateButtons;
	FLastMenu := FMenus^.FCurrent;
	FPrograms^.EndUpdate;
	FinishDrawing; 
end;

procedure TPGME.UpdateButtons;
begin
	FBtnEdit^.SetEnabled((Not FReadOnly) and FWritable);
	FBtnRun^.SetEnabled(Assigned(FPrograms^.FCurrent));
end;

procedure TPGME.CheckLocal(var AEvent : TEvent); 
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmListViewDouble : if AEvent.InfoPtr = FPrograms then begin
				ClearEvent(AEvent);
				PutCommand(cmExecute, @Self);
			end;
			cmNextMenu : begin
				ClearEvent(AEvent);
				FMenus^.Down(True);
			end;
			cmPrevMenu : begin
				ClearEvent(AEvent);
				FMenus^.Up(True);
			end;
		end; 
	inherited CheckLocal(AEvent);
	if FLastMenu <> FMenus^.FCurrent then begin
		OpenCurrentMenu;
	end else if (FLastProgram <> FPrograms^.FCurrent) then
		UpdateButtons;
	FLastMenu := FMenus^.FCurrent;
	FLastProgram := FPrograms^.FCurrent;

	if AEvent.What = evCommand then
		case AEvent.Command of
			cmExecute : begin
				ClearEvent(AEvent);
				ExecuteItem(FLastProgram)
			end;
		end; 
end;

procedure TPGME.ExecuteItem(AItem : PListItem); 
var
	FName,
	FExt,
	FOpts,
	FRet,
	FView,
	FTemp, S : String;
	F : TFile;
	OK : Boolean;
	P : PCacheProc;
begin
{	FBatLoop := true; }

	{ Add expand relative to Menu Path }
	{ Add parsing of ?:\ for removable media }
	{ Add Option prompting and parsing }
	{ Add Restore Current Paths }
	
	if Not Assigned(AItem) then exit;
	if Not Assigned(AItem^.Data) then exit;	
	FConfig.OpenSection(GetPathID + '.VIEWERS');
	with PCacheApp(AItem^.Data)^ do begin
		FRet := '/M=' + PCacheMenu(Parent)^.GetNameID + 
			' /P=' + GetNameID;
		FName := FileExpand(FileName);
		FExt := UCase(Copy(FName, LastPos('.', FName) + 1, Length(FName) ));
		FOpts := '';
		FView := FConfig.GetValue(FExt, '');
		if FView <> '' then begin
			FOpts := FName;
			FName := FileExpand(FView);
			FExt := UCase(Copy(FName, LastPos('.', FName) + 1, Length(FName) ));
		end;
		if SimpleExec and (not MaxMemory) and (Count = 0) then begin
			CallBack(FName, FOpts, FRet, ReturnAfter or (not FCanQuit));
			Terminate;
		end else begin
			FTemp := GetEnv('TEMP');
			if FTemp = '' then begin
				{ Cannot Do Complex, no temp dir } 
				GetNoTempDir;
				FNoTempDir^.ShowModal;		
				exit;
			end;
			if not FBatLoop  then begin
				{ Cannot Do Complex, no batch looping } 
				GetNoBatchLoop;
				FNoBatchLoop^.ShowModal;		
				exit;
			end;
			F.Create;
			F.Assign(Dir(FTemp) + 'PGMEGORN.BAT');
			F.Rewrite;
			
			OK := (F.IOResult = 0);
			if OK then begin
				S := '@ECHO OFF';
				F.WriteLn(S);
				OK := (F.IOResult = 0);				
			end;
			
			P := PCacheProc(First);
			if OK then begin
				if Not assigned(P) then begin
					if FExt = 'BAT' then
						S := 'CALL ' + FName + ' ' + FOpts
					else
						S := FName + ' ' + FOpts;
						F.WriteLn(S);
					OK := (F.IOResult = 0);
				end else while Assigned(P) and OK do begin
					S := P^.GetNameID;
					F.WriteLn(S);
					P := PCacheProc(P^.Next);
				end;
			end;
			
			if OK then begin
				S := '';
				F.WriteLn(S);
				OK := (F.IOResult = 0);				
			end;
			if OK and (ReturnAfter or (not FCanQuit)) then begin
				S := 'PGM.BAT ' + FRet;
				F.WriteLn(S);
				OK := (F.IOResult = 0);				
			end;
			if OK then begin
				F.Close;
				OK := (F.IOResult = 0);				
			end;
			F.Destroy;
			if OK then 
				Terminate
			else begin
{				GetGornError;
				FGornError^.ShowModal;}
			end;
		end;
	end;
end;

{$ENDIF}
