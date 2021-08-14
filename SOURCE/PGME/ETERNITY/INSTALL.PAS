{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ETERNITY.DEF}
{$I ..\QUICKCRT\TEMPLATE.DEF}
{ DEFINE SOUND}
program Program_Manager_Eternity_Installer; { X Series }

{ The installer is a strictly modal program. Like a wizard. It walks through one
dialog after another until it is finished. Then, it closes. This is an example of only
one way this can be accomplished using the QuickCrt framework. Basically, it creates
the entire application and all dialog boxes. Then before the Application object is executed
it pushes a command into the QCrt event que. The application starts and sees the event
causing the first dialog to be executed. If the dialog closes with a good result, then
it pushes the command to trigger the next dialog. The last dialog pushes a cmQuit to
close the program. There are several other ways to accomplish the same result, but
this is one of the easiest. }

uses QCrt, QCLass, QStrings, QFmtStr, QFiles, QDos, QErrors, QObjects,
    QControls,	QFileDlg, QResrc;

{$I LICENSE.INC}

const
	class_TInstallOptions : TObjectClass ='TInstallOptions';
	class_TInstallPanel : TObjectClass = 'TInstallPanel';
	class_TInstaller : TObjectClass = 'TInstaller';

const
	cmContinue	  = $1000;
	cmBrowse	  = $1001;
	cmShowWelcome = $2001;
	cmShowLicense = $2002;
	cmShowOptions = $2003;
	cmShowInstall = $2004;
	cmShowThanks  = $2005;
	cmError		  = $2006;

const
	InstallSize   : LongInt = 0;
	InstallFiles  : Word = 0;
	InstallPath   : String = '';
	InstallError  : integer = 0;
	InstallUses	  : LongInt = 512; { could improve accuracy by scanning first }
	LstDir        : String = '';
	LsmDir        : String = '';
	BatchPath	  : String = '';
	BatchName	  : String = 'PGM.BAT';
	ExecName	  : String = 'PGME.EXE';
	RunName	  	  : String = 'PGMEGORN.BAT';
	IGOR		  : String = '';
	GoodState	  : boolean = false;

    {$IFNDEF TEMPLATES}
    var
        IRes    : PResource;
    {$ENDIF}

    var
        PLst : PFile;

procedure BadInstaller;
begin
    TextAttr := $4F;
    GotoXY(1,WhereY);
    ClrEol;
    Write(' Sorry. But, it appears that the installer is corrupted.');
    TextAttr := $07;
    WriteLn;
    ErrorsOnExit := False;
    {$IFNDEF TEMPLATES}
	if Assigned(IRes) then Dispose(IRes,Destroy);
	IRes := nil;
	if Assigned(PLst) then Dispose(PLst,Destroy);
	PLst := nil;
    {$ENDIF}
    Halt(1);
end;

procedure Detective;
var
	T, S : String;
	F : TFile;
	E, P : integer;
