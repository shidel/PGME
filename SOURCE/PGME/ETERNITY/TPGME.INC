{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ --- TPGME --- }

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
			FDlgPGMOpt, FDlgPGMCfg,
			FDlgLaunch,
			FDlgFindExec, FDlgOptExec,
			FDlgChangeLang, FDlgChangeTheme,
			FDlgAbout, FDlgDosCommand,
			FDlgMove : PDialog;
			FFind : record
				FLabel : PLabel;
				FEdit : PEdit;
			end;
			FDate, FTime : PLabel;
			FMenuCache : PObject;
			FEditMenu : PControl;
			FBtnPAdd, FBtnPEdit, FBtnPDel, FBtnPMove,
			FBtnMAdd, FBtnMEdit, FBtnMDel,
			FBtnPGMOpt, FBtnPGMCfg : PButton;
			FResumeText, FPauseText, FReqSearch : String;
			FRetError, FRetWait : integer;
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
		procedure AtStartup; virtual;
		procedure Idle; virtual;
		procedure ExecuteItem(AItem : PListItem); virtual;
		function GetChangeLang(ATheme, ALang: String) : PDialog; virtual;
		function GetChangeTheme(ATheme, ALang: String) : PDialog; virtual;
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
		function GetDlgLaunch : PControl; virtual;
		function GetDlgFindExec : PControl; virtual;
		function GetDlgOptExec : PControl; virtual;
		function GetDlgAbout : PControl; virtual;
		function GetDlgMove : PControl; virtual;
		function GetDlgDosCommand : PControl; virtual;
		function GetReturnStr (CanWait : boolean): String; virtual;
		procedure CreateAllDialogs(ConserveMemory : boolean); virtual;
		procedure Restart; virtual;
		procedure ScreenSaver; virtual;
		procedure DeleteMenu; virtual;
		procedure DeleteProgram; virtual;
		procedure DeleteCache; virtual;
		procedure SelectMenu(AValue : String); virtual;
		procedure SelectProg(AValue : String); virtual;
		procedure CheckLanguage(ATheme : String);
		procedure CheckTheme(ALanguage : String);
		procedure AddMenu; virtual;
		procedure AddProgram; virtual;
		procedure ModifyMenu; virtual;
		procedure ModifyProgram; virtual;
		procedure CopyOrMove(AMove : boolean); virtual;
		procedure UpdateProgram(AMenu : TConfigFile; ADlg : PDialog); virtual;
		function  SearchStr(AValue : String) : String; virtual;
		procedure Search(AValue : String); virtual;
	end;

{ Some Global Variables }
var
	PGME : TPGME;

{$ENDIF}

{$IFDEF IMPLEMENTATION}

const
    cHideMode : array[0..2] of string = ('NO', 'AUTO', 'YES');

