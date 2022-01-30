{ Copyright 2015-2022 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ --- TDlgPGMCfg --- }

{$IFDEF INTERFACE}
const
	class_TDlgPGMCfg = 'TDlgPGMCfg';

type
	PDlgPGMCfg = ^TDlgPGMCfg;
	TDlgPGMCfg = object (TDialog)
    public { protected }
    	FSound : PToggle;
    	FLang : PDropList;
        FIdle : PDropList;
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure BeforeShow; virtual;
		procedure Save; virtual;
	public
		constructor Create(AParent : PControl);
		function Modified: boolean; virtual;
	end;

{$ENDIF}

{$IFDEF IMPLEMENTATION}

const
    cPowerModeID : array[0..3] of string = (
        'OFF', 'HALT', 'BIOS', 'ANY');
    cPowerMode : array[0..3] of string = (
        '<disabled>', 'CPU Halt', 'BIOS APM', 'ANY');

function TDlgPGMCfg.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TDlgPGMCfg) then
		ObjectClass := class_TDlgPGMCfg
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgPGMCfg.Create;
const
	ColorLabel = $70;
	ColorDrop  = $1E;
	ColorToggle = $0F;
	ColorOK    = $2E;
var
	B, E, T : TBounds;
	SRec : TSearchRec;
	I : integer;
begin
	inherited Create(AParent, 'DIALOG.CONFIG');
	{ if PControl(Parent)^.Height <= 16 then
		SetShadow(False); }
	FAutoEndModal := True;
	Bounds(1,1,56,12, B);
	SetBounds(B);
	SetTitle('Eternity Config Settings');
	SetHelpText('The Program Manager Eternity program configuration settings dialog box.');
	AddHelpText('');
	AddHelpText('This is where you can change the language and some other');
	AddHelpText('settings for PGME.');
	TextAttr := ColorLabel;
	Bounds(3,2, 15, 1, B);
	Bounds(B.Left + B.Width + 1, B.Top, Self.Width - B.Left - B.Width - 4, 10, E);
	if GetShadow then Dec(E.Width, 2);
	if not GetShadow then Dec(E.Height, 2);
	with PLabel(New(PLabel, Create(@Self, 'LANGUAGE.LABEL')))^ do begin
		SetBounds(B);
		FHAlign := AlignRight;
		SetCaption('Language File:');
	end;
	TextAttr := ColorDrop;
	FLang := New(PDropList, Create(@Self, 'LANGUAGE.DROP'));
	with PDropList(FLang)^ do begin
		SetHelpText('Use the "Language File" field to select the desired language');
		AddHelpText('for PGME to use. Please note that the program will always');
		AddHelpText('fallback to English for itself and menus when the requested');
		AddHelpText('language is not available.');
		SetBounds(E);
		SetReadOnly(True);
		AddItem(Application^.FFileNone);
		FindFirst(GetExePath + '*' + LanguageExt, faAnyFile, SRec);
		while (DosError = 0) do begin
			if SRec.Attr and faDirectory <> faDirectory then
				AddItem(SRec.Name);
			FindNext(SRec);
		end;
		{ SetCaption(Application^.FFileNone); }
	end;
	Bounds(4, B.Top + 2, Self.Width - 9, 1, B);
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'SOUND.LABEL')))^ do begin
		T := B;
		T.Width := T.Width - 12;
		SetBounds(T);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		SetCaption('Sound Effects:');
	end;
	TextAttr := ColorToggle;
	FSound := New(PToggle, Create(@Self, 'SOUND.TOGGLE'));
	with PToggle(FSound)^ do begin
		SetHelpText('Use the "Sound Effects" toggle switch to enable and disable the');
		AddHelpText('musical notes played by PGME when launching a program, at startup, ');
		AddHelpText('at shutdown, etc.');
		T.Left := T.Left + T.Width;
		T.Width := 12;
		SetBounds(T);
		SetAnchors(abRight or abTop);
	end;

	TextAttr := ColorLabel;
	Inc(B.Top, 2);
	T := B;
	T.Width := 15;
	Bounds(T.Left + T.Width, T.Top, Self.Width - T.Left - T.Width - 5, 8, E);
	with PLabel(New(PLabel, Create(@Self, 'IDLE.LABEL')))^ do begin
		SetBounds(T);
		FHAlign := AlignLeft;
		SetCaption('Power Saving:');
	end;
	TextAttr := ColorDrop;
	FIdle := New(PDropList, Create(@Self, 'IDLE.DROP'));
	with PDropList(FIdle)^ do begin
		SetHelpText('Use the "Power Saving" field to select the desired method');
		AddHelpText('for PGME to use to conserve power while idle. This takes');
		AddHelpText('effect immediately on "OK" and may cause the system to freeze.');
		AddHelpText('However, the change is not saved until after the power setting');
		AddHelpText('has been activated. If it is unable to set the request power');
		AddHelpText('saving mode, it may downgrade or disable the setting.');
		SetBounds(E);
		SetReadOnly(True);
		Application^.FLanguage.OpenSection('STRINGS');
		for I := low(cPowerMode) to high(cPowerMode) do begin
	    	cPowerMode[I]:=Application^.FLanguage.GetValue(GetPathID + '.ITEM.'
	    	    + cPowerModeID[I],cPowerMode[I]);
    		AddItem(cPowerMode[I]);
    	end;
		{SetCaption(Application^.FFileNone);}
	end;

	Inc(B.Top, 2);
	B.Width := 12;
	B.Left := Width div 2 - B.Width div 2 + 2;
	if GetShadow then Dec(B.Left, 2);
	TextAttr := ColorOK;
	with PButton(New(PButton, Create(@Self, 'BUTTON.OK')))^ do begin
		SetHelpText('5;Use the "%0" button to apply any changes and close the');
		AddHelpText('dialog box.');
		SetAnchors(abBottom);
		SetBounds(B);
		SetCaption('OK');
		SetAllowedOutside(True);
		SetCommand(cmOK);
	end;