begin
	F.Create;
	BatchPath := '';
	InstallPath := '';

	T := TailDelim(GetEnv('DOSDIR'));
	if T <> '' then begin
	    if DirExists(T + 'APPINFO') then LsmDir:= TailDelim(T + 'APPINFO');
	    if DirExists(T + 'PACKAGES') then LstDir:= TailDelim(T + 'PACKAGES');
	end;

	{ Verify installer }
    {$IFNDEF TEMPLATES}
    InstallSize := IRes^.GetTotalSize(1);      { Block Type 1 for File }
    InstallUses := InstallSize;
    InstallFiles := IRes^.GetTotalCount(1);
	if (InstallSize < 10240) or (InstallFiles < 2) then  BadInstaller;
    {$ENDIF}

	{ Find and set path/filename for Igor. }

	T := GetEnv('TEMP');
	if T <> '' then begin
		if T[Length(T)] = '\' then
			T := '%TEMP%'
		else
			T := '%TEMP%\'
	end else begin
		T := GetEnv('TMP');
		if T = '' then
			T := Dir(InstallPath)
		else begin
			if T[Length(T)] = '\' then
				T := '%TMP%'
			else
				T := '%TMP%\'
		end;
	end;

	IGOR := T + RunName;
	{ Previous Launcher Batch file if it exists }
	T := GetEnv('PATH');
	while T <> '' do begin
		S := PullStr(';', T);
		if FileExists(Dir(S) + BatchName) then begin
			BatchPath := Dir(S);
			T := '';
		end;
	end;

	{ Previous installation or default to first fixed drive }
	if BatchPath <> '' then begin
		F.Create;
		F.Assign(Dir(S) + BatchName);
		F.Reset;
		E := F.IOResult;
		while (E = 0) and (not F.EOF) do begin
			F.ReadLn(S, Sizeof(S) - 1);
			S := Trim(UCASE(S));
			E := F.IOResult;
			if E = 0 then begin
				P := Pos('\' + ExecName, S);
				if P > 0 then begin
					S := Dir(Copy(S, 1, P - 1));
					if (S <> '') and DirExists(S) then begin
						E := -1;
						InstallPath := Dir(S);
					end;
				end;
			end;
		end;
		F.Close;
	end;
	if InstallPath = '' then
		InstallPath := Dir(Char(BootDrive + 64) + ':\ETERNITY');

	F.Destroy;
end;

type
	PInsPanel = ^TInsPanel;
	TInsPanel = object(TPanel)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
			PFileName : PLabel;
			PFProgress, PTProgress : PProgressBar;
			IRec : TResourceRec;
			Finished : boolean;
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure Copy; virtual;
		procedure SaveBatch; virtual;
	end;

	PInstaller = ^TInstaller;
	TInstaller = object(TApplication)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
		procedure BeforeShow; virtual;
	public
			PWelcome,
			PLicense,
			POptions,
			PInstall,
			PThanks,
			PError : PDialog;
			PDoneBtn : PButton;
			PInstalling : PInsPanel;
			PDest : PEdit;
			PBatch : PDropList;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		constructor Create;
		destructor Destroy; virtual;
		procedure Idle; virtual;
	end;

	POptionDialog = ^TOptionDialog;
	TOptionDialog = object(TDialog)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
			PDir : PPathDialog;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		constructor Create(AParent : PControl; AName : String);
	end;

{ --- File Installing Panel --- }
var
    PIns : PInsPanel;
    IBytes : longInt;

function Progress(AFileName : String; ACompleted, ABytes : word) : boolean; far;
var
    P : Longint;
begin
    Progress:=True;
    Inc(IBytes, ABytes);
    P := (IBytes * 100 div InstallSize) - 1;
    if P < 0 then P := 0;
    if P > 100 then P := 100;
    if Assigned(PIns) then begin
        PIns^.PFProgress^.SetProgress(ACompleted);
        PIns^.PTProgress^.SetProgress(P);
    end;
end;

function TInsPanel.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TInstallPanel) then
		ObjectClass := class_TInstallPanel
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TInsPanel.Create(AParent : PControl; AName : String);
var
	P : PControl;
	B : TBounds;
begin
	inherited Create(AParent, AName);
    PIns := @Self;
    {$IFNDEF TEMPLATES}
	IRes^.FindFirst('',IRec);
	{$ELSE}
	IRec.Data.BlockType := 0;
	{$ENDIF}

	IBytes := 0;

	TextAttr := $70;
	Bounds(1, 3, 1, 1, B);
	TextAttr := $70;
	PFileName := New(PLabel, Create(@Self, 'Name'));
	with PLabel(PFileName)^ do begin
		SetCaption('FILENAME.EXE');
		FHAlign := AlignRight;
		Bounds(B.Left + B.Width + 1, B.Top, 12, 1, B);
		SetAnchors(abNone);
		SetBounds(B);
	end;
	TextAttr := $1B;
	PFProgress := New(PProgressBar, Create(@Self, 'Progress'));
	with PProgressBar(PFProgress)^ do begin
		Bounds(B.Left + B.Width + 1, B.Top , 64 - (B.Left + B.Width), 1 , B);
		SetBounds(B);
		SetAnchors(abNone);
	end;

	TextAttr := $70;
	P := New(PLabel, Create(@Self, 'Complete'));
	with PLabel(P)^ do begin
		SetCaption('Complete');
		FHAlign := AlignRight;
		SetAnchors(abNone);
		Bounds(3, B.Top + 2, 12, 1, B);
		SetBounds(B);
	end;
	TextAttr := $1B;
	PTProgress := New(PProgressBar, Create(@Self, 'Total'));
	with PProgressBar(PTProgress)^ do begin
		Bounds(B.Left + B.Width + 1, B.Top , 64 - (B.Left + B.Width), 1 , B);
		SetBounds(B);
		SetAnchors(abNone);
	end;
	Finished := False;
