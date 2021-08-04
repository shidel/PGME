{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ --- TApplication --- }

{$IFDEF INTERFACE}
const
	class_TApplication = 'TApplication';

type
	PApplication = ^TApplication;
	TApplication = object(TControl)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FHelp : PHelp;
		FInfo : TInformation;
		FOriginalPath : String;
		FLanguageID : String;
		FConfig,
		FLanguage,
		FTheme,
		FKeyboard : TConfigFile;
		FFont : TFont;
		FFocused, FMouseFocus : PControl;
		FDefaultDAC, FUserDAC : Pointer;
		FTerminator, FCanQuit : boolean;
	{ User Config Stuff }
		FClrScr,
		FUseFader,
		FUseDAC,
		FUseSound, FInSound, FOutSound : boolean;
		FFileDateFmt, FFileTimeFmt : String;
		FStartTone, FEndTone : String;

	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure Execute; virtual;
		procedure Terminate; virtual;
		function GetClearScreen : boolean; virtual;
		procedure SetClearScreen(AValue : boolean); virtual;
		function GetConfigName : string; virtual;
		procedure ApplyConfig( AConfig : PConfigFile ); virtual;
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
		procedure ApplyKeyboard( AConfig : PConfigFile ); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;

		procedure ApplySettings( AControl : PControl ); virtual;
		procedure AfterConfig; virtual;
		procedure AtStartup; virtual;
		procedure AtShutdown; virtual;

		procedure CreateAllDialogs(ConserveMemory : boolean); virtual;
		{ Only used when making Templates, Slow and major memory hog when not given true. }
		procedure FreeDialog( var ADialog : PDialog ); virtual;
		procedure FreeAllDialogs; virtual;
		function GetCommandName( ACode : word ) : String; virtual;
		function GetKeyName( ACode : TKeyValue ) : String; virtual;
		function GetCommandCode( ACommand : string ) : word; virtual;
		procedure Configure; virtual;
		procedure Idle; virtual;
		procedure SetFocused(AControl : PControl); virtual;
		function GetFocused : PControl; virtual;
		{$IFDEF TEMPLATES}
		procedure OnDisk(AObject : PObject); virtual;
		procedure WriteOnDisk(AConfig : TConfigFile; Section : String); virtual;
		{$ENDIF}
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TApplication.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TApplication) then
		ObjectClass := class_TApplication
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TApplication.Create(AParent : PControl; AName : String);
var
	B : TBounds;
	T : String;
begin
	{$IFDEF DEVLOG}
		WriteLog('MaxAvail ' + IntStr(MaxAvail) + ' / MemAvail ' + IntStr(MemAvail));
	{$ENDIF}
	inherited Create(AParent, AName);
	FTerminator := False;
{	FPreserve := True;}
	FInfo.Create(ftProgram);
	FOriginalPath := '';
	if not assigned(Application) then begin
		Application := @Self;
		GetDir(0, FOriginalPath);
		ChDir(NoTailDelim(GetExePath));
	end;
	FLanguageID := 'en_US';
	FClearWindow := False;
	FAnchors := alClient;
	FConfig.Create('CONFIG');
	FConfig.SetFreeOnDestroy(False);
	Bounds(1,1,Lo(ScreenMax) + 1, Hi(ScreenMax) + 1,B);
	SetBounds(B);
	FHelp := New(PHelp, Create(@Self, 'HELP'));
	FTheme.Create('THEME');
	FTheme.SetFreeOnDestroy(False);
	FKeyboard.Create('KEYS');
	FKeyboard.SetFreeOnDestroy(False);
	FFont.Create('FONT');
	FFont.SetFreeOnDestroy(False);
	FFocused := nil;
	FMouseFocus := nil;
	FUseFader := True;
	FUseDac := True;
	FDefaultDac := nil;
	FUserDAC := nil;
	FClrScr := false;
	FUseSound := True;
	FInSound := True;
	FOutSound := True;
	FCanQuit := True;

	FStartTone := '200:2,300:2,400:2,500:3,0:3,400:2,500:6';
	FEndTone := '500:1,100:1,300:1';
end;

