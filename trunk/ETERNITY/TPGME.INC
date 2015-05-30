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
			FIdleCount : integer;
			FTimeOut : integer;
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
			FNoTempDir, FNoBatchLoop, FGornError,
			FExecMissing : PDialog;
			FDlgPAdd, FDlgPEdit, FDlgPDel,
			FDlgMAdd, FDlgMEdit, FDlgMDel,
			FDlgPGMOpt, FDlgPGMCfg : PDialog;			
			FFind : record
				FLabel : PLabel;
				FEdit : PEdit;
			end;
			FDate, FTime : PLabel;
			FMenuCache : PObject;
			FEditMenu : PControl;
			FBtnPAdd, FBtnPEdit, FBtnPDel,
			FBtnMAdd, FBtnMEdit, FBtnMDel,
			FBtnPGMOpt, FBtnPGMCfg : PButton;
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
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		procedure Idle; virtual;
		procedure ExecuteItem(AItem : PListItem); virtual;
		function GetNoTempDir : PDialog; virtual;
		function GetNoBatchLoop : PDialog; virtual;
		function GetGornError : PDialog; virtual;
		function GetExecMissing : PDialog; virtual;
		function GetEditMenu : PControl; virtual;
		function GetDlgPAdd : PControl; virtual;
		function GetDlgPEdit : PControl; virtual;
		function GetDlgPDelete : PControl; virtual;
		function GetDlgMAdd : PControl; virtual;
		function GetDlgMEdit : PControl; virtual;
		function GetDlgMDelete : PControl; virtual;
		function GetDlgPGMOpt : PControl; virtual;		
		function GetDlgPGMCfg : PControl; virtual;
		function GetReturnStr : String; virtual;		
		procedure CreateAllDialogs(ConserveMemory : boolean); virtual;
		procedure Restart; virtual;
		procedure ScreenSaver; virtual;
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
	FIdleCount := 0;
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
	FGornError := nil;
	FExecMissing := nil;
	FEditMenu := nil;
	FBtnPAdd := nil;
	FBtnPEdit := nil;
	FBtnPDel := nil;
	FBtnMAdd := nil;
	FBtnMEdit := nil;
	FBtnMDel := nil;
	FBtnPGMOpt := nil;
	FDlgPAdd := nil;
	FDlgPEdit := nil;
	FDlgPDel := nil;
	FDlgMAdd := nil;
	FDlgMEdit := nil;
	FDlgMDel := nil;
	FDlgPGMOpt := nil;
	FDlgPGMCfg := nil;
	FTimeOut := 0;
	OldIdleProc := QCrt.IdleProc;
	QCrt.IdleProc := PGMIdleProc;
	
	ParseCommandLine;
	
	FMenuCache := New(PMenuCache, Create);
	
	FInfo.SetFileName('PGM.EXE');
	FInfo.SetTitle('Program Manager Eternity');
	SetHelpText('Program Manager Eternity Edition Help System');
	AddHelpText('');
	AddHelpText('The help system is context sensitive based on the current active');
	AddHelpText('window. You can close the help window by clicking its close button');
	AddHelpText('or outside the window with the mouse. Also, by using the keyboard');
	AddHelpText('to generate a CloseWindow or Escape command (Default Alt+W or Esc).');
	AddHelpText('Generating a Exit program command (Alt+Q) will close help and exit');
	AddHelpText('the program. This assumes that the configuration option for CANCLOSE');
	AddHelpText('is not set to FALSE. ');
	
	{ Left Pane }
	TextAttr := $06;
	Bounds(1,1,Application^.Width - Split,5, B);
	FProgramWindow := New(PWindow, Create(@Self, 'PROGRAMS'));
	with PWindow(FProgramWindow)^ do begin
		SetHelpText('The main Program List window:');
		AddHelpText('');

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
			SetHelpText('5;"%0" Button: Assuming that PGME is not running in KIOSK mode');
			AddHelpText('and allows you to quit to a command prompt this button will');
			AddHelpText('shutdown PGME.');
			AddHelpText('The default keyboard shortcut is "ALT+Q".');
			SetCaption('Quit');
			SetCommand(cmQuit);
			SetAnchors(alBottom or alRight);
			SetBounds(B);	
		end;
		
		{  Execute Button }
		Bounds(1, 1, 11, 1, B);
		FBtnRun := New(PButton, Create(FProgramWindow, 'BUTTON.RUN'));
		with FBtnRun^ do begin
			SetHelpText('5;"%0" Button: This will launch the currently selected program.');
			AddHelpText('The default keyboard shortcut is "ENTER".');
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
			SetHelpText('5;"%0" Button: This will open a help window.');
			AddHelpText('The default keyboard shortcut is "ALT+H".');
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
			SetHelpText('5;"%0" Button: This will display the edit popup menu.');
{			AddHelpText('The default keyboard shortcut is "ALT+E".');}
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