end;

destructor TInsPanel.Destroy;
begin
    PIns := nil;
	inherited Destroy;
end;

procedure TInsPanel.SaveBatch;
var
	T : String;
	FOut : TFile;
begin
	MakeDir(BatchPath);
	T := Dir(BatchPath) + BatchName;
	if Assigned(PLst) then PLst^.WriteLn(T);
	FOut.Create;
	FOut.Assign(Dir(BatchPath) + BatchName);
	FOut.Rewrite;
	PFilename^.SetCaption(LSpace(BatchName, 12));
	PFProgress^.SetProgress(0);
	{$IFDEF SOUND}
		Sound(250);
		Delay(25);
		NoSound;
		Delay(50);
	{$ENDIF}
	InstallError := FOut.IOResult;

	T := '@ECHO OFF';
	if InstallError = 0 then FOut.WriteLn(T);
	T := 'IF EXIST ' + IGOR + ' DEL ' + IGOR;
	if InstallError = 0 then FOut.WriteLn(T);
	T := Dir(InstallPath) + ExecName + ' /BAT %1 %2 %3 %4 %5 %6 %7 %8 %9';
	if InstallError = 0 then FOut.WriteLn(T);
	T := 'IF EXIST ' + IGOR + ' ' + IGOR;
	if InstallError = 0 then FOut.WriteLn(T);
	if InstallError <> 0 then begin
		PInstaller(Application)^.PInstall^.Hide;
		PutCommand(cmNone, nil);
	end;
	PFProgress^.SetProgress(100);
	PTProgress^.SetProgress(100);
	FOut.Close;
	{$IFDEF SOUND}
		Delay(250);
	{$ENDIF}
	FOut.Destroy;
	Finished := True;
end;

procedure TInsPanel.Copy;
var
    L, S : String;

begin
	if Finished then exit;
	if IRec.Data.BlockType = 1 then begin
	    S := IRec.Name;
	    L := Dir(InstallPath) + S;
	    Delete(S, 1, Length(PathOf(S)));
		PFilename^.SetCaption(LSpace(S, 12));
		PFProgress^.SetProgress(0);
		{$IFDEF SOUND}
			Sound(250);
			Delay(25);
			NoSound;
			Delay(50);
		{$ENDIF}
        {$IFNDEF TEMPLATES}
        if (Not Assigned(PLst)) and (LstDir <> '') then begin
            PLst := New(PFile, Create);
            PLst^.Assign(LstDir + 'PGME.LST');
            PLst^.Rewrite;
        end;
        if NoTailDelim(PathOf(IRec.Name)) = 'APPINFO' then begin
            if Assigned(PLst) then begin
        	    L := Dir(GetEnv('DOSDIR')) + IRec.Name;
                PLst^.WriteLn(L);
                IRes^.ExportResource(Dir(GetEnv('DOSDIR')), IRec, Progress);
            end;
        end else begin
            if Assigned(PLst) then PLst^.WriteLn(L);
            IRes^.ExportResource(Dir(InstallPath), IRec, Progress);
        end;
        InstallError := IRes^.GetResult;
        {$ELSE}
        InstallError := 0;
        {$ENDIF}
		if InstallError <> 0 then begin
			PInstaller(Application)^.PInstall^.Hide;
			PutCommand(cmNone, nil);
			Finished := True;
			Exit;
		end;
        {$IFNDEF TEMPLATES}
            if not IRes^.FindNext(IRec) then
                IRec.Data.BlockType := 0;
        {$ENDIF}
	end;
	if IRec.Data.BlockType = 0 then begin
        {$IFNDEF TEMPLATES}
	    SaveBatch;
	    {$ENDIF}
	    if Assigned(PLst) then PLst^.Close;
	    Finished := True;
	end;
end;

{ --- Main Installer Application --- }
function TInstaller.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TInstaller) then
		ObjectClass := class_TInstaller
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TInstaller.Create;
var
	P : PControl;
	B : TBounds;
	T : TFormatData;
	S, E, G : String;
	I : Integer;