destructor TApplication.Destroy;
begin
	if Assigned(FDefaultDAC) then
		begin
			FreeMem(FDefaultDAC, Sizeof(TDac));
			FDefaultDAC := nil;
		end;
	if Assigned(FUserDAC) then
		begin
			FreeMem(FUserDAC, Sizeof(TDac));
			FUserDAC := nil;
		end;

	FTheme.Destroy;
	FLanguage.Destroy;
	FConfig.Destroy;
	FKeyboard.Destroy;
	FFont.Destroy;

	if Application = @Self then begin
		{$I-}
		Chdir(NoTailDelim(FOriginalPath));
		{$I+}
	end;
	FInfo.Destroy;
	inherited Destroy;
	{$IFDEF DEVLOG}
		WriteLog('MaxAvail ' + IntStr(MaxAvail) + ' / MemAvail ' + IntStr(MemAvail));
	{$ENDIF}
end;

procedure TApplication.CreateAllDialogs;
begin
end;

procedure TApplication.FreeDialog;
begin
	if not assigned(ADialog) then exit;
	{$IFDEF TEMPLATES}
		OnDisk(ADialog);
	{$ENDIF}
	if Assigned(ADialog^.Parent) then
		PListItem(ADialog^.Parent)^.Remove(ADialog);
	Dispose(ADialog, Destroy);
	ADialog := nil;
end;

procedure TApplication.FreeAllDialogs;
var
	P, N : PListItem;
begin
	P := First;
	while Assigned(P) do begin
		N := P^.Next;
		if P^.ClassOf(class_TDialog) then begin
			{$IFDEF TEMPLATES}
				OnDisk(P);
			{$ENDIF}
			if Assigned(P^.Parent) then Remove(P);
			Dispose(P, Destroy);
		end;
		P := N;
	end;
end;

procedure TApplication.Configure;
begin
	FConfig.SetFileName(GetConfigName);
	FConfig.Read;
	FConfig.OpenSection('*');
	{$IFDEF TEMPLATES}
		FConfig.GetValue('TYPE', ftConfig);
	{$ENDIF}
	FLanguage.Create('Language');
	FLanguage.SetFreeOnDestroy(False);
	FLanguage.SetFileName(FConfig.GetValue('LANGUAGE', 'DEFAULT' + LanguageExt));
	FLanguage.Read;
	{$IFDEF TEMPLATES}
		AddComments(FLanguage, @LanguageComments);
	{$ENDIF}
	FLanguage.OpenSection('*');
	FLanguageID := FLanguage.GetValue('ID', 'en_US');
	FLanguage.GetValue('TYPE', ftLanguage);
	FLanguage.GetValue('NAME', 'US English');
	FFileDateFmt := FLanguage.GetValue('FILE.DATE', '0[X3.2],1[L2],2[Z2];%1/%2/%0');
	FFileTimeFmt := FLanguage.GetValue('FILE.TIME', '0[Z2],1[L2],2[Z2],3[Z2],4{a,p};%1:%2%4');

	FTheme.SetFileName(FConfig.GetValue('THEME', 'DEFAULT' + ThemeExt) );
	FTheme.Read;
	{$IFDEF TEMPLATES}
		AddComments(FTheme, @ThemeComments);
		FTheme.OpenSection('*');
		FTheme.GetValue('TYPE', ftTheme);
		FTheme.GetValue('ID', 'DEFAULT');
		FTheme.GetValue('NAME', 'Default Theme');
	{$ENDIF}
	FKeyboard.SetFileName(FConfig.GetValue('KEYBOARD', 'DEFAULT' + KeymapExt) );
	FKeyboard.Read;
	FKeyboard.SetAutoAdd(True);
	{$IFDEF TEMPLATES}
		AddComments(FKeyboard, @KeyMapComments);
		FKeyboard.OpenSection('*');
		FKeyboard.GetValue('TYPE', ftKeyMap );
		FKeyboard.GetValue('ID', 'USPC101');
		FKeyboard.GetValue('NAME', 'US English 101-key PC Enhanced Keyboard');
	{$ENDIF}
	FFont.SetFileName(FConfig.GetValue('FONT', 'DEFAULT' + FontExt));
end;

procedure TApplication.Idle;
begin
end;

procedure TApplication.ApplySettings (AControl : PControl);
begin
	AControl^.ApplyConfig(@FConfig);
	AControl^.ApplyLanguage(@FLanguage);
	AControl^.ApplyTheme(@FTheme);
	AControl^.ApplyKeyboard(@FKeyboard);
end;

procedure TApplication.Terminate;
begin
	FTerminator := True;
	PutCommand(cmQuit, @Self);