function TPGME.GetGornError;
var
	B : TBounds;
begin
	if Not Assigned(FGornError) then begin
		TextAttr := $4F;
		FGornError := New(PMessageBox, Create(@Self, 'DIALOG.GORNERROR'));
		with PMessageBox(FGornError)^ do begin
			SetBackground(4);
			SetTextAttr($4F);
			Bounds(1,1,66,10, B);
			SetBounds(B);					
			FAutoEndModal := True;
			SetTitle('Oh, NO! Me so sorry! :(');
			SetMessage(' ');
			AppendMessage('');
			AppendMessage('An error occurred when trying to write to the PGMEGORN.BAT');
			AppendMessage('temporary file. The file may be locked, readonly or just');
			AppendMessage('unaccessible at this time.');
			AppendMessage(' ');
			AppendMessage('Maybe, if you delete the file manually it will fix it.'); 
			SetHelpText('This is an error dialog box.');
			AddHelpText('');
			AddHelpText('PGME encountered an error while trying to create a temporary');
			AddHelpText('file called PGMEGORN.BAT in your TEMP directory. PGME needs to be');
			AddHelpText('able to write to the "Go Run" file in order to free the');
			AddHelpText('maximum amount of memory and to perform complex program launching.');
			AddHelpText('If you can not give PGME write access to that location or file,');
			AddHelpText('PGME can only perform simple launching. You may be able');
			AddHelpText('to resolve the issue by deleting it manually, rebooting the');
			AddHelpText('computer or standing on one leg, hopping up and down and');
			AddHelpText('spelling your full name backwards three times.'); 
		end;
		ApplySettings(FGornError);
	end;
	GetGornError := FGornError;
end;

function TPGME.GetExecMissing;
var
	B : TBounds;
begin
	if Not Assigned(FExecMissing) then begin
		TextAttr := $4F;
		FExecMissing := New(PMessageBox, Create(@Self, 'DIALOG.ExecMissing'));
		with PMessageBox(FExecMissing)^ do begin
			SetBackground(4);
			SetTextAttr($4F);
			Bounds(1,1,66,9, B);
			SetBounds(B);					
			FAutoEndModal := True;
			SetTitle('Oh, NO! Me so sorry! :(');
			SetMessage(' ');
			AppendMessage('');
			AppendMessage('PGME was unable to locate a required executable item and');
			AppendMessage('can not run that menu entry at this time.');
			AppendMessage(' ');
			AppendMessage('Maybe, verify that the program has not been relocated.'); 
			SetHelpText('This is an error dialog box.');
			AddHelpText('');
			AddHelpText('PGME was unable to locate a program file needed to launch');
			AddHelpText('the requested item.');
			AddHelpText('The "Required" program is specified in the FILE field of the');
			AddHelpText('appropriate menu. Its path is absolute or relative to the');
			AddHelpText('menu file. If the FILE field is defined, it is tested');
			AddHelpText('for both Complex and Simple launching.');
			AddHelpText('There are a couple of things that can cause');
			AddHelpText('this error to occur. First, the program has been deleted or moved.');
			AddHelpText('Second, it was located on removable media that is not currently');
			AddHelpText('available. Finally, the main file is associated with a viewer');
			AddHelpText('helper in the ETERNITY.CFG file and the viewer could not');
			AddHelpText('be found.');
			
		end;
		ApplySettings(FExecMissing);
	end;
	GetExecMissing := FExecMissing;