end;

procedure TDlgPGMCfg.BeforeShow;
var
    DosLang, TL : string;
    S : String;
    I : integer;
begin
	inherited BeforeShow;
	with Application^.FConfig do begin
        DosLang := UCase(Trim(GetEnv('LANG')));
        TL := ''; {'DEFAULT' + LanguageExt;}
        if DosLang <> '' then
            OpenSection('LANG.' + DosLang)
        else
            OpenSection('LANG.NULL');
        TL := GetValue('LANGUAGE', TL);
        OpenSection('*');
        TL := GetValue('LANGUAGE', TL);
		FLang^.SetCaption(IsStr(TL, '', Application^.FFileNone));
		OpenSection('PGM');
        I := StrIndex(GetValue('CPU_IDLE', cPowerModeID[IdleMethod]), cPowerModeID);
        if I < 0 then I := 0;
        FIdle^.SetCaption(cPowerMode[I]);
		S := UCase(Trim(GetValue('USE_SOUND', 'TRUE')));
		FSound^.SetOn(( S = 'TRUE') or (S='ON') or (S='ENABLED'));
	end;
end;

procedure TDlgPGMCfg.Save;
var
    I : integer;
begin
	with Application^.FConfig do begin
		OpenSection('*');
		SetValue('LANGUAGE', IsStr(FLang^.GetCaption, Application^.FFileNone, ''));
		OpenSection('PGM');
        I := StrIndex(Trim(UCase(FIdle^.GetCaption)), cPowerMode);
        if I < 0 then I := 0;
		if I = High(cPowerModeID) then I := imAny;
		IdleMethod := I;
		for I := 0 to 9 do IdleProc;
		I := IdleMethod;
		SetValue('CPU_IDLE', cPowerModeID[I]);

		Application^.FUseSound := FSound^.GetOn;
		if FSound^.GetOn then
		    SetValue('USE_SOUND', 'TRUE')
		else
		    SetValue('USE_SOUND', 'FALSE');
		Write;
	end;
end;
function TDlgPGMCfg.Modified;
var
    DosLang, TL : string;
    X : Boolean;
    I : integer;
begin
	X := False;
	with Application^.FConfig do begin
        DosLang := UCase(Trim(GetEnv('LANG')));
        TL := ''; {'DEFAULT' + LanguageExt;}
        if DosLang <> '' then
            OpenSection('LANG.' + DosLang)
        else
            OpenSection('LANG.NULL');
        TL := GetValue('LANGUAGE', TL);
        OpenSection('*');
        TL := GetValue('LANGUAGE', TL);
		OpenSection('*');
		X := X or (IsStr(TL, '', Application^.FFileNone)
		    <> Trim(FLang^.GetCaption));
        I := StrIndex(Trim(UCase(FIdle^.GetCaption)), cPowerMode);
        if I < 0 then I := 0;
        X := X or (cPowerModeID[I] <> GetValue('CPU_IDLE', cPowerModeID[0]));
        X := X or (FSound^.GetOn <> Application^.FUseSound);
	end;
	Modified := X;
end;
{$ENDIF}