end;

procedure TApplication.Execute;
var
	TempDac : Pointer;
	Size : integer;
	Screen, Temp : Pointer;
	Event : TEvent;
	QCrtSettings : TQCrtSettings;
	{$IFDEF DEVLOG_EVENTS}
		TempStr : String;
	{$ENDIF}
begin
	Screen := nil;
	GetQCrtState(QCrtSettings);

	Configure;
	ApplySettings(@Self);
    AfterConfig;
	if FUseDac then begin
		if Not MemCheck(Sizeof(TDac)) then Halt(erInsufficient_Memory);
		if not Assigned(FDefaultDAC) then
			GetMem(FDefaultDAC, Sizeof(TDac));
		ReadDac(0,256,FDefaultDAC^);
		if Not MemCheck(Sizeof(TDac)) then Halt(erInsufficient_Memory);
		if not Assigned(FUserDAC) then begin
			GetMem(FUserDAC, Sizeof(TDac));
			Move(FDefaultDAC^, FUserDAC^, Sizeof(TDac));
		end;
	end;

	{$IFDEF TEMPLATES}
		CreateAllDialogs(True);
	{$ENDIF}

	{$IFNDEF TEMPLATES} { If template mode then don't loop application }
		CheckScroll := False;
		CheckCursor := False;
		HideCursor;
        AtStartup;

		{ Same as TWindow.ShowModal but this the application version }
		if Not FShown then
			begin
				if FUseDAC and FUseFader then
					FadeOutDAC(0,256,FDefaultDAC^);
			end;

		if (FFont.GetFileName <> '') and (not FClrScr) then
			begin
				Size := GetVideoSize;
				Temp := GetVideoPtr;
				if Not MemCheck(Size) then Halt(erInsufficient_Memory);
				GetMem(Screen, Size);
				SetVideoPtr(Screen);
				CopyFromVideo;
				SetVideoPtr(Temp);
			end;

		if FFont.GetFileName <> '' then
			begin
				FFont.SetEnabled(True);
				HideCursor;
			end;

		if FClrScr then begin
			TextAttr := $07;
			ClrScr;
		end;

		if Not FShown then
			begin
				if FUseDAC and FUseFader then begin
					if Not MemCheck(Sizeof(TDac)) then Halt(erInsufficient_Memory);
					GetMem(TempDac, Sizeof(TDac));
					FillChar(TempDac^, Sizeof(TDac), 0);
					WriteDAC(0,256,TempDAC^);
					FreeMem(TempDac, Sizeof(TDac));
				end;
				FVisible := True;
				AdjustSize;
				Show;
				if FUseDAC then begin
					if FUseFader then
						FadeInDAC(0,256,FUserDAC^);
					WriteDAC(0,256,FUserDAC^)
				end;
			end;
			{$IFDEF DEVLOG}
				WriteLog('Start Execution Loop');
				LogInc;
			{$ENDIF}
			if FInSound and (FStartTone <> '') then SpeakerPlay(FStartTone);
			repeat
				ClearEvent(Event);
				while Event.What = evNothing do begin
					GetEvent(Event);
					if (Event.What = evNothing) then Idle;
				end;
				{$IFDEF DEVLOG_EVENTS}
				if Event.What and evMouse = evNone then begin
					TempStr := 'Event: ';
					case Event.What of
						evNone : TempStr := TempStr + 'evNone';
						evKeyboard :  begin
							TempStr := TempStr + 'evKeyboard ';
							TempStr := TempStr + ZPad(BinStr(Event.ShiftCode), 16) + ':' +
								ZPad(HexStr(Event.KeyCode), 4);
							if Event.Original <> 0 then
								TempStr := TempStr + ' (' + ZPad(HexStr(Event.Original), 4) + ')';
								end;
						evCommand : begin
							TempStr	:= TempStr + ' evCommand ' + Application^.GetCommandName(Event.Command);
						end;
					end;
					WriteLog(TempStr);
				end;
				{$ENDIF}
				if (Event.What <> evCommand) or (Event.Command <> cmQuit) then
					CheckEvent(Event);
			until (FTerminator) or (Not FShown) or
			(FCanQuit and (Event.What = evCommand) and (Event.Command = cmQuit));
			if FOutSound and (FEndTone <> '') then begin
				PurgeEvents;
				SpeakerPlay(FEndTone);
				While SpeakerState and SpeakerPlaying do begin
					ClearEvent(Event);
					GetEvent(Event);
					if (Event.What = evSystem) and (Event.Command = cmSpeaker) then
						SpeakerNext;
				end;
				SpeakerOff;
			end;
			if Assigned(Application) then Application^.FFocused := nil;
			{ IdleProc := OldIdleProc; }
			{$IFDEF DEVLOG}
				LogDec;
				WriteLog('End Execution Loop');
			{$ENDIF}

		if FUseDAC and FUseFader then
			FadeOutDAC(0,256,FUserDAC^);

		Hide;
	{$ENDIF}

	{$IFDEF TEMPLATES}
		FKeyboard.Write;
		FTheme.Write;
		FLanguage.Write;
		FConfig.Write;
	{$ENDIF}

	{$IFNDEF TEMPLATES}
		FFont.SetEnabled(False);
	{$ENDIF}

	SetQCrtState(QCrtSettings);
	if Assigned(Screen) then
		begin
			Temp := GetVideoPtr;
			SetVideoPtr(Screen);
			CopyToVideo;
			SetVideoPtr(Temp);
			FreeMem(Screen, Size);
		end;
	if FUseDAC and Assigned(FDefaultDac) then begin
		if FUseFader and (Not FClrScr) then
			FadeInDAC(0,256,FDefaultDAC^);
		WriteDac(0,256,FDefaultDAC^);
	end;
	AtShutdown;
end;

function TApplication.GetClearScreen : boolean;
begin
	GetClearScreen := FClearWindow;
end;

procedure TApplication.SetClearScreen(AValue : boolean);
var
	Reshow : boolean;
begin
	if AValue = FClearWindow then exit;
	Reshow := FShown;
	if Reshow then Hide;
	FClearWindow := AValue;
	if Reshow then Hide;
end;

function TApplication.GetConfigName : string;
var
	T : String;
begin
	T := GetExeName;
    T := Copy(T, 1, LastPos('.', T) - 1 );
    if T = '' then
    	GetConfigName := ''
    else
    	GetConfigName := GetExePath + T + ConfigExt;
end;

procedure TApplication.ApplyLanguage( AConfig : PConfigFile );
var
	I : integer;
	Key, Str, Temp : String;

begin
	inherited ApplyLanguage(AConfig);

	AConfig^.OpenSection('KEYNAMES');

	for I := $61 to $7A do begin
		Temp := AConfig^.GetValue('$' + ZPad(HexStr(I),4), UCase(Char(I)));
	end;
	for I := $30 to $39 do begin
		Temp := AConfig^.GetValue('$' + ZPad(HexStr(I),4), Char(I));
	end;
	for I := $3B to $44 do begin
		Temp := AConfig^.GetValue('$' + ZPad(HexStr(I), 2) + '00', 'F' + IntStr(I- $3A));
	end;

	for I := 0 to DefaultKeyNamesMax do begin
		Str := DefaultKeyNames[I];
		Key := Copy(Str, 1, Pos('=', Str) - 1 );
		Temp := Copy(Str, Length(Key) + 2, Length(Str));
		Temp := AConfig^.GetValue(Key, Temp);
	end;
end;

procedure TApplication.ApplyKeyboard( AConfig : PConfigFile );
var
	I, VCount : integer;
	Key, Str, Temp, Flag, Scan : String;
	P, T : PListItem;
	PMap : PCodeMap;
begin
	inherited ApplyKeyboard(AConfig);

	{ Key Code Mapping }
	AConfig^.OpenSection('ASSIMILATE');
	{$IFNDEF TEMPLATES}
	if not AConfig^.IsValid then
	{$ENDIF}
		for I := 0 to DefaultMappedKeysMax do begin
			Str := DefaultMappedKeys[I];
			Key := Copy(Str, 1, Pos('=', Str) - 1 );
			Temp := Copy(Str, Length(Key) + 2, Length(Str));
			Temp := AConfig^.GetValue(Key, Temp);
			{ WriteLog(Key + '+' + Temp); }
		end;

	{ Count them }
	VCount := 0;
	if Assigned(AConfig^.FSectionKey) then begin
		P := AConfig^.FSectionKey^.First;
		while Assigned(P) do begin
			T := P^.First;
			while Assigned(T) do begin
				VCount := VCount +  Length(T^.GetNameID) - Length(FilterChar(T^.GetNameID, ',')) + 1;
				T := T^.Next;
			end;
			P := P^.Next;
		end;
	end;

	{ Create KeyMap  }
	PMap := nil;
	if Assigned(AConfig^.FSectionKey) then begin
		GetMem(PMap, SizeOf(TCodeMapEntry) * VCount);
 		VCount := 0;
		P := AConfig^.FSectionKey^.First;
		while Assigned(P) do begin
			T := P^.First;
			Key := P^.GetNameID;
			while Assigned(T) do begin
				Str := T^.GetNameID;
				While Str <> '' do begin
					Flag := '';
					Scan := '$0000';
					I := Pos(',', Str);
					If I = 0 then I := Length(Str) + 1;
					Temp := Copy(Str, 1, I - 1);
					Str := Copy(Str, I + 1, Length(Str));
					I := Pos(':', Temp);
					if I > 0 then begin
						Flag := Ucase(Temp[I + 1]); { I don't care if user messes it up }
						Delete(Temp, I, 2);
					end;
					I := Pos('+', Temp);
					if I > 0 then begin
						Scan := Copy(Temp, I + 1, Length(Temp)); { I don't care if user messes it up }
						Delete(Temp, I, Length(Scan) + 1 );
					end;
					TCodeMap(PMap^)[VCount].ToCode := StrInt(Key);
					TCodeMap(PMap^)[VCount].FromCode := StrInt(Temp);
					TCodeMap(PMap^)[VCount].ShiftCode := StrInt(Scan);
					if Flag = 'C' then
						TCodeMap(PMap^)[VCount].Flags := $0001
					else if Flag = 'M' then
						TCodeMap(PMap^)[VCount].Flags := $0002
					else
						TCodeMap(PMap^)[VCount].Flags := $0000;
					{ Writelog(IntStr(VCount) + ' ' + Temp + '(' + Scan + ':' + Flag + ')->' + Key ); }
					Inc(VCount);
				end;
				T := T^.Next;
			end;
			P := P^.Next;
		end;
	end;
	if Assigned(PMap) then begin
		SetKeyMap(PMap, VCount);
		FreeMem(PMap, SizeOf(TCodeMapEntry) * VCount);
	end else begin
		SetKeyMap(nil, 0);
	end;

	{ Command Mapping, very similar to key map }
	AConfig^.OpenSection('COMMANDS');
	{$IFNDEF TEMPLATES}
	if not AConfig^.IsValid then
	{$ENDIF}
		for I := 0 to DefaultMappedCommandsMax do begin
			Str := DefaultMappedCommands[I];
			Key := Copy(Str, 1, Pos('=', Str) - 1 );
			Temp := Copy(Str, Length(Key) + 2, Length(Str));
			Temp := AConfig^.GetValue(Key, Temp);
			{ WriteLog(Key + '=' + Temp); }
		end;

	{$IFDEF TEMPLATES}
	for I := 0 to DefaultCommandNamesMax do begin
		Str := DefaultCommandNames[I];
		Key := Copy(Str, 1, Pos('=', Str) - 1 );
		if StrInt(Copy(Str, Length(Key) + 2, Length(Str))) < $0500 then
			Temp := AConfig^.GetValue(Key, '$0000');
		{ WriteLog(Key + '=' + Temp); }
	end;
	{$ENDIF}

	{ Count them }
	VCount := 0;
	if Assigned(AConfig^.FSectionKey) then begin
		P := AConfig^.FSectionKey^.First;
		while Assigned(P) do begin
			T := P^.First;
			while Assigned(T) do begin
				VCount := VCount +  Length(T^.GetNameID) - Length(FilterChar(T^.GetNameID, ',')) + 1;
				T := T^.Next;
			end;
			P := P^.Next;
		end;
	end;

	{ Create Command Map }
	PMap := nil;
	if Assigned(AConfig^.FSectionKey) then begin
		GetMem(PMap, SizeOf(TCodeMapEntry) * VCount);
 		VCount := 0;
		P := AConfig^.FSectionKey^.First;
		while Assigned(P) do begin
			T := P^.First;
			Key := IntStr(GetCommandCode(UCase(P^.GetNameID)));
			while Assigned(T) do begin
				Str := T^.GetNameID;
				While Str <> '' do begin
					Flag := '';
					Scan := '$0000';
					I := Pos(',', Str);
					If I = 0 then I := Length(Str) + 1;
					Temp := Copy(Str, 1, I - 1);
					Str := Copy(Str, I + 1, Length(Str));
					I := Pos(':', Temp);
					if I > 0 then begin
						Flag := Ucase(Temp[I + 1]); { I don't care if user messes it up }
						Delete(Temp, I, 2);
					end;
					I := Pos('+', Temp);
					if I > 0 then begin
						Scan := Copy(Temp, I + 1, Length(Temp)); { I don't care if user messes it up }
						Delete(Temp, I, Length(Scan) + 1 );
					end;
					TCodeMap(PMap^)[VCount].ToCode := StrInt(Key);
					TCodeMap(PMap^)[VCount].FromCode := StrInt(Temp);
					TCodeMap(PMap^)[VCount].ShiftCode := StrInt(Scan);
					if Flag = 'C' then
						TCodeMap(PMap^)[VCount].Flags := $0001
					else if Flag = 'M' then
						TCodeMap(PMap^)[VCount].Flags := $0002
					else
						TCodeMap(PMap^)[VCount].Flags := $0000;
					(* with TCodeMap(PMap^)[VCount] do
						Writelog('{' + Lspace(IntStr(VCount),3) + '}' +
						'( FromCode:$' + ZPad(HexStr(FromCode), 4) + '; ' +
						' ToCode:$' + ZPad(HexStr(ToCode), 4) + '; ' +
						' ShiftCode:$' + ZPad(HexStr(ShiftCode), 4) + '; ' +
						' Flags:$' + ZPad(HexStr(Flags), 4) + ' ), ' ); *)
				Inc(VCount);
				end;
				T := T^.Next;
			end;
			P := P^.Next;
		end;
	end;
	if Assigned(PMap) then begin
		SetCommandMap(PMap, VCount);
		FreeMem(PMap, SizeOf(TCodeMapEntry) * VCount);
	end else begin
		SetCommandMap(nil, 0);
	end;
end;

procedure TApplication.AfterConfig;
begin
end;

procedure TApplication.AtStartup;
begin
end;

procedure TApplication.AtShutdown;
begin
end;

function TApplication.GetCommandName( ACode : word ) : String;
var
	I : integer;
	T, S : String;
begin
	T := '$' + ZPad(HexStr(ACode), 4);
	GetCommandName := T;
	for I := 0 to DefaultCommandNamesMax do begin
		S := UCase( Copy(DefaultCommandNames[I],Pos('=', DefaultCommandNames[I]) + 1, Length(T) + 4));
		if S = T then begin
			GetCommandName := Copy(DefaultCommandNames[I], 1, Pos('=', DefaultCommandNames[I]) - 1);
			Break;
		end;
	end;
end;

function TApplication.GetCommandCode( ACommand : string ) : word;
var
	I : integer;
	S : String;
begin
	ACommand := UCase(ACommand);
	GetCommandCode := StrInt(ACommand);
	for I := 0 to DefaultCommandNamesMax do begin
		S := UCase(Copy(DefaultCommandNames[I],1,Pos('=', DefaultCommandNames[I]) - 1));
		if S = ACommand then begin
			GetCommandCode := StrInt(Copy(DefaultCommandNames[I], Pos('=', DefaultCommandNames[I])+1, Length(ACommand)+4 ));
			Break;
		end;
	end;
end;

procedure TApplication.SetFocused(AControl : PControl);
begin
	if AControl = FFocused then exit;
	FFocused := AControl;
end;

function TApplication.GetFocused : PControl;
begin
	GetFocused := FFocused;
end;

procedure TApplication.ApplyConfig( AConfig : PConfigFile );
begin
	inherited ApplyConfig(AConfig);
	FClrScr := StrBool(AConfig^.GetValue('CLEAR', BoolStr(FClrScr)));
	FClearWindow := FClrScr;
	FUseFader := StrBool(AConfig^.GetValue('USE_FADER', BoolStr(FUseFader)));
	FUseDAC := StrBool(AConfig^.GetValue('USE_DAC', BoolStr(FUseDAC)));
	FUseSound := StrBool(AConfig^.GetValue('USE_SOUND', BoolStr(FUseSound)));
	if Not FUseSound then SpeakerOff;
end;

procedure TApplication.ApplyTheme( AConfig : PConfigFile );
var
	Pal : TPalette;
	Dac : TDac;
	I	: integer;
	RGB : TRGB;
	IRGB : record
		R, G, B : integer;
	end;
{$IFNDEF TEMPLATES}
	Flag : boolean;
{$ENDIF}
begin
	inherited ApplyTheme(AConfig);
	FStartTone := AConfig^.GetValue('TONE.START', FStartTone);
	FEndTone := AConfig^.GetValue('TONE.END', FEndTone);
	ReadDac(0, 256, Dac);
	GetPalette(Pal);
	{$IFDEF TEMPLATES}
		for I := 0 to 15 do begin
			RGB := Dac[Pal.Palette[I]];
			IRGB.R := RGB.R;
			IRGB.G := RGB.G;
			IRGB.B := RGB.B;
			AConfig^.SetInts('PALETTE.' + HexStr(I), IRGB, 3);
		end;
	{$ELSE}
		Flag := False;
		for I := 0 to 15 do begin
			RGB := Dac[Pal.Palette[I]];
			IRGB.R := RGB.R;
			IRGB.G := RGB.G;
			IRGB.B := RGB.B;
			AConfig^.GetInts('PALETTE.' + HexStr(I), IRGB, 3);
			RGB.R := IRGB.R and $3F;
			RGB.G := IRGB.G and $3F;
			RGB.B := IRGB.B and $3F;
			if (RGB.R <> Dac[Pal.Palette[I]].R) or (RGB.G <> Dac[Pal.Palette[I]].G)
			or (RGB.B <> Dac[Pal.Palette[I]].B) then begin
				Dac[Pal.Palette[I]] := RGB;
				Flag := True;
			end;
		end;
		if Flag then begin
			if not Assigned(FUserDAC) then
				GetMem(FUserDAC, Sizeof(TDac));
			Move(DAC, FUserDAC^, Sizeof(TDac));
		end;
	{$ENDIF}
end;

function TApplication.GetKeyName( ACode : TKeyValue ) : String;
begin
	GetKeyName := ''; { Later Maybe }
end;

{$IFDEF TEMPLATES}
	procedure TApplication.OnDisk(AObject : PObject);
	var
		N : PListItem;
		Section : String;
	begiN
		{$IFDEF ONDISK}
			N := PListItem(AObject)^.First;
			while Assigned(N) do begin
				OnDisk(N);
				N := N^.Next;
			end;
			Section := AObject^.GetPathId;
			{WriteOnDisk(FConfig, Section);}
			{WriteOnDisk(FLanguage, Section);}
			WriteOnDisk(FTheme, Section);
			{WriteOnDisk(FKeyboard, Section);}
		{$ENDIF}
	end;

	procedure TApplication.WriteOnDisk(AConfig : TConfigFile; Section : String);
	var
		F : TFile;
		S,O : String;
		P, J, K : PListItem;
	begin
		if Assigned(AConfig.FSectionKey) then O := AConfig.FSectionKey^.GetNameID else O := '';
		K := AConfig.FList.FindID(Section);
		if Assigned(K) then begin
			F.Create;
			F.Assign(Dir(PathOf(AConfig.FFileName)) + 'ONDISK.' + FileExtension(ACOnfig.FFileName));
			F.Reset;
			if F.IOResult <> 0 then begin
				F.Close;
				F.Rewrite;
			end;
			F.SeekEOF;
			S := '[' + K^.GetNameID + ']' + #$0d#$0a;
			{WriteLn(Copy(S, 1, Length(S) - 2));  }
			F.Write(S);
			P := K^.First;
			while assigned(P) do begin
				J := P^.First;
				if Assigned(J) then begin
					S := #9 + P^.GetNameID + '=' + J^.GetNameID + #$0d#$0a;
					{WriteLn(Copy(S, 1, Length(S) - 2));  }
					F.Write(S);
					J := J^.Next;
					while Assigned(J) do begin
						S := #9#9 + '+=' + J^.GetNameID + #$0d#$0a;
						{WriteLn(Copy(S, 1, Length(S) - 2));  }
						F.Write(S);
						J := J^.Next;
					end;
				end;
				P := P^.Next;
			end;
			K^.Clear;
			if (O <> '') and (O <> Section) then begin
				O := '[' + O + ']' + #$0d#$0a;
				F.Write(O);
			end;
			F.Close;
			F.Destroy;
		end;
	end;
	{$ENDIF}

{$ENDIF}