end;

function TPGME.GetEditMenu : PControl; 
var
	B : TBounds;
begin
	if Not Assigned(FEditMenu) then begin
		TextAttr := $04;
		Bounds(FBtnEdit^.Left + FBtnEdit^.Width - 21, FBtnEdit^.Top - 12, 22, 12, B);
		FEditMenu := New(PPopup, Create(PControl(FBtnEdit^.Parent), 'POPUP.EDITMENU'));
		with PPopUp(FEditMenu)^ do begin
			SetHelpText('A note regarding the edit button popup menu shortcut keys.');
			AddHelpText('They will only work when popup menu is visible.');
			SetShadow(False);
			SetBounds(B);	
			SetAnchors(abBottom or abRight);
			Bounds(0,4,Width, 1, B);
			with PLine(New(PLine, Create(FEditMenu, 'LINE.UPPER')))^ do begin
				SetStyle(lnSingle or lnHorizontal);
				SetAllowedOutside(True);
				SetAnchors(abLeft or abRight or abTop);
				SetBounds(B);
			end;
			Bounds(0,8,Width, 1, B);
			with PLine(New(PLine, Create(FEditMenu, 'LINE.LOWER')))^ do begin
				SetStyle(lnSingle or lnHorizontal);
				SetAllowedOutside(True);
				SetAnchors(abLeft or abRight or abBottom);
				SetBounds(B);
			end;
			{TextAttr := $0A;}
			TextAttr := $0E;
			Bounds(1,1,Width, 1, B);
			FBtnPAdd := New(PButton, Create(FEditMenu, 'BTN.PROG.ADD'));
			with PButton(FBtnPAdd)^ do begin
				SetHelpText('5;"%0" Option: Select this to add a program to the current menu.');
				AddHelpText('The default keyboard shortcut is "A".');
				SetCaption('Add Program...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmProgAdd);
				SetKeycode(kbA, 0, kfMatches);
			end;
			{TextAttr := $0E;}
			Inc(B.Top);
			FBtnPEdit := New(PButton, Create(FEditMenu, 'BTN.PROG.EDIT'));
			with PButton(FBtnPEdit)^ do begin
				SetHelpText('5;"%0" Option: Select this to edit the selected programs information or');
				AddHelpText('its execution process. The default keyboard shortcut is "E".');
				SetCaption('Edit Program...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmProgEdit);
				SetKeycode(kbE, 0, kfMatches);
			end;
			{TextAttr := $0C;}
			Inc(B.Top);
			FBtnPDel := New(PButton, Create(FEditMenu, 'BTN.PROG.DELETE'));
			with PButton(FBtnPDel)^ do begin
				SetHelpText('5;"%0" Option: Select delete the currently selected program from the menu.');
				AddHelpText('The default keyboard shortcut is "D".');
				SetCaption('Delete Program...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmProgDel);
				SetKeycode(kbD, 0, kfMatches);
			end;
			{TextAttr := $0A;}
			Inc(B.Top, 2);
			FBtnMAdd := New(PButton, Create(FEditMenu, 'BTN.MENU.ADD'));
			with PButton(FBtnMAdd)^ do begin
				SetHelpText('5;"%0" Option: Select this to add a new menu.');
				AddHelpText('The default keyboard shortcut is "N".');
				SetCaption('Add Menu...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmMenuAdd);
				SetKeycode(kbN, 0, kfMatches);
			end;
			{TextAttr := $0E;}
			Inc(B.Top);
			FBtnMEdit := New(PButton, Create(FEditMenu, 'BTN.MENU.EDIT'));
			with PButton(FBtnMEdit)^ do begin
				SetHelpText('5;"%0" Option: Select this to edit the current menus information.');
				AddHelpText('The default keyboard shortcut is "N".');
				SetCaption('Edit Menu...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmMenuEdit);
				SetKeycode(kbM, 0, kfMatches);
			end;
			{TextAttr := $0C;}
			Inc(B.Top);
			FBtnMDel := New(PButton, Create(FEditMenu, 'BTN.MENU.DELETE'));
			with PButton(FBtnMDel)^ do begin
				SetHelpText('5;"%0" Option: Select this to delete the current menu.');
				AddHelpText('The default keyboard shortcut is "K".');
				SetCaption('Delete Menu...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmMenuDel);
				SetKeycode(kbK, 0, kfMatches);
			end;
			TextAttr := $0B; 
			Inc(B.Top, 2);
			FBtnPGMOpt := New(PButton, Create(FEditMenu, 'BTN.OPTIONS'));
			with PButton(FBtnPGMOpt)^ do begin
				SetHelpText('5;"%0" Option: Select this modify PGME display options.');
				AddHelpText('The default keyboard shortcut is "O".');
				SetCaption('Display Options...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmPGMOpts);
				SetKeycode(kbO, 0, kfMatches);
			end;
			Inc(B.Top);
			FBtnPGMCfg := New(PButton, Create(FEditMenu, 'BTN.CONFIG'));
			with PButton(FBtnPGMCfg)^ do begin
				SetHelpText('5;"%0" Option: Select this modify PGME behavior options.');
				AddHelpText('The default keyboard shortcut is "P".');
				SetCaption('Config Settings...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmPGMCfg);
				SetKeycode(kbP, 0, kfMatches);
			end;
		end;
		ApplySettings(FEditMenu);
	end;
	FBtnPAdd^.SetEnabled(FWritable and Assigned(FMenus^.FCurrent));
	FBtnPEdit^.SetEnabled(FWritable and Assigned(FMenus^.FCurrent) and Assigned(FPrograms^.FCurrent));
	FBtnPDel^.SetEnabled(FWritable and Assigned(FMenus^.FCurrent) and Assigned(FPrograms^.FCurrent));
	FBtnMAdd^.SetEnabled((Not FReadOnly));
	FBtnMEdit^.SetEnabled(FWritable and Assigned(FMenus^.FCurrent));
	FBtnMDel^.SetEnabled(FWritable and Assigned(FMenus^.FCurrent));
	FBtnPGMOpt^.SetEnabled(Not FReadOnly);
	FBtnPGMCfg^.SetEnabled(Not FReadOnly);
	GetEditMenu := FEditMenu;	
end;

function TPGME.GetDlgPAdd : PControl;
var
	B : TBounds;
begin
	if Not Assigned(FDlgPAdd) then begin
		FDlgPAdd := New(PDlgPAdd, Create(@Self));
		ApplySettings(FDlgPAdd);
	end;
	GetDlgPAdd := FDlgPAdd;
end;

function TPGME.GetDlgPEdit : PControl; 
var
	B : TBounds;
begin
	if Not Assigned(FDlgPEdit) then begin
		FDlgPEdit := New(PDlgPEdit, Create(@Self));
		ApplySettings(FDlgPEdit);
	end;
	GetDlgPEdit := FDlgPEdit;
end;

function TPGME.GetDlgPDelete : PControl;
var
	B : TBounds;
begin
	if Not Assigned(FDlgPDel) then begin
		FDlgPDel := New(PDlgPDel, Create(@Self));
		ApplySettings(FDlgPDel);
	end;
	GetDlgPDelete := FDlgPDel;
end;

function TPGME.GetDlgMAdd : PControl; 
var
	B : TBounds;
begin
	if Not Assigned(FDlgMAdd) then begin
		FDlgMAdd := New(PDlgMAdd, Create(@Self));
		ApplySettings(FDlgMAdd);
	end;
	GetDlgMAdd := FDlgMAdd;
end;

function TPGME.GetDlgMEdit : PControl;
var
	B : TBounds;
begin
	if Not Assigned(FDlgMEdit) then begin
		FDlgMEdit := New(PDlgMEdit, Create(@Self));
		ApplySettings(FDlgMEdit);
	end;
	GetDlgMEdit := FDlgMEdit;
end;

function TPGME.GetDlgMDelete : PControl; 
var
	B : TBounds;
begin
	if Not Assigned(FDlgMDel) then begin
		FDlgMDel := New(PDlgMDel, Create(@Self));
		ApplySettings(FDlgMDel);
	end;
	GetDlgMDelete := FDlgMDel;
end;

function TPGME.GetDlgPGMOpt : PControl;
var
	B : TBounds;
begin
	if Not Assigned(FDlgPGMOpt) then begin
		FDlgPGMOpt := New(PDlgPGMOpt, Create(@Self));
		ApplySettings(FDlgPGMOpt);
	end;
	GetDlgPGMOpt := FDlgPGMOpt;
end;

function TPGME.GetDlgPGMCfg : PControl;
var
	B : TBounds;
begin
	if Not Assigned(FDlgPGMCfg) then begin
		FDlgPGMCfg := New(PDlgPGMCfg, Create(@Self));
		ApplySettings(FDlgPGMCfg);
	end;
	GetDlgPGMCfg := FDlgPGMCfg;
end;

procedure TPGME.CreateAllDialogs;
begin
	inherited CreateAllDialogs(ConserveMemory);
	GetEditMenu;
	GetNoTempDir;
	if ConserveMemory then FreeDialog(PDialog(FNoTempDir));
	GetNoBatchLoop;
	if ConserveMemory then FreeDialog(PDialog(FNoBatchLoop));
	GetGornError;
	if ConserveMemory then FreeDialog(PDialog(FGornError));
	GetExecMissing;
	if ConserveMemory then FreeDialog(PDialog(FExecMissing));
	GetDlgPAdd;
	if ConserveMemory then FreeDialog(PDialog(FDlgPAdd));
	GetDlgPEdit;
	if ConserveMemory then FreeDialog(PDialog(FDlgPEdit));
	GetDlgPDelete;
	if ConserveMemory then FreeDialog(PDialog(FDlgPDel));
	GetDlgMAdd;
	if ConserveMemory then FreeDialog(PDialog(FDlgMAdd));
	GetDlgMEdit;
	if ConserveMemory then FreeDialog(PDialog(FDlgMEdit));
	GetDlgMDelete;
	if ConserveMemory then FreeDialog(PDialog(FDlgMDel));
	GetDlgPGMOpt;
	if ConserveMemory then FreeDialog(PDialog(FDlgPGMOpt));
	GetDlgPGMCfg;
	if ConserveMemory then FreeDialog(PDialog(FDlgPGMCfg));
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
			V := uCase(Trim(V));
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
	FInSound := True;
	if (FReqMenu <> '') and (FReqProg <> '') then
		FInitRun := False;
end;

procedure TPGME.BeforeShow;
var
	T, P :PListItem;
	S : String;
begin
	inherited BeforeShow;
	GotoXY(1, WhereY);
	TextAttr := $07;
	ClrEOL;
	{ if requested from command line open menu }
	if Not (Assigned(FMenus) and Assigned(FMenuCache)) then exit;
	if FReqMenu <> '' then begin
		P := FMenus^.FItems.First;
		while Assigned(P) do begin
			S := UCase(PCacheDir(PCacheMenu(P^.Data)^.Parent)^.GetNameID + 
				PCacheMenu(P^.Data)^.GetNameID);
			if S = FReqMenu then begin
				FMenus^.FCurrent := P;
				OpenCurrentMenu;
				P := nil;
			end else
				P := P^.Next;
		end;
	end;
	{ if requested from command line select program }
	if Not (Assigned(FPrograms) and Assigned(FMenuCache)) then exit;
	if FReqProg <> '' then begin
		P := FPrograms^.FItems.First;
		while Assigned(P) do begin
			S := PCacheApp(P^.Data)^.GetNameID;
			if S = FReqProg then begin
				FPrograms^.FCurrent := P;
				P := nil;
			end else
				P := P^.Next;
		end;
	end;
	UpdateButtons;
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
	AConfig^.SetValue('TXT', AConfig^.GetValue('TXT', 'QSCROLL.EXE'));
	AConfig^.SetValue('DOC', AConfig^.GetValue('DOC', 'QSCROLL.EXE'));
	AConfig^.SetValue('ME', AConfig^.GetValue('ME', 'QSCROLL.EXE'));
	AConfig^.OpenSection(GetPathID);
	FReadOnly := StrBool(AConfig^.GetValue('READONLY', BoolStr(FReadOnly)));
	FCanQuit := StrBool(AConfig^.GetValue('CANQUIT', BoolStr(FCanQuit)));
	if Not FCanQuit then 
		CallBack('', '', '', True); { Preset return to PGME, incase of crash }
	FBtnQuit^.SetEnabled(FCanQuit);
	FProgramWindow^.SetCanClose(FCanQuit);
	AConfig^.GetInts('SCREEN_TIMEOUT', FTimeOut, 1);
	UpdateButtons;
end;

procedure TPGME.Idle;
const
	I : word = 100;
	S : word = 100;
	L : longInt = 0;
var
	Ticks : LongInt absolute $0040:$006c;
	Hour, Min, Sec, Sec100 : word;
begin
	inherited Idle;
	if (Not Assigned(Application)) or (Not Application^.GetVisible) then exit;
	if Ticks = L then Exit;
	L := Ticks;

	GetTime(Hour, Min, Sec, Sec100);
	if Sec <> I then begin
		I := Sec;
		Inc(FIdleCount);
	end;	
	if EventFlag then begin
		EventFlag := False;
		FIdleCount := 0;
	end;
	if ((FTimeOut = 0) and (FIdleCount >= 30)) or 
	((FTimeOut > 0) and (FIdleCount >= FTimeOut * 60)) then 
		ScreenSaver;
	{ If a Subwindow or Dialog is visible then don't update clock, QuickCrt doesn't
	 support Write-Behind yet. This list keeps getting longer and longer. I really
	 need to add write behined! }
	if Assigned(FHelp) and FHelp^.GetVisible then Exit;
	if Assigned(FNoTempDir) and FNoTempDir^.GetVisible then Exit;
	if Assigned(FNoBatchLoop) and FNoBatchLoop^.GetVisible then Exit;
	if Assigned(FGornError) and FGornError^.GetVisible then Exit;
	if Assigned(FExecMissing) and FExecMissing^.GetVisible then Exit;
	if Assigned(FDlgPAdd) and FDlgPAdd^.GetVisible then Exit;
	if Assigned(FDlgPEdit) and FDlgPEdit^.GetVisible then Exit;
	if Assigned(FDlgPDel) and FDlgPDel^.GetVisible then Exit;
	if Assigned(FDlgMAdd) and FDlgMAdd^.GetVisible then Exit;
	if Assigned(FDlgMEdit) and FDlgMEdit^.GetVisible then Exit;
	if Assigned(FDlgMDel) and FDlgMDel^.GetVisible then Exit;
	if Assigned(FDlgPGMOpt) and FDlgPGMOpt^.GetVisible then Exit;
	if Assigned(FDlgPGMCfg) and FDlgPGMCfg^.GetVisible then Exit;
	
	if S <> Sec then begin
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
	if FLastMenu = FMenus^.FCurrent then exit;
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
	FBtnEdit^.SetEnabled(Not FReadOnly);
	FBtnRun^.SetEnabled(Assigned(FPrograms^.FCurrent));
end;

procedure TPGME.CheckLocal(var AEvent : TEvent); 
var
	R : integer;
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
	
	if (AEvent.What = evCommand) and (not FReadOnly) then
		case AEvent.Command of
			cmEditFile : begin
				ClearEvent(AEvent);
				GetEditMenu;
				R := PWindow(FEditMenu)^.ShowModal;
			end;
			cmMenuAdd : begin
				ClearEvent(AEvent);
				GetDlgMAdd;
				R := PDialog(FDlgMAdd)^.ShowModal;
			end;
			cmPGMOpts : begin
				ClearEvent(AEvent);
				GetDlgPGMOpt;
				R := PDialog(FDlgPGMOpt)^.ShowModal;
				if R = mrOK then begin
					PDlgPGMOpt(FDlgPGMOpt)^.Save;
					CallBack('', '', GetReturnStr, True);
					Restart;							
				end;
			end;
			cmPGMCfg : begin
				ClearEvent(AEvent);
				GetDlgPGMCfg;
				R := PDialog(FDlgPGMCfg)^.ShowModal;
				if R = mrOK then begin
					PDlgPGMCfg(FDlgPGMCfg)^.Save;
					CallBack('', '', GetReturnStr, True);
					Restart;
				end;
			end;
		end; 

	if (AEvent.What = evCommand) and (not FReadOnly) and FWritable then
		case AEvent.Command of
			cmMenuEdit : begin
				GetDlgMEdit;
				R := PDialog(FDlgMEdit)^.ShowModal;
			end;
			cmMenuDel : begin
				GetDlgMDelete;
				R := PDialog(FDlgMDel)^.ShowModal;
			end;
			cmProgAdd : begin
				GetDlgPAdd;
				R := PDialog(FDlgPAdd)^.ShowModal;
			end;
			cmProgEdit : begin
				GetDlgPEdit;
				R := PDialog(FDlgPEdit)^.ShowModal;
			end;
			cmProgDel : begin
				GetDlgPDelete;
				R := PDialog(FDlgPDel)^.ShowModal;
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

function TPGME.GetReturnStr : String; 
begin
	GetReturnStr := '';
	if not Assigned(FMenuCache) then exit;
	if Assigned(FPrograms) and Assigned(FPrograms^.FCurrent) then 
		with PCacheApp(FPrograms^.FCurrent^.Data)^ do 
			GetReturnStr := '/M=' + PCacheDir(Parent^.Parent)^.GetNameID + 
				PCacheMenu(Parent)^.GetNameID + ' /P=' + GetNameID;
end;

procedure TPGME.ExecuteItem(AItem : PListItem); 
var
	FHoldDir,
	FDir,
	FExecDir,
	FName,
	FExt,
	FOpts,
	FRet,
	FView,
	FTemp, S, S2 : String;
	F : TFile;
	OK : Boolean;
	P : PCacheProc;
	
	procedure FileWrite(S : String);
	begin
		if OK then begin
			F.WriteLn(S);
			OK := (F.IOResult = 0);				
		end;
	end;
	
	function CheckViewer : boolean;
	begin
		CheckViewer := True;
		with PCacheApp(AItem^.Data)^ do begin
			FExt := UCase(Copy(FName, LastPos('.', FName) + 1, Length(FName) ));
			FView := FConfig.GetValue(FExt, '');
			if SimpleExec and (not MaxMemory) and (Count = 0) and (FName <> '') and 
			(not PGMVars) and (FView = '') and (FExt = 'BAT') then begin
				FExecDir := PathOf(FName);
				FView := GetEnv('COMSPEC') + ' /C';
			end;
			if FView <> '' then begin
				S2 := FView;
				S := Trim(PullStr(' ', S2));
				S2 := Trim(S2);
				if S2 <> '' then S2 := S2 + ' ';
				FOpts := S2 + FName;
				FName := FileExpand(S);
				FExt := UCase(Copy(FName, LastPos('.', FName) + 1, Length(FName) ));
				if not FileExists(FName) then begin
					CheckViewer := False;
					GetExecMissing;
					FExecMissing^.ShowModal;
					Exit;
				end;
			end;
		end;
	end;
	
begin
	(* 
	 * Add Option prompting and parsing
	 * Add PGME Value as Formatting to Options like %E for eternity path...
	 *)
	
	if Not Assigned(AItem) then exit;
	if Not Assigned(AItem^.Data) then exit;	

	FConfig.OpenSection(GetPathID + '.VIEWERS');
	with PCacheApp(AItem^.Data)^ do begin
		FDir := PCacheDir(Parent^.Parent)^.GetNameID;
		FExecDir := '';
		GetDir(0, FHoldDir);
		ChDir(FDir);
		FRet := GetReturnStr;
		if FRet = '' then 
			FRet := '/M=' + FDir + PCacheMenu(Parent)^.GetNameID + 
			' /P=' + GetNameID;
		if FileName <> '' then begin
			FName := FileExpand(FileName);
			if not FileExists(FName) then begin
				GetExecMissing;
				FExecMissing^.ShowModal;
				Exit;
			end;
		end	else
			FName := '';
		FOpts := '';
		if not CheckViewer then exit;
		ChDir(FHoldDir);
		if SimpleExec and (not MaxMemory) and (Count = 0) and (FName <> '') and 
		(not PGMVars) then begin
			if FExecDir <> '' then 
				FOriginalPath := FExecDir;
			if Tone <> '' then 
				FEndTone := Tone;
			CallBack(FName, FOpts, FRet, ReturnAfter or (not FCanQuit));
			Terminate;
		end else begin
			FTemp := GetEnv('TEMP');
			
			if FTemp = '' then begin
				{ Cannot Do Complex, no temp dir } 
				GetNoTempDir;
				FNoTempDir^.ShowModal;		
				Exit;
			end;
			if not FBatLoop  then begin
				{ Cannot Do Complex, no batch looping } 
				GetNoBatchLoop;
				FNoBatchLoop^.ShowModal;		
				Exit;
			end;
			F.Create;
			F.Assign(Dir(FTemp) + 'PGMEGORN.BAT');
			F.Rewrite;
			
			OK := (F.IOResult = 0);
			FileWrite('@ECHO OFF');
			FileWrite('');
			if PGMVars then 
				S2 := 'SET '
			else
				S2 := 'REM SET ';
			
			FileWrite(S2 + 'PGME=' + GetExePath);
			FileWrite(S2 + 'MENU=' + FDir + PCacheMenu(Parent)^.GetNameID );
			FileWrite(S2 + 'PROG=' + GetNameID);
			FileWrite('');
			FileWrite('REM Begin by setting current dir and drive to menu directory.');
			FileWrite(Copy(FDir, 1, 2));
			FileWrite('CD ' + Copy(FDir, 3, Length(FDir)));
			FileWrite('');
			FileWrite('REM Continue by running complex launching processes.');

			P := PCacheProc(First);
			if OK then begin
				if Not assigned(P) then begin
					if FExt = 'BAT' then
						FileWrite('CALL ' + FName + ' ' + FOpts)
					else
						FileWrite(FName + ' ' + FOpts);
				end else while Assigned(P) and OK do begin
					FileWrite(P^.GetNameID);
					P := PCacheProc(P^.Next);
				end;
			end;

			if PGMVars then begin			
				FileWrite('');
				FileWrite('REM Clear PGME Environment Variables');
				FileWrite(S2 + 'PGME=');
				FileWrite(S2 + 'MENU=');
				FileWrite(S2 + 'PROG=');
			end;
			
			FileWrite('');
			FileWrite('REM Return to Program Manager Eternity');
			FileWrite('PGM.BAT ' + FRet);

			if OK then begin
				F.Close;
				OK := (F.IOResult = 0);				
			end;
			F.Destroy;
			if OK then begin
				if Tone <> '' then 
					FEndTone := Tone;
				Terminate
			end else begin
				GetGornError;
				FGornError^.ShowModal;
			end;
		end;
	end;
end;

procedure TPGME.Restart;
begin
	FTheme.OpenSection(GetNameID);
	FEndTone := FTheme.GetValue('TONE.RESTART', FEndTone);
	CallBack('', '', GetReturnStr, True);
	Terminate;
	FOutSound := True;
end;

procedure TPGME.ScreenSaver;
var
	S : String;
	E : TEvent;
begin
	FConfig.OpenSection(GetPathID);
	S := FileExpand(FConfig.GetValue('SCREEN_SAVER', 'BLANKER.SCR'));
	if FileExists(S) then begin
		CallBack(S, '', GetReturnStr, True);
		FOutSound := False;
		Terminate;
	end else begin
		Hide;
		TextAttr := $07;
		ClrScr;
		TurnMouseOff;
		repeat
			FIdleCount := 0;
			GetEvent(E);
		until E.What <> evNothing;
		PurgeEvents;
		TurnMouseOn;
		Show;
	end;
end;

procedure TPGME.ApplyTheme;
begin
	inherited ApplyTheme(AConfig);
	AConfig^.OpenSection(GetNameID);
	with AConfig^ do begin
		SetValue('TONE.RESTART', GetValue('TONE.RESTART', '400:1,600:1,800:1,200:1'));
		SetValue('TONE.RETURN', GetValue('TONE.RETURN', '800:2,0:1,600:2,0:1,400:6,0:2'));
	end;
	if FInitRun = false then
		FStartTone := AConfig^.GetValue('TONE.RETURN', FStartTone);
end;
{$ENDIF}