var
	OldIdleProc : procedure;

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
	I : integer;
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
	FDlgChangeLang := nil;
	FDlgChangeTheme := nil;
	FDlgLaunch := nil;
	FDlgAbout := nil;
	FDlgDosCommand := nil;
	FDlgMove := nil;
	FTimeOut := 0;
	FRetError := 0;
	FRetWait := 0;
	OldIdleProc := QCrt.IdleProc;
	QCrt.IdleProc := PGMIdleProc;

	ParseCommandLine;
	{
	For I := 0 to ParamCount do
	    WriteLn(I, '="', ParamStr(I), '"');
	Pause;
	}

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
			SetHelpText('5;"%0" Button: This will focus the "Find" field ');
			AddHelpText('to input text for searching. Or, if that field');
			AddHelpText('already contains text, it will search again. With');
			AddHelpText('the default settings, pressing "ALT+F" will also');
			AddHelpText('focus the "Find" field.');
			SetCommand(cmFind);
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
			SetCommand(cmEditItem);
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
		FVersion := PLabel(New(PButton, Create(P, 'SAM_I_AM')));
		with FVersion^ do begin
			OnFormat := FormatVersion;
			SetCaption('0[Z4,X3.2],1,2[Z2];Version %1/%2/%0');
			SetAnchors(alClient);
			SetCommand(cmAbout);
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
			SetHelpText('Use the "Find" edit field to enter text for searching.');
			AddHelpText('By default, pressing "Alt+F" will automatically focus this field.');
			SetBounds(B);
			SetAnchors(alRight or alBottom);
			SetRespond(cmSearch);
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
		Bounds(FBtnEdit^.Left + FBtnEdit^.Width - 21, FBtnEdit^.Top - 13, 22, 13, B);
		FEditMenu := New(PPopup, Create(PControl(FBtnEdit^.Parent), 'POPUP.EDITMENU'));
		with PPopUp(FEditMenu)^ do begin
			SetHelpText('A note regarding the edit button popup menu shortcut keys.');
			AddHelpText('They will only work when popup menu is visible.');
			SetShadow(False);
			SetBounds(B);
			SetAnchors(abBottom or abRight);
			Bounds(0,5,Width, 1, B);
			with PLine(New(PLine, Create(FEditMenu, 'LINE.UPPER')))^ do begin
				SetStyle(lnSingle or lnHorizontal);
				SetAllowedOutside(True);
				SetAnchors(abLeft or abRight or abTop);
				SetBounds(B);
			end;
			Bounds(0,9,Width, 1, B);
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
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select this option to add a program'
				+ ' to the current menu.');
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
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select this option to edit the '
				+ 'selected programs information or its execution process.');
				AddHelpText('The default keyboard shortcut is "E".');
				SetCaption('Edit Program...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmProgEdit);
				SetKeycode(kbE, 0, kfMatches);
			end;
			{TextAttr := $0C;}
			Inc(B.Top);
			FBtnPMove := New(PButton, Create(FEditMenu, 'BTN.PROG.MOVE'));
			with PButton(FBtnPMove)^ do begin
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select this option to move or copy'
				+ ' the currently selected program.');
				AddHelpText('The default keyboard shortcut is "X".');
				SetCaption('Move Program...');
				FHAlign := AlignLeft;
				SetBounds(B);
				SetCommand(cmProgMove);
				SetKeycode(kbX, 0, kfMatches);
			end;
			{TextAttr := $0C;}
			Inc(B.Top);
			FBtnPDel := New(PButton, Create(FEditMenu, 'BTN.PROG.DELETE'));
			with PButton(FBtnPDel)^ do begin
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select delete to remove the'
				+ ' currently selected program from the menu.');
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
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select this option to add a new menu.');
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
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select this option to edit the'
				+ ' current menu' + #$27 + 's information.');
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
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select this option to delete the'
				+ ' current menu.');
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
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select this option to modify'
				+ ' PGME' + #$27 + 's display options.');
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
				SetAnchors(abLeft or abRight or abTop);
				SetHelpText('5;"%0" Option: Select this option to change some'
				+ 'of PGME' + #$27 + 's behavior options.');
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
	FBtnPMove^.SetEnabled(FWritable and Assigned(FMenus^.FCurrent) and Assigned(FPrograms^.FCurrent));
	FBtnMAdd^.SetEnabled((Not FReadOnly));
	FBtnMEdit^.SetEnabled(FWritable and Assigned(FMenus^.FCurrent));
	FBtnMDel^.SetEnabled(FWritable and Assigned(FMenus^.FCurrent));
	FBtnPGMOpt^.SetEnabled(Not FReadOnly);
	FBtnPGMCfg^.SetEnabled(Not FReadOnly);
	GetEditMenu := FEditMenu;
end;

function TPGME.GetDlgPAdd : PControl;
begin
	if Not Assigned(FDlgPAdd) then begin
		FDlgPAdd := New(PDlgPAdd, Create(@Self));
		ApplySettings(FDlgPAdd);
	end;
	GetDlgPAdd := FDlgPAdd;
end;

function TPGME.GetDlgPEdit : PControl;
begin
	if Not Assigned(FDlgPEdit) then begin
		FDlgPEdit := New(PDlgPEdit, Create(@Self));
		ApplySettings(FDlgPEdit);
	end;
	GetDlgPEdit := FDlgPEdit;
end;

function TPGME.GetDlgPDelete : PControl;
begin
	if Not Assigned(FDlgPDel) then begin
		FDlgPDel := New(PDlgPDel, Create(@Self));
		ApplySettings(FDlgPDel);
	end;
	GetDlgPDelete := FDlgPDel;
end;

function TPGME.GetDlgMAdd : PControl;
begin
	if Not Assigned(FDlgMAdd) then begin
		FDlgMAdd := New(PDlgMAdd, Create(@Self));
		ApplySettings(FDlgMAdd);
	end;
	GetDlgMAdd := FDlgMAdd;
end;

function TPGME.GetDlgMEdit : PControl;
begin
	if Not Assigned(FDlgMEdit) then begin
		FDlgMEdit := New(PDlgMEdit, Create(@Self));
		ApplySettings(FDlgMEdit);
	end;
	GetDlgMEdit := FDlgMEdit;
end;

function TPGME.GetDlgMDelete : PControl;
begin
	if Not Assigned(FDlgMDel) then begin
		FDlgMDel := New(PDlgMDel, Create(@Self));
		ApplySettings(FDlgMDel);
	end;
	GetDlgMDelete := FDlgMDel;
end;

function TPGME.GetDlgPGMOpt : PControl;
begin
	if Not Assigned(FDlgPGMOpt) then begin
		FDlgPGMOpt := New(PDlgPGMOpt, Create(@Self));
		ApplySettings(FDlgPGMOpt);
	end;
	GetDlgPGMOpt := FDlgPGMOpt;
end;

function TPGME.GetDlgPGMCfg : PControl;
begin
	if Not Assigned(FDlgPGMCfg) then begin
		FDlgPGMCfg := New(PDlgPGMCfg, Create(@Self));
		ApplySettings(FDlgPGMCfg);
	end;
	GetDlgPGMCfg := FDlgPGMCfg;
end;

function TPGME.GetDlgLaunch : PControl;
begin
	if Not Assigned(FDlgLaunch) then begin
		FDlgLaunch := New(PDlgLaunch, Create(@Self, 'DIALOG.LAUNCH'));
		ApplySettings(FDlgLaunch);
	end;
	GetDlgLaunch := FDlgLaunch;
end;

function TPGME.GetDlgFindExec : PControl;
var
	S : String;
	P : PListItem;
begin
	if Not Assigned(FDlgFindExec) then begin
		FDlgFindExec := New(POpenDialog, Create(@Self, 'DIALOG.FINDEXEC'));
		with POpenDialog(FDlgFindExec)^ do begin
			SetHelpText('This is a standard "Select File" dialog box.');
			AddHelpText('');
			AddHelpText('You may use the dropdown path list to select one of the systems current');
			AddHelpText('active directories. Or, you may type in one for direct access. Also,' );
			AddHelpText('recently entered paths are stored into the list for quick selection.');
			AddHelpText('');
			AddHelpText('The filter list can be used filter the directory listings by file types.' );
			AddHelpText('They will be sorted in the order listed. Also, if a filename is entered manually' );
			AddHelpText('that contains wildcards, it will be added as a user filter.');
			AddHelpText('');
			FAutoEndModal := True;
			SetTitle('Select Program');
			FBOK.SetCaption('OK');
			FBMKDir.SetVisible(False);
			AddFilter('Executable (*.EXE;*.COM;*.BAT)');
			AddFilter('Screen Saver (*.SCR)');
			FConfig.OpenSection(Self.GetPathID +'.VIEWERS');
			S := '';
			if Assigned(FConfig.FSectionKey) then begin
				P := FConfig.FSectionKey^.First;
				while Assigned(P) do begin
					if S <> '' then S := S + ';';
					S := S + '*.' + P^.GetNameID;
					P := P^.Next;
				end;
			end;
			if S <> '' then
				AddFilter('Viewable (' + S + ')');
			AddFilter('All Files (*.*)');
		end;
		ApplySettings(FDlgFindExec);

	end;
	GetDlgFindExec := FDlgFindExec;
end;

function TPGME.GetDlgOptExec : PControl;
begin
	if Not Assigned(FDlgOptExec) then begin
		FDlgOptExec := New(PDlgExecOpt, Create(@Self, 'DIALOG.OPTEXEC'));
		ApplySettings(FDlgOptExec);

	end;
	GetDlgOptExec := FDlgOptExec;
end;

function TPGME.GetDlgAbout : PControl;
begin
	if Not Assigned(FDlgAbout) then begin
		FDlgAbout := New(PDlgAbout, Create(@Self));
		ApplySettings(FDlgAbout);
	end;
	GetDlgAbout := FDlgAbout;
end;

function TPGME.GetDlgMove : PControl;
begin
	if Not Assigned(FDlgMove) then begin
		FDlgMove := New(PDlgMove, Create(@Self));
		ApplySettings(FDlgMove);
	end;

	GetDlgMove := FDlgMove;
end;

function TPGME.GetDlgDosCommand : PControl;
begin
	if Not Assigned(FDlgDosCommand) then begin
		FDlgDosCommand := New(PDlgDosCommand, Create(@Self));
		ApplySettings(FDlgDosCommand);
	end;
	GetDlgDosCommand := FDlgDosCommand;
end;

function TPGME.GetChangeLang;
var
	B : TBounds;
	FD : array[0..1] of TFormatData;
begin
	FillChar(FD, Sizeof(FD), 0);
	FD[0].ID := FmtString;
	FD[0].StringValue := ATheme;
	FD[1].ID := FmtString;
	FD[1].StringValue := ALang;
	if Not Assigned(FDlgChangeLang) then begin
		TextAttr := $1F;
		FDlgChangeLang := New(PMessageBox, Create(@Self, 'DIALOG.LANGTOO'));
		with PMessageBox(FDlgChangeLang)^ do begin
			SetBackground(1);
			SetTextAttr($1F);
			Bounds(1,1,68,11, B);
			SetBounds(B);
			FAutoEndModal := True;
			SetTitle('Hmmm. One more thing?');
			SetMessage(' ');
			AppendMessage('');
			AppendMessage('Hey, about that theme you want to apply. It would really');
			AppendMessage('appreciate a different language file.');
			AppendMessage(' ');
			AppendMessage('1;So, do you want to switch to the %0 language?');
			SetHelpText('The change language too "One more thing" question dialog box.');
			AddHelpText('');
			AddHelpText('The requested theme that is going to be applied highly suggests');
			AddHelpText('using a different language than the one you are currently using.');
			Bounds(B.Width div 2 - 14, 7, 11, 1, B);
			TextAttr := $4E;
			with PButton(New(PButton, Create(FDlgChangeLang, 'BUTTON.NO')))^ do begin
				SetBounds(B);
				SetAnchors(abRight or abBottom);
				SetCaption('Nope!');
				SetCommand(cmCancel);
			end;
			Inc(B.Left, 14);
			TextAttr := $2E;
			with PButton(New(PButton, Create(FDlgChangeLang, 'BUTTON.YES')))^ do begin
				SetBounds(B);
				SetAnchors(abLeft or abBottom);
				SetCaption('Yes');
				SetCommand(cmOK);
			end;
		end;
		ApplySettings(FDlgChangeLang);
	end;
	if Assigned(FDlgChangeLang) then
		PMessageBox(FDlgChangeLang)^.SetFormatData(FD, 2);
	GetChangeLang := FDlgChangeLang;
end;

function TPGME.GetChangeTheme;
var
	B : TBounds;
	FD : array[0..1] of TFormatData;
begin
	FillChar(FD, Sizeof(FD), 0);
	FD[0].ID := FmtString;
	FD[0].StringValue := ATheme;
	FD[1].ID := FmtString;
	FD[1].StringValue := ALang;
	if Not Assigned(FDlgChangeTheme) then begin
		TextAttr := $1F;
		FDlgChangeTheme := New(PMessageBox, Create(@Self, 'DIALOG.THEMETOO'));
		with PMessageBox(FDlgChangeTheme)^ do begin
			SetBackground(1);
			SetTextAttr($1F);
			Bounds(1,1,68,11, B);
			SetBounds(B);
			FAutoEndModal := True;
			SetTitle('Hmmm. One more thing?');
			SetMessage(' ');
			AppendMessage('');
			AppendMessage('Hey, about that language you want to apply. It really');
			AppendMessage('wants to use a different theme file.');
			AppendMessage(' ');
			AppendMessage('0;So, do you want to switch to the %0 theme?');
			SetHelpText('The change theme too "One more thing" question dialog box.');
			AddHelpText('');
			AddHelpText('The requested language that is going to be applied highly suggests');
			AddHelpText('using a different theme than the one you are currently using.');
			Bounds(B.Width div 2 - 14, 7, 11, 1, B);
			TextAttr := $4E;
			with PButton(New(PButton, Create(FDlgChangeTheme, 'BUTTON.NO')))^ do begin
				SetBounds(B);
				SetAnchors(abRight or abBottom);
				SetCaption('Nope!');
				SetCommand(cmCancel);
			end;
			Inc(B.Left, 14);
			TextAttr := $2E;
			with PButton(New(PButton, Create(FDlgChangeTheme, 'BUTTON.YES')))^ do begin
				SetBounds(B);
				SetAnchors(abLeft or abBottom);
				SetCaption('Yes');
				SetCommand(cmOK);
			end;
		end;
		ApplySettings(FDlgChangeTheme);
	end;
	if Assigned(FDlgChangeTheme) then
		PMessageBox(FDlgChangeTheme)^.SetFormatData(FD, 2);
	GetChangeTheme := FDlgChangeTheme;
end;

procedure TPGME.CreateAllDialogs;
begin
	inherited CreateAllDialogs(ConserveMemory);
	GetDlgAbout;
	if ConserveMemory then FreeAllDialogs;
	GetDlgDosCommand;
	if ConserveMemory then FreeAllDialogs;
	GetEditMenu;
	if ConserveMemory then FreeAllDialogs;
	GetNoTempDir;
	if ConserveMemory then FreeAllDialogs;
	GetNoBatchLoop;
	if ConserveMemory then FreeAllDialogs;
	GetGornError;
	if ConserveMemory then FreeAllDialogs;
	GetExecMissing;
	if ConserveMemory then FreeAllDialogs;
	GetDlgPAdd;
	if ConserveMemory then FreeAllDialogs;
	GetDlgPEdit;
	if ConserveMemory then FreeAllDialogs;
	GetDlgPDelete;
	if ConserveMemory then FreeAllDialogs;
	GetDlgMAdd;
	if ConserveMemory then FreeAllDialogs;
	GetDlgMEdit;
	if ConserveMemory then FreeAllDialogs;
	GetDlgMDelete;
	if ConserveMemory then FreeAllDialogs;
	GetDlgPGMOpt;
	if ConserveMemory then FreeAllDialogs;
	GetDlgPGMCfg;
	if ConserveMemory then FreeAllDialogs;
	GetDlgLaunch;
	if ConserveMemory then FreeAllDialogs;
	GetDlgFindExec;
	if ConserveMemory then FreeAllDialogs;
	GetDlgOptExec;
	if ConserveMemory then FreeAllDialogs;
	GetDlgMove;
	if ConserveMemory then FreeAllDialogs;
	GetChangeLang('','');
	if ConserveMemory then FreeAllDialogs;
	GetChangeTheme('','');
	if ConserveMemory then FreeAllDialogs;
end;

procedure TPGME.ParseCommandLine;
var
	I : integer;
	O, V : String;
begin
    FPauseText := '';
	FResumeText := '';
	FReqSearch := '';
	for I := 1 to ParamCount do begin
		V := Trim(ParamStr(I));
		if FResumeText <> '' then
			FResumeText := FResumeText + ' ' + V
		else
		if (Length(V) > 1) and (V[1] = '/') then begin
			O := UCase(Trim(PullStr('=', V)));
			V := Trim(V);
			if O = '/BAT' then
				FBatLoop := True
			else if O = '/INIT' then
				FInitRun := True
			else if O = '/M' then
				FReqMenu := UCase(V)
			else if O = '/P' then
				FReqProg := UCase(V)
			else if O = '/T' then
				FResumeText := V
			else if O = '/ERROR' then
				FRetError := StrInt(V)
			else if O = '/W' then begin
				FRetWait := StrInt(V);
			end;
		end else
			FReqSearch := Trim(FReqSearch + ' ' + V);
	end;
{	FBatLoop := True; }
	FInSound := True;
	if (FReqMenu <> '') and (FReqProg <> '') then
		FInitRun := False;
end;

procedure TPGME.SelectMenu(AValue : String);
var
	T, P :PListItem;
	S : String;
begin
	if Not (Assigned(FMenus) and Assigned(FMenuCache)) then exit;
	P := FMenus^.FItems.First;
	while Assigned(P) do begin
		S := UCase(PCacheDir(PCacheMenu(P^.Data)^.Parent)^.GetNameID +
			PCacheMenu(P^.Data)^.GetNameID);
		if S = AValue then begin
			FMenus^.SetIndex(P^.GetIndex);
			OpenCurrentMenu;
			P := nil;
		end else
			P := P^.Next;
	end;
end;

procedure TPGME.SelectProg(AValue : String);
var
	T, P :PListItem;
	S : String;
begin
	if Not (Assigned(FPrograms) and Assigned(FMenuCache)) then exit;
	P := FPrograms^.FItems.First;
	while Assigned(P) do begin
		S := PCacheApp(P^.Data)^.GetNameID;
		if S = AValue then begin
			FPrograms^.SetIndex(P^.GetIndex);
			P := nil;
		end else
			P := P^.Next;
	end;
end;

procedure TPGME.BeforeShow;
begin
	inherited BeforeShow;
	GotoXY(1, WhereY);
	TextAttr := $07;
	ClrEOL;
	if FReqMenu <> '' then SelectMenu(FReqMenu);
	if FReqProg <> '' then SelectProg(FReqProg);
	if FReqSearch <> '' then Search(FReqSearch);
	UpdateButtons;
end;

procedure TPGME.Configure;
begin
	if (Not FInitRun) then begin
		GotoXY(1, WhereY);
		TextAttr := $0B;
		if FResumeText = '' then
			Write('Resuming Eternity...')
		else
			Write(FResumeText);
		ClrEol;
	end;

	inherited Configure;
	PMenuCache(FMenuCache)^.ApplyLanguage(@FLanguage, GetPathID);
	FEditTitle^.SetCaption(PMenuCache(FMenuCache)^.FNoMenuStr); {Set Initially to No Title}
	FPauseText := FLanguage.GetValue(GetPathID + '.BOOT.PAUSE', 'press any key to continue . . .');
	if FInitRun then begin
		GotoXY(1, WhereY);
		TextAttr := $0E;
		Write(FLanguage.GetValue(GetPathID + '.BOOT.CACHE.UPDATE', 'Updating Menu Cache...'));
		ClrEol;
	end;
	FConfig.OpenSection(GetPathID);
	FResumeText := FLanguage.GetValue(GetPathID + '.BOOT.RESUME', 'Resuming Eternity...');
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
	if CPUCacheProg = '' then begin
    	AConfig^.OpenSection('THROTTLE');
    	CPUCacheProg := AConfig^.GetValue('CPUCACHE', 'CPUCACH.COM');
    	if CPUCacheProg <> '' then begin
    	    if FileExists(FileExpand(CPUCacheProg)) then
    	        CPUCacheProg := FileExpand(CPUCacheProg)
    	    else
    	        CPUCacheProg := SearchPathSPec(CPUCacheProg);
    	end;
    end;
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

procedure TPGME.AtStartup;
var
    W : LongInt;
begin
    inherited AtStartUp;
	if (FRetWait = 0) then Exit;
	HideCursor;
    GotoXY(1, WhereY);
    ClrEOL;
    GotoXY(1, GetMaxY);
    TextAttr := $1E;
    Write(' ', FPauseText);
    ClrEol;
    W := FRetWait;
    if W > 0  then
        ScheduleCommand(cmTimer, nil, W * 1000);
    EventPause;
    ClearTimers;
    PurgeEvents;
    GotoXY(1, WhereY);
    TextAttr := $07;
    ClrEol;
	{ SetCursor(PrevCursor); }
end;

procedure TPGME.Idle;
const
	I : word = 100;
	S : word = 100;
	L : longInt = 0;
	F : boolean = false;
var
	Ticks : LongInt absolute $0040:$006c;
	Hour, Min, Sec, Sec100 : word;
begin
	inherited Idle;
	if Assigned(FMenus) then begin
		if (FMenus^.FCurrent <> FLastMenu) and (not F) then begin
			F := True;
			PutCommand(cmNone, nil);
		end else
			F := False;
	end;
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

	if S = Sec then Exit;
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
	if Assigned(FDlgChangeLang) and FDlgChangeLang^.GetVisible then Exit;
	if Assigned(FDlgChangeTheme) and FDlgChangeTheme^.GetVisible then Exit;
	if Assigned(FDlgAbout) and FDlgAbout^.GetVisible then Exit;
	if Assigned(FDlgMove) and FDlgMove^.GetVisible then Exit;

    FTime^.Update;
    FDate^.Update;
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
	if Assigned(FMenus^.FCurrent) and Assigned(FMenus^.FCurrent^.Data) then begin
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
	end else
		FEditTitle^.SetCaption('');
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

procedure TPGME.DeleteMenu;
var
	F : File;
	H, P : PListItem;
begin
	if Not Assigned(FMenus^.FCurrent) then exit;
	H := FMenus^.FCurrent;
	P := PListItem(H^.Data);

	Assign(F, PCacheDir(P^.Parent)^.GetNameID + PCacheMenu(P)^.GetNameID);
	Erase(F);
	if IOResult <> 0 then exit; { Should show an error here! }

	{ Only removing for searching, cache will update itself on next launch }
	PListItem(P^.Parent)^.Remove(P);
	Dispose(P, Destroy);

	StartDrawing;
	FMenus^.DeleteItem(FMenus^.FCurrent);
	FinishDrawing;
end;

procedure TPGME.DeleteProgram;
var
	H, P : PListItem;
	M : PConfigFile;
begin
	if Not Assigned(FPrograms^.FCurrent) then exit;
	if Not MemCheck(Sizeof(TConfigFile)) then Halt(8);
	H := FPrograms^.FCurrent;
	P := PListItem(H^.Data);
	M := New(PConfigFile, Create('ROOT'));
	with PCacheApp(FPrograms^.FCurrent^.Data)^ do begin
		M^.SetFileName(PCacheDir(Parent^.Parent)^.GetNameID +
			PCacheMenu(Parent)^.GetNameID);
		M^.Read;
		M^.OpenSection(GetNameID);
		M^.DeleteSection;
		M^.Write;
	end;
	Dispose(M, Destroy);

	{ Only removing for searching, cache will update itself on next launch }
	PListItem(P^.Parent)^.Remove(P);
	Dispose(P, Destroy);

	StartDrawing;
	FPrograms^.DeleteItem(FPrograms^.FCurrent);
	FinishDrawing;
end;

procedure TPGME.DeleteCache;
var
	F : File;
begin
	Assign(F, GetExePath + 'ETERNITY.DAT');
	Erase(F);
	if IOResult <> 0 then exit;
end;

function TPGME.GetReturnStr(CanWait : Boolean) : String;
var
    W : Longint;
	S : String;
begin
	S := '';
	W := 0;
	GetReturnStr := S;
	if not Assigned(FMenuCache) then exit;
	if Assigned(FPrograms) and Assigned(FPrograms^.FCurrent) then begin
		with PCacheApp(FPrograms^.FCurrent^.Data)^ do begin
			S := '/M=' + PCacheDir(Parent^.Parent)^.GetNameID +
				PCacheMenu(Parent)^.GetNameID + ' /P=' + GetNameID;
			W := ReturnWait;
			if W = -2 then W := PCacheMenu(Parent)^.DefaultWait;
		end;
	end else if Assigned(FMenus) and Assigned(FMenus^.FCurrent) then begin
		with PCacheMenu(FMenus^.FCurrent^.Data)^ do begin
			S := '/M=' + PCacheDir(Parent)^.GetNameID + GetNameID;
			W := DefaultWait;
		end;
	end;
	FLanguage.OpenSection('STRINGS');
	FResumeText := FLanguage.GetValue(GetNameID + '.BOOT.RESUME', 'Resuming Eternity...');
	if CanWait and (W <> 0) then
	    S := S + ' /W=' + IntStr(W);
	S := S + ' /T=' + FResumeText;
	GetReturnStr := S;
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
			FExt := FileExtension(FName);

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

	procedure FileWriteExec(Command : String);
	var
		TDir : String;
	begin
		FName := PullStr(' ', Command);
		TDir := PathOf(FName);
		if (TDir <> '') and (TDir <> FDir) then begin
			if TDir[1] <> FDir[1] then
				FileWrite (Copy(TDir, 1,2));
			FileWrite('CD ' + Copy(TDir, 3, Length(TDir)));
			FName := Copy(FName, Length(TDir) + 1, Length(FName));
		end;
		if (Length(FName) = 2) and (FName[2] = ':') then
			FDir := FName;
		if FName = 'CD' then
			FDir := Command;
		FExt := FileExtension(FName);
		FView := FConfig.GetValue(FExt, '');
		if FView <> '' then begin
			Command := Trim(FName + ' ' + Command);
			FName := FileExpand(FView);
		end;
		FExt := FileExtension(FName);
		if FExt = 'BAT' then begin
			Command := Trim(FName + ' ' + Command);
			FName := 'CALL';
		end;
		FileWrite(Trim(FName + ' ' + Command));
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
		ChDir(NoTailDelim(FDir));
		FRet := GetReturnStr(True);
		if FRet = '' then
			FRet := '/M=' + FDir + PCacheMenu(Parent)^.GetNameID +
			' /P=' + GetNameID + ' /T=' + FResumeText;
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
		ChDir(NoTailDelim(FHoldDir));

		if SimpleExec and (not MaxMemory) and (Count = 0) and (FName <> '') and
		(not PGMVars) then begin
			if PathOf(FName) <> FDir then
				FExecDir := PathOf(FName);
			if FView <> '' then begin
				FView := FileExpand(FView);
				S := FOpts;
				FExecDir := PathOf(FileExpand(PullStr(' ', S)));
			end;
			if FExecDir <> '' then
				FOriginalPath := FExecDir;
			if Tone <> '' then
				FEndTone := Tone;
			ChDir(NoTailDelim(FExecDir));
			SetCurDisk(Ord(FExecDir[1]) - 64);
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

			FileWrite(S2 + 'PGME_PATH=' + GetExePath);
			FileWrite(S2 + 'PGME_MENU=' + FDir + PCacheMenu(Parent)^.GetNameID );
			FileWrite(S2 + 'PGME_PROG=' + GetNameID);
			FileWrite('');
			FileWrite('REM Begin by setting current dir and drive to menu directory.');
			FileWrite(Copy(FDir, 1, 2));
			FileWrite('CD ' + NoTailDelim(Copy(FDir, 3, Length(FDir))));
			FileWrite('');
			FileWrite('REM Continue by running complex launching processes.');

			FExt := UCase(Copy(FName, LastPos('.', FName) + 1, Length(FName) ));
			FView := FConfig.GetValue(FExt, '');

			P := PCacheProc(First);
			if OK then begin
				if Not assigned(P) then begin
					FileWriteExec(FName + ' ' + FOpts);
				end else while Assigned(P) and OK do begin
					if (Length(P^.GetNameID) > 4) and (UCase(Copy(P^.GetNameID, 1, 4)) = 'CD \') then
						FileWriteExec(NoTailDelim(P^.GetNameID))
					else
						FileWriteExec(P^.GetNameID);
					P := PCacheProc(P^.Next);
				end;
			end;

			if PGMVars then begin
				FileWrite('');
				FileWrite('REM Clear PGME Environment Variables');
				FileWrite(S2 + 'PGME_PATH=');
				FileWrite(S2 + 'PGME_MENU=');
				FileWrite(S2 + 'PGME_PROG=');
			end;

			FileWrite('');
			{FileWrite('PAUSE');}
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
			CallBack('', '', '', False); { if can quit is false, was set to return }
		end;
	end;
end;

procedure TPGME.Restart;
begin
	FTheme.OpenSection(GetNameID);
	FEndTone := FTheme.GetValue('TONE.RESTART', FEndTone);
	CallBack('', '', GetReturnStr(False), True);
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
		CallBack(S, '', GetReturnStr(False), True);
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

procedure TPGME.CheckLanguage;
var
	C : TConfigFile;
	L, S : String;
begin
	C.Create('TEMP');
	C.SetFileName(ATheme);
	C.Read;
	C.OpenSection('*');
	S := UCase(Trim(C.GetValue('LANGUAGE', '')));
	C.Destroy;
	FConfig.OpenSection('*');
	L := UCase(Trim(FConfig.GetValue('LANGUAGE', '')));
	if (S <> '') and (S <> L) and (FileExists(S)) then begin
		GetChangeLang(ATheme, S);
		if FDlgChangeLang^.ShowModal = mrOK then begin
			FConfig.OpenSection('*');
			FConfig.SetValue('LANGUAGE', S);
			DeleteCache;
			{ needed to update the "Resuming Eternity" message at restart to
			  the new language on very first restart. }
			FLanguage.SetFileName(S);
			FLanguage.Read;
		end;
	end;
end;

procedure TPGME.CheckTheme;
var
	C : TConfigFile;
	L, S : String;
begin
	C.Create('TEMP');
	C.SetFileName(ALanguage);
	C.Read;
	C.OpenSection('*');
	S := UCase(Trim(C.GetValue('THEME', '')));
	C.Destroy;
	FConfig.OpenSection('*');
	L := UCase(Trim(FConfig.GetValue('THEME', '')));
	if (S <> '') and (S <> L) and (FileExists(S)) then begin
		GetChangeTheme(S, ALanguage);
		if FDlgChangeTheme^.ShowModal = mrOK then begin
			FConfig.OpenSection('*');
			FConfig.SetValue('THEME', S);
		end;
	end;
end;

procedure TPGME.AddMenu;
var
	M : TConfigFile;
	CMenu, S : String;
begin
	if not (Assigned(FMenus) and
	Assigned(FDlgMAdd) and Assigned(PDlgMAdd(FDlgMAdd)^.FLabelFile)) then exit;
	M.Create('TEMP');
	CMenu := PDlgMAdd(FDlgMAdd)^.FLabelFile^.GetCaption;
	M.SetFileName(CMenu);
	M.OpenSection('*');
	M.SetValue('TITLE.' + Application^.FLanguageID, PDlgMAdd(FDlgMAdd)^.FEditShort^.GetCaption );
	if Trim(M.GetValue('TITLE', '')) = '' then
		M.SetValue('TITLE', PDlgMAdd(FDlgMAdd)^.FEditShort^.GetCaption );
	M.SetValue('DESCRIPTION.' + Application^.FLanguageID, PDlgMAdd(FDlgMAdd)^.FEditLong^.GetCaption );
	if Trim(M.GetValue('DESCRIPTION', '')) = '' then
		M.SetValue('DESCRIPTION', PDlgMAdd(FDlgMAdd)^.FEditLong^.GetCaption );

	S := CMenu;
	Delete(S, 1, LastPos('\', S));
	S := Copy(S, 1, Pos('.', S) - 1);

	M.SetValue('ID', S);
	M.SetValue('WRITABLE', 'yes');
	M.SetValue('TYPE', 'MENU');
	M.SetValue('MERGE', '');

	M.Write;
	M.Destroy;
	StartDrawing;
	UpdateMenuCache;
	SelectMenu(CMenu);
	FMenus^.Update;
	FinishDrawing;
end;

procedure TPGME.ModifyMenu;
var
	M : TConfigFile;
	CMenu, CProg : String;
begin
	if not (Assigned(FMenus) and Assigned(FMenus^.FCurrent) and
	Assigned(FDlgMEdit)) then exit;
	M.Create('TEMP');
	CMenu := '';
	CProg := '';
	if Assigned(FPrograms) and Assigned(FPrograms^.FCurrent) then
		with PCacheApp(FPrograms^.FCurrent^.Data)^ do
			CProg := GetNameID;
	with PCacheMenu(FMenus^.FCurrent^.Data)^ do
		CMenu := PCacheDir(Parent)^.GetNameID + GetNameID;
	M.SetFileName(CMenu);
	M.Read;
	M.OpenSection('*');
	M.SetValue('TITLE.' + Application^.FLanguageID, PDlgMEdit(FDlgMEdit)^.FEditShort^.GetCaption );
	if Trim(M.GetValue('TITLE', '')) = '' then
		M.SetValue('TITLE', PDlgMEdit(FDlgMEdit)^.FEditShort^.GetCaption );
	M.SetValue('DESCRIPTION.' + Application^.FLanguageID, PDlgMEdit(FDlgMEdit)^.FEditLong^.GetCaption );
	if Trim(M.GetValue('DESCRIPTION', '')) = '' then
		M.SetValue('DESCRIPTION', PDlgMEdit(FDlgMEdit)^.FEditLong^.GetCaption );
	if Assigned(PDlgMEdit(FDlgMEdit)^.FLockToggle) then
		if PDlgMEdit(FDlgMEdit)^.FLockToggle^.GetOn then
			if PDlgMEdit(FDlgMEdit)^.FLockAccept^.GetState = 1 then
				M.SetValue('WRITABLE', 'NO');
	M.Write;
	M.Destroy;
	StartDrawing;
	UpdateMenuCache;
	SelectMenu(CMenu);
	SelectProg(CProg);
	FMenus^.Update;
	FinishDrawing;
end;

procedure TPGME.UpdateProgram(AMenu : TConfigFile; ADlg : PDialog);
var
	P, K : PListItem;
begin
	AMenu.SetValue('TITLE.' + Application^.FLanguageID,
		PDlgProg(ADlg)^.FEditTitle^.GetCaption );
	AMenu.SetValue('FILE', PDlgPAdd(ADlg)^.FFile);

	if Trim(AMenu.GetValue('TITLE', '')) = '' then
		AMenu.SetValue('TITLE', PDlgProg(ADlg)^.FEditTitle^.GetCaption );
	if StrBool(AMenu.GetValue('MAXAVAIL', 'False')) <> PDlgProg(ADlg)^.FMaxAvail^.GetOn then
		AMenu.SetValue('MAXAVAIL', BoolStr(PDlgProg(ADlg)^.FMaxAvail^.GetOn));
	if StrBool(AMenu.GetValue('RETURN', 'True')) <> PDlgProg(ADlg)^.FReturn^.GetOn then
		AMenu.SetValue('RETURN', BoolStr(PDlgProg(ADlg)^.FReturn^.GetOn));
	if StrBool(AMenu.GetValue('CPUCACHE', 'False')) <> PDlgProg(ADlg)^.FCPUCache^.GetOn then
		AMenu.SetValue('CPUCACHE', BoolStr(PDlgProg(ADlg)^.FCPUCache^.GetOn));
	if StrBool(AMenu.GetValue('SIMPLE', 'True')) <> PDlgProg(ADlg)^.FSimple then
		AMenu.SetValue('SIMPLE', BoolStr(PDlgProg(ADlg)^.FSimple));
	if StrBool(AMenu.GetValue('PGMVARS', 'False')) <> PDlgProg(ADlg)^.FPGMVars then
		AMenu.SetValue('PGMVARS', BoolStr(PDlgProg(ADlg)^.FPGMVars));
	if AMenu.GetValue('CLASS', '') = '' then
		AMenu.SetValue('CLASS', PDlgPAdd(ADlg)^.FClass);
	if PDlgProg(ADlg)^.FCommands.Count > 0 then begin
		AMenu.SetValue('COMMANDS', '');
		K := AMenu.FindKey('COMMANDS');
		P := PDlgProg(ADlg)^.FCommands.First;
		while Assigned(P) and Assigned(K) do begin
			K^.Add(New(PListItem, Create(P^.GetNameID)));
			P := P^.Next;
		end;
	end else
		AMenu.DeleteKey('COMMANDS');
	if Assigned(PDlgProg(ADlg)^.FEditTones) then
		AMenu.SetValue('TONE', PDlgProg(ADlg)^.FEditTones^.GetCaption);
end;

procedure TPGME.AddProgram;
var
	M : TConfigFile;
	CMenu, CProg : String;
	S : String;
	I : Integer;
begin
	if not Assigned(FMenuCache) then exit;
	if not (Assigned(FMenus) and Assigned(FMenus^.FCurrent)) then exit;
	with PCacheMenu(FMenus^.FCurrent^.Data)^ do begin
		CMenu := PCacheDir(Parent)^.GetNameID + GetNameID;
	end;
	M.Create('TEMP');
	M.SetFileName(CMenu);
	M.Read;
	CProg := ReduceStr(	OnlyAlphaNum(PDlgPAdd(FDlgPAdd)^.FEditTitle^.GetCaption), 10);
	if M.FList.FindID(CProg) <> nil then begin
		I := 0;
		while M.FList.FindID(CProg + '-' + HexStr(I)) <> nil do Inc(I);
		CProg := CProg + '-' + HexStr(I);
	end;
	M.OpenSection(CProg);
	S := CMenu;
	Delete(S, 1, LastPos('\', S));
	S := Copy(S, 1, Pos('.', S) - 1);
	M.SetValue('GROUP', S);
	UpdateProgram(M, FDlgPAdd);
	M.Write;
	M.Destroy;
	StartDrawing;
	UpdateMenuCache;
	SelectMenu(CMenu);
	SelectProg(CProg);
	FMenus^.Update;
	FinishDrawing;
end;

procedure TPGME.ModifyProgram;
var
	M : TConfigFile;
	CMenu, CProg : String;
begin
	if not Assigned(FMenuCache) then exit;
	if not (Assigned(FMenus) and Assigned(FMenus^.FCurrent) and
	Assigned(FPrograms) and Assigned(FPrograms^.FCurrent)) then exit;
	with PCacheApp(FPrograms^.FCurrent^.Data)^ do begin
		CMenu := PCacheDir(Parent^.Parent)^.GetNameID + PCacheMenu(Parent)^.GetNameID;
		CProg := GetNameID;
	end;
	M.Create('TEMP');
	M.SetFileName(CMenu);
	M.Read;
	M.OpenSection(CProg);
	UpdateProgram(M, FDlgPEdit);
	M.Write;
	M.Destroy;
	StartDrawing;
	UpdateMenuCache;
	SelectMenu(CMenu);
	SelectProg(CProg);
	FMenus^.Update;
	FinishDrawing;
end;

procedure TPGME.CopyOrMove(AMove : Boolean);
var
	I : Integer;
	M : TConfigFile;
	Hold , P : PListItem;
	OMenu, NMenu,
	OProg, NProg,
	Title : String;
begin
	if not Assigned(FMenuCache) then exit;
	if not (Assigned(FMenus) and Assigned(FMenus^.FCurrent) and
	Assigned(FPrograms) and Assigned(FPrograms^.FCurrent)) then exit;
	with PCacheApp(FPrograms^.FCurrent^.Data)^ do begin
		OMenu := PCacheDir(Parent^.Parent)^.GetNameID + PCacheMenu(Parent)^.GetNameID;
		OProg := GetNameID;
		NMenu := PCacheDir(PCacheMenu(PDlgMove(FDlgMove)^.Destination)^.Parent)^.GetNameID +
			PCacheMenu(PDlgMove(FDlgMove)^.Destination)^.GetNameID;
		NProg := OProg;
	end;
	{ Read Source Item }
	M.Create('TEMP');
	M.SetFileName(OMenu);
	M.Read;
	M.OpenSection(OProg);
	Hold := M.FSectionKey;
	M.FList.Remove(Hold);

	{ Write to Destination }
	M.SetFileName(NMenu);
	M.Read;
	Title := '';
	if Assigned(Hold) then begin
		P := Hold^.FindID('TITLE');
		if Assigned(P) and Assigned(P^.First) then
			Title := Trim(P^.First^.GetNameID);
	end;
	if Title = '' then
		Title := 'FAILED TO LOCATE TITLE';

	NProg := ReduceStr(OnlyAlphaNum(Title), 10);
	if M.FList.FindID(NProg) <> nil then begin
		I := 0;
		while M.FList.FindID(NProg + '-' + HexStr(I)) <> nil do Inc(I);
		NProg := NProg + '-' + HexStr(I);
	end;

	M.OpenSection(NProg);
	while Assigned(Hold^.First) do begin
		P := Hold^.First;
		Hold^.Remove(P);
		M.FSectionKey^.Add(P);
	end;
	M.Write;

	{ free left over bits }
	Dispose(Hold, Destroy);

	{ Dete Source Item }
	if AMove then begin
		M.SetFileName(OMenu);
		M.Read;
		M.OpenSection(OProg);
		M.DeleteSection;
		M.Write;
	end;
	M.Destroy;
	StartDrawing;
	UpdateMenuCache;
	SelectMenu(NMenu);
	SelectProg(NProg);
	FMenus^.Update;
	FinishDrawing;
end;

function TPGME.SearchStr(AValue :String) : String;
begin
	AValue := FilterChar(AValue, ' ');
	SearchStr := UCase(AValue);
end;

procedure TPGME.Search(AValue : String);
var
	CM, FM : PListItem;
	CP, FP : PCacheApp;

	function CompareStr(AStr : String) : boolean;
	begin
		CompareStr := False;
		AStr := SearchStr(AStr);
		if Pos(AValue, AStr) > 0 then begin
			StartDrawing;
			SelectMenu(PCacheDir(PCacheMenu(CM^.Data)^.Parent)^.GetNameID +
				PCacheMenu(CM^.Data)^.GetNameID);
			if Assigned(CP) then
				SelectProg(CP^.GetNameID);
			FinishDrawing;
			CompareStr := True;
		end;
	end;

begin
	AValue := SearchStr(AValue);
	if Not Assigned(FMenus^.FCurrent) then Exit;

	FM := FMenus^.FCurrent;
	FP := nil;
	if Assigned(FPrograms^.FCurrent) then
		FP := PCacheApp(FPrograms^.FCurrent^.Data);
	CM := FM;
	CP := FP;

	repeat
		if Assigned(CP) then
			CP := PCacheApp(CP^.Next)
		else
			CP := PCacheApp(PCacheMenu(CM^.Data)^.First);
		if Assigned(CP) then begin
			if CompareStr(CP^.GetNameID) then Exit;
			if CompareStr(CP^.Title) then Exit;
			if CompareStr(CP^.GroupName) then Exit;
		end else begin
			CM := CM^.Next;
			if not Assigned(CM) then
				CM := FMenus^.FItems.First;
			if CompareStr(PCacheMenu(CM^.Data)^.GetNameID) then Exit;
			if CompareStr(PCacheMenu(CM^.Data)^.ShortTitle) then Exit;
			if CompareStr(PCacheMenu(CM^.Data)^.LongTitle) then Exit;
		end;
	until (CM = FM) and (CP = FP);
end;

procedure TPGME.CheckLocal(var AEvent : TEvent);
var
	R : integer;
	S : String;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmListViewDouble : if AEvent.InfoPtr = FPrograms then begin
				ClearEvent(AEvent);
				PutCommand(cmExecute, @Self);
			end;
			cmEdit : if AEvent.InfoPtr = FFind.FEdit then begin
				ClearEvent(AEvent);
				Search(Trim(FFind.FEdit^.GetCaption));
			end;
			cmFind : begin
				ClearEvent(AEvent);
				if AEvent.InfoPtr = FBtnSearch then begin
					if Trim(FFind.FEdit^.GetCaption) <> '' then
						Search(Trim(FFind.FEdit^.GetCaption))
					else
						PutCommand(cmSearch, @Self);
				end else
					PutCommand(cmSearch, @Self);
			end;
			cmNextMenu : begin
				ClearEvent(AEvent);
				FMenus^.Down(True);
			end;
			cmPrevMenu : begin
				ClearEvent(AEvent);
				FMenus^.Up(True);
			end;
			cmAbout : begin
				ClearEvent(AEvent);
				GetDlgAbout;
				FDlgAbout^.ShowModal;
			end;
		end;

	if (AEvent.What = evCommand) and (not FReadOnly) then
		case AEvent.Command of
			cmEditItem : begin
				ClearEvent(AEvent);
				GetEditMenu;
				R := PWindow(FEditMenu)^.ShowModal;
			end;
			cmMenuAdd : begin
				ClearEvent(AEvent);
				GetDlgMAdd;
				PDlgMAdd(FDlgMAdd)^.SetData(nil);
				R := PDialog(FDlgMAdd)^.ShowModal;
				if R = mrOK then
					AddMenu;
			end;
			cmPGMOpts : begin
				ClearEvent(AEvent);
				GetDlgPGMOpt;
				R := PDialog(FDlgPGMOpt)^.ShowModal;
				if (R = mrOK) and (PDlgPGMOpt(FDlgPGMOpt)^.Modified) then begin
					with Application^.FConfig do begin
						OpenSection('*');
						S := Trim(PDlgPGMOpt(FDlgPGMOpt)^.FTheme^.GetCaption);
						if GetValue('THEME', S) <> S then
							CheckLanguage(S);
						end;
						PDlgPGMOpt(FDlgPGMOpt)^.Save;
						Restart;
					end;
			end;
			cmPGMCfg : begin
				ClearEvent(AEvent);
				GetDlgPGMCfg;
				R := PDialog(FDlgPGMCfg)^.ShowModal;
				if (R = mrOK) and (PDlgPGMCfg(FDlgPGMCfg)^.Modified) then begin
					with Application^.FConfig do begin
						OpenSection('*');
						S := Trim(PDlgPGMCfg(FDlgPGMCfg)^.FLang^.GetCaption);
						if GetValue('LANGUAGE', S) <> S then
							CheckTheme(S);
						end;
					PDlgPGMCfg(FDlgPGMCfg)^.Save;
					{ need PGME to reset titles in cache to new language }
					DeleteCache;
					{ needed to update the "Resuming Eternity" message at restart to
					  the new language on very first restart. }
					FLanguage.SetFileName(UCase(Trim(PDlgPGMCfg(FDlgPGMCfg)^.FLang^.GetCaption)));
					FLanguage.Read;
					Restart;
				end;
			end;
		end;

	if (AEvent.What = evCommand) and (not FReadOnly) and FWritable then
		case AEvent.Command of
			cmMenuEdit : begin
				ClearEvent(AEvent);
				GetDlgMEdit;
				if Assigned(FMenus) and Assigned(FMenus^.FCurrent) then
					PDlgMEdit(FDlgMEdit)^.SetData(FMenus^.FCurrent)
				else
					PDlgMEdit(FDlgMEdit)^.SetData(nil);
				R := PDialog(FDlgMEdit)^.ShowModal;
				if R = mrOK then
					ModifyMenu;
			end;
			cmMenuDel : begin
				ClearEvent(AEvent);
				GetDlgMDelete;
				R := PDialog(FDlgMDel)^.ShowModal;
				if R = mrOK then
					DeleteMenu;
			end;
			cmProgAdd : begin
				ClearEvent(AEvent);
				GetDlgPAdd;
				PDlgPAdd(FDlgPAdd)^.SetData(nil);
				R := PDialog(FDlgPAdd)^.ShowModal;
				if R = mrOK then
					AddProgram;
			end;
			cmProgEdit : begin
				ClearEvent(AEvent);
				GetDlgPEdit;
				if Assigned(FPrograms) and Assigned(FPrograms^.FCurrent) then
					PDlgPEdit(FDlgPEdit)^.SetData(FPrograms^.FCurrent)
				else
					PDlgPEdit(FDlgPEdit)^.SetData(nil);
				R := PDialog(FDlgPEdit)^.ShowModal;
				if R = mrOK then
					ModifyProgram;
			end;
			cmProgDel : begin
				ClearEvent(AEvent);
				GetDlgPDelete;
				R := PDialog(FDlgPDel)^.ShowModal;
				if R = mrOK then
					DeleteProgram;
			end;
			cmProgMove : begin
				ClearEvent(AEvent);
				GetDlgMove;
				R := PDialog(FDlgMove)^.ShowModal;
				if R = mrOK then
					CopyOrMove(PDlgMove(FDlgMove)^.MoveItem);
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

{$ENDIF}