begin
	inherited Create(nil, 'Installer');
	SetHelpText ('This is the Program Manager Eternity Edition Installer.');
	AddHelpText ('');
	AddHelpText ('The installer is a strictly modal program. Like a wizard. It walks through one');
	AddHelpText ('dialog after another until it is finished. Then, it closes. The source');
	AddHelpText ('for the installer demonstrates one way this can be accomplished using the');
	AddHelpText ('QuickCrt framework. Basically, it creates the entire application');
	AddHelpText ('and all dialog boxes. Then before the Application object is executed');
	AddHelpText ('it pushes a command into the QCrt event que. The application starts');
	AddHelpText ('and sees the event causing the first dialog to be executed.');
	AddHelpText ('If the dialog closes with a good result, then it pushes the command');
	AddHelpText ('to trigger the next dialog. The last dialog pushes a cmQuit to');
	AddHelpText ('close the program. There are several other ways to accomplish the');
	AddHelpText ('same result, but this is one of the easiest. ');
	{ Get Total size of this installer }
	{ Create Welcome Dialog }
	PWelcome := New(PMessageBox, Create(@Self, 'Welcome'));
	with PMessageBox(PWelcome)^ do begin
		SetHelpText ('This is the Installers welcome dialog box.');
		AddHelpText ('');
		AddHelpText ('It shows the approximate amount of space required for a full installation.');
		Bounds(5,10,60,12, B);
		SetBounds(B);
		SetTitle('Program Manager Eternity Installer');
		SetMessage(
			'0;'#13 +
			'0;Welcome,'#13 +
			'0;'#13 +
			'0;This is the Program Manager Eternity Edition'#13 +
			'0;software installer. It will require approximately' + #13 +
			'0;%0 bytes for a complete installation.'
		);
		T.ID := fmtString;
		T.StringValue := Comma(IntStr(InstallUses));
		SetFormatData(T, 1);
		TextAttr := $1E;
		P := New(PButton, Create(PWelcome, 'Continue'));
		with PButton(P)^ do begin
			SetCaption('Continue');
			SetCommand(cmOk);
			SetRespond(cmEnter);
			with PControl(Parent)^ do
				Bounds(Width - 18, Height - 4, 12, 1, B);
			SetAnchors( abRight or abBottom );
			SetBounds(B);
		end;
	end;

	{ Create License Dialog }
	PLicense := New(PMessageBox, Create(@Self, 'License'));
	with PMessageBox(PLicense)^ do begin
		SetHelpText ('This is the Installers License Information box.');
		AddHelpText ('');
		AddHelpText ('It shows information reguarding the license that this version');
		AddHelpText ('of the Program Manager has been released under.');
		if Pos('invalid', SoftwareLicense) > 0 then begin
			SetBackground($4);
			SetTextColor($F);
		end;
		Bounds(5,10,73,11, B);
		SetBounds(B);
		SetTitle('Software License Agreement');
		SetMessage(
			#13 +
			SoftwareLicense
		);
		TextAttr := $3E;
		P := New(PButton, Create(PLicense, 'Accept'));
		with PButton(P)^ do begin
			if Pos('invalid', SoftwareLicense) > 0 then
				SetCaption('Ignore')
			else
				SetCaption('Accept');
			SetCommand(cmOk);
			SetRespond(cmEnter);
			with PControl(Parent)^ do
				Bounds(Width - 18, Height - 4, 12, 1, B);
			SetAnchors( abRight or abBottom );
			SetBounds(B);
		end;
	end;

	TextAttr := $07;
	{ Create Options Dialog }
	POptions := New(POptionDialog, Create(@Self, 'Options'));
	with PMessageBox(POptions)^ do begin
		SetHelpText ('This is the Installers Options dialog box. ');
		AddHelpText ('');
		AddHelpText ('It provides a way to change several of default installation options.');
		AddHelpText ('Also, if a previous version the PGM Eternity was installed,');
		AddHelpText ('it will attempt to configure itself to the same settings.');
		Bounds(5,10,60,12, B);
		SetBounds(B);
		SetTitle('Installation Options');
		TextAttr := $1E;
		P := New(PButton, Create(POptions, 'Install'));
		with PButton(P)^ do begin
			SetCaption('Install');
			SetCommand(cmOk);
			SetRespond(cmEnter);
			with PControl(Parent)^ do
				Bounds(Width - 19, Height - 4, 13, 1, B);
			SetAnchors( abRight or abBottom );
			SetBounds(B);
		end;
		TextAttr := $70;
		P := New(PLabel, Create(POptions, 'PATH'));
		with PLabel(P)^ do begin
			SetCaption('Install Destination Path:');
			with PControl(Parent)^ do
				Bounds(3, 2, 25, 1, B);
			SetAnchors( abLeft or abTop );
			SetBounds(B);
		end;
		TextAttr := $1E;
		PDest := New(PEdit, Create(POptions, 'PATH.EDIT'));
		with PEdit(PDest)^ do begin
			SetCaption(InstallPath);
			FHAlign := AlignLeft;
			with PControl(Parent)^ do
				Bounds(3, 3, Width - 20, 1, B);
			SetAnchors( abLeft or abTop or abRight );
			SetBounds(B);
		end;

		TextAttr := $2E;
		P := New(PButton, Create(POptions, 'Browse'));
		with PButton(P)^ do begin
			SetCaption('Browse');
			SetCommand(cmBrowse);
			with PControl(Parent)^ do
				Bounds(Width - 16, 3, 10, 1, B);
			SetAnchors( abRight or abTop );
			SetBounds(B);
		end;

		TextAttr := $70;
		P := New(PLabel, Create(POptions, 'LAUNCH'));
		with PLabel(P)^ do begin
			with PControl(Parent)^ do
				Bounds(3, 5, 37, 1, B);
			SetBounds(B);
			SetCaption('Location in PATH of PGM.BAT Launcher:');
			SetAnchors( abLeft or abTop );
		end;

		TextAttr := $1E;
		PBatch := New(PDropList, Create(POptions, 'LAUNCH.DROP'));
		with PDropList(PBatch)^ do begin
			SetSorted(True);
			SetReadOnly(True);
			E := GetEnv('PATH');
			if E = '' then begin
				S := DriveLetters;
				for I := 1 to Length(S) do
					if S[I] = UCase(S[I]) then begin
						if Length(E) > 0 then
							E := E + ';' + S[I] + ':\'
						else
							E := S[I] + ':\';
					end;
			end;
			G := '';
			while Length(E) > 0 do begin
				S := Dir(PullStr(';', E)) + BatchName;
				if (Not IsDOSBox) or (Ucase(Copy(S,1,2)) <> 'Z:') then begin
    				AddItem(S);
	    			if (G = '') or (S[1] < G[1]) or ( (S[1] = G[1]) and (Length(S) < Length(G) )) then
		    			G := S;
		    	end;
			end;
			if BatchPath <> '' then
				G := Dir(BatchPath) + BatchName
			else
				if G = '' then G := Dir('\') + BatchName;
			SetCaption(G);
			FEdit.FHAlign := AlignLeft;
			with PControl(Parent)^ do
				Bounds(3, 6, Width - 9, 10, B);
			SetAnchors( abLeft or abTop or abRight or abBottom );
			SetBounds(B);
		end;
	end;

	{ Create Install Dialog }
	PInstall := New(PDialog, Create(@Self, 'Install'));
	with PMessageBox(PInstall)^ do begin
		SetHelpText ('This is the Installers Installation box.');
		AddHelpText ('');
		AddHelpText ('It shows the progress of the installation.');
		AddHelpText ('Just wait for the "Done" button to appear.');
		Bounds(5,10,70,10, B);
		SetBounds(B);
		SetTitle('Installing Eternity');
		TextAttr := $1E;
		PDoneBtn := New(PButton, Create(PInstall, 'Done'));
		with PButton(PDoneBtn)^ do begin
			SetCaption('Done');
			SetCommand(cmOk);
			SetRespond(cmEnter);
			with PControl(Parent)^ do
				Bounds(Width - 18, Height - 4, 12, 1, B);
			SetAnchors( abRight or abBottom );
			SetBounds(B);
			SetVisible(False);
		end;
		TextAttr := $71;
		PInstalling := New(PInsPanel, Create(PInstall, 'Installing'));
		with PInstalling^ do begin
			with PControl(Parent)^ do
				Bounds(1,1, Width - 18, Height - 5, B);
			SetAnchors(alTop or alLeft or alRight or abBottom);
			SetBounds(B);
		end;
	end;

	{ Create Thanks Dialog }
	PThanks := New(PMessageBox, Create(@Self, 'Thanks'));
	with PMessageBox(PThanks)^ do begin
		SetHelpText ('This is the Installers Finished dialog box.');
		AddHelpText ('');
		AddHelpText ('It shows that the installation has completed and thanks you for');
		AddHelpText ('choosing PGME. Why did you need help on this?');
		Bounds(5,10,60,10, B);
		SetBounds(B);
		SetTitle('Thank You');
		SetMessage(
			#13  +
			'Installation is now complete.'#13 +
			#13 +
			'Thank You for using the Program Manager Eternity.'
		);
		TextAttr := $3E;
		P := New(PButton, Create(PThanks, 'Exit'));
		with PButton(P)^ do begin
			SetCaption('Exit');
			SetCommand(cmOk);
			SetRespond(cmEnter);
			with PControl(Parent)^ do
				Bounds(Width - 18, Height - 4, 12, 1, B);
			SetAnchors( abRight or abBottom );
			SetBounds(B);
		end;
	end;

	{ Create Error Dialog }
	PError := New(PMessageBox, Create(@Self, 'Error'));
	with PMessageBox(PError)^ do begin
		SetHelpText ('This is and Error dialog box.');
		AddHelpText ('');
		AddHelpText ('Something really, really, really bad happend. Sorry.');
		SetBackground($4);
		SetTextColor($F);
		Bounds(5,10,60,10, B);
		SetBounds(B);
		SetTitle('Oh, No!');
		SetMessage(
			'0;'#13  +
			'0;A "%0" error has occured.'#13 +
			'0;'#13 +
			'Sorry, but the installation has failed.'
		);
		TextAttr := $07;
		P := New(PButton, Create(PError, 'Exit'));
		with PButton(P)^ do begin
			SetCaption('Exit');
			SetCommand(cmOk);
			SetRespond(cmEnter);
			with PControl(Parent)^ do
				Bounds(Width - 18, Height - 4, 12, 1, B);
			SetAnchors( abRight or abBottom );
			SetBounds(B);
		end;
	end;

	{ Force first dialog }
	FUseDac := False;
	FUseFader := False;
	FStartTone := '';
	FEndTone := '';
end;

destructor TInstaller.Destroy;
begin
	inherited Destroy;
end;

procedure TInstaller.CheckLocal(var AEvent : TEvent);
var
	T : integer;
	D : TFormatData;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmShowWelcome : begin
				T := PWelcome^.ShowModal;
				if T <> mrOK then PutCommand(cmQuit, nil);
				ClearEvent(AEvent);
				PutCommand(cmShowLicense, nil);
			end;
			cmShowLicense : begin
				T := PLicense^.ShowModal;
				if T <> mrOK then PutCommand(cmQuit, nil);
				ClearEvent(AEvent);
				PutCommand(cmShowOptions, nil);
			end;
			cmShowOptions : begin
				T := POptions^.ShowModal;
				if T <> mrOK then PutCommand(cmQuit, nil);
				InstallPath := Dir(PDest^.GetCaption);
				BatchPath := Dir(PBatch^.GetCaption);
				BatchPath := Copy(BatchPath, 1, Length(BatchPath) - Length(BatchName) - 1);
				ClearEvent(AEvent);
				PutCommand(cmShowInstall, nil);
			end;
			cmShowInstall : begin
				T := PInstall^.ShowModal;
				if T <> mrOK then begin
					if InstallError <> 0 then
						PutCommand(cmError, nil)
					else
						PutCommand(cmQuit, nil);
				end else
					PutCommand(cmShowThanks, nil);
				ClearEvent(AEvent);
			end;
			cmShowThanks : begin
				PThanks^.ShowModal;
				PutCommand(cmQuit, nil);
				ClearEvent(AEvent);
			   	GoodState := True;
			end;
			cmError : begin
				D.ID := fmtString;
				D.StringValue := ErrorStr(InstallError);
				PMessageBox(PError)^.SetFormatData(D, 1);
				PError^.ShowModal;
				PutCommand(cmQuit, nil);
				ClearEvent(AEvent);
			end;
		end;
	inherited CheckLocal(AEvent);
end;

procedure TInstaller.Idle;
var
	B : TBounds;
begin
	{ Delay calls IdleProc while waiting, can cause recursive issue. So Null It. Normally,
	  You Shouldn't put Delays inside an Idle Procedure!!! }
	{$IFDEF SOUND}
		{ QCrt.IdleProc := NullProc;	 }
	{$ENDIF}
	inherited Idle;
	if Assigned(PInstalling) and PInstalling^.GetShown then begin

		if not PInstalling^.Finished then begin
			PInstalling^.Copy;
			Exit;
		end;
		if Assigned(PDoneBtn) and PDoneBtn^.FVisible = true then exit;
		PInstalling^.GetBounds(B);
		Dec(B.Height, 2);
		PInstalling^.SetBounds(B);
		PInstalling^.PFileName^.SetCaption(Lspace('Completed', 12));
		PDoneBtn^.SetVisible(True);
		PDoneBtn^.Show;
		{$IFDEF SOUND}
			Sound(500);
			Delay(250);
			NoSound;
		{$ENDIF}
	end;
end;

procedure TInstaller.BeforeShow;
begin
	inherited BeforeShow;
	TextAttr := $07;
	GotoXY(1,WhereY);
	ClrEol;
end;

{ --- TOptionDialog --- }
function TOptionDialog.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TInstallOptions) then
		ObjectClass := class_TInstallOptions
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TOptionDialog.Create(AParent : PControl; AName : String);
begin
	inherited Create(AParent, AName);
	PDir := New(PPathDialog, Create(AParent, 'Browser'));
end;

procedure TOptionDialog.CheckLocal;
var
	T : integer;
	S : String;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmEdit : if AEvent.InfoPtr = PInstaller(Application)^.PDest then begin
				S := Dir(PInstaller(Application)^.PDest^.GetCaption);
				PInstaller(Application)^.PDest^.SetCaption(S);
			end;
			cmBrowse : begin
				ClearEvent(AEvent);
				PDir^.SetDir(PInstaller(Application)^.PDest^.GetCaption);
				T := PDir^.ShowModal;
				if T = mrOK then begin
					S := Dir(PDir^.GetDir);
					PInstaller(Application)^.PDest^.SetCaption(S);
					PInstaller(Application)^.PDest^.Update;
				end;
			end;
		end;
	inherited CheckLocal(AEvent);
end;

var
	Installer : TInstaller;

begin
	{$IFDEF SOUND}
		NoSound;
	{$ENDIF}
	TextAttr := $07;
	Write('Starting installer...');
	{$IFNDEF TEMPLATES}
    IRes := New(PResource, Create(ParamStr(0), New(PFile, Create)));
    if not IRes^.Open then BadInstaller;
	Detective;
	{$ENDIF}
	with Installer do begin
		Create;
		{$IFNDEF TEMPLATES}
		PutCommand(cmShowWelcome, nil);
		{$ENDIF}
		Execute;
		Destroy;
	end;
    {$IFNDEF TEMPLATES}
	if Assigned(IRes) then Dispose(IRes,Destroy);
	IRes := nil;
	if Assigned(PLst) then Dispose(PLst,Destroy);
	PLst := nil;
    {$ENDIF}
	if GoodState then begin
		TextAttr := $0B;
		GotoXY(1,WhereY);
		TextAttr := $07;
		ClrEol;
		WriteLn;
		WriteLn('The Program Manager Eternity has been installed.');
		WriteLn('It may be launched by entering "PGM.BAT" from the command line.');
		WriteLn;
		TextAttr := $0B;
		Write('Goodbye.');
		TextAttr := $07;
		WriteLn;
	end else begin
		{$IFNDEF TEMPLATES}
		TextAttr := $4F;
		GotoXY(1,WhereY);
		Write(' The Program Manager Eternity installation program has been aborted.');
		ClrEol;
		TextAttr := $07;
		WriteLn;
		{$ENDIF}
		ErrorsOnExit := False;
		Halt(1);
	end;
end.
