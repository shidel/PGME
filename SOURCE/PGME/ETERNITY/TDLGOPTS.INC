{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ --- TDlgPGMOpt --- }

{$IFDEF INTERFACE}
const
	class_TDlgPGMOpt = 'TDlgPGMOpt';

type
	PDlgPGMOpt = ^TDlgPGMOpt;
	TDlgPGMOpt = object (TDIALOG)
    public { protected }
    	FTimeout : PLabel;
    	FSaver, FTheme, FFont : PDropList;
    	FSlide : PSlider;
    	FDAC, FFader : PCheckbox;
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
		procedure Save; virtual;
	public
		constructor Create(AParent : PControl);
		function Modified : boolean; virtual;
	end;

{$ENDIF}

{$IFDEF IMPLEMENTATION}
procedure FormatTimeOut (AVisual : PControl; var AValue : String); far;
var
	FD : array[0..1] of TFormatData;
begin
	if not Assigned(AVisual) then exit;
	if AVisual = PControl(PDlgPGMOpt(PGME.FDlgPGMOpt)^.FTimeout) then begin
		FD[0].ID := FmtInteger;
		FD[0].IntegerValue := PDlgPGMOpt(PGME.FDlgPGMOpt)^.FSlide^.GetValue;
		FD[1].ID := FmtInteger;
		if FD[0].IntegerValue = 0 then begin
			FD[0].IntegerValue := 30;
			FD[1].IntegerValue := 0;
		end else begin
			FD[1].IntegerValue := 1;
		end;
		AValue := FormatStr(PDlgPGMOpt(PGME.FDlgPGMOpt)^.FTimeout^.GetCaption, FD, 2);
	end;
end;

function TDlgPGMOpt.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TDlgPGMOpt) then
		ObjectClass := class_TDlgPGMOpt
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgPGMOpt.Create;
const
	ColorLabel = $70;
	ColorDrop  = $1E;
	ColorSlide = $0F;
	ColorOK    = $2E;
var
	B, E, D : TBounds;
	SRec : TSearchRec;
begin
	inherited Create(AParent, 'DIALOG.OPTIONS');
	FAutoEndModal := True;
	if PControl(Parent)^.Height <= 16 then
		SetShadow(False);
	Bounds(1,1,56,17, B);
	SetBounds(B);
	SetTitle('Eternity Display Options');
	SetHelpText('The Program Manager Eternity program display options dialog box.');
	AddHelpText('This is where you can make changes to the behaviour and');
	AddHelpText('appearance of PGME.');
	TextAttr := ColorLabel;
	Bounds(3,2, 15, 1, B);
	Bounds(B.Left + B.Width + 1, B.Top, Self.Width - B.Left - B.Width - 4, 1, E);
	if GetShadow then Dec(E.Width, 2);
	D := E;
	D.Height := 10;
	if not GetShadow then Dec(D.Height, 2);
	with PLabel(New(PLabel, Create(@Self, 'THEME.LABEL')))^ do begin
		SetBounds(B);
		FHAlign := AlignRight;
		SetCaption('Theme File:');
	end;
	TextAttr := ColorDrop;
	FTheme := New(PDropList, Create(@Self, 'THEME.DROP'));
	with PDropList(FTheme)^ do begin
		SetHelpText('Use the "Theme File" drop down list to select the theme you wish');
		AddHelpText('PGME to use.');
		SetBounds(D);
		FindFirst(GetExePath + '*' + ThemeExt, faAnyFile, SRec);
		while (DosError = 0) do begin
			if SRec.Attr and faDirectory <> faDirectory then
				AddItem(SRec.Name);
			FindNext(SRec);
		end;
		SetCaption('DEFAULT.THM');
	end;
	Inc(B.Top);
	Inc(B.Top);
	TextAttr := ColorLabel;
	FDAC := New(PCheckbox, Create(@Self, 'DAC.CHECK'));
	with PCheckbox(FDAC)^ do begin
		SetHelpText('Enable the "Custom Colors" checkbox to allow PGME to use custom');
		AddHelpText('VGA color definitions in theme files.');
		E.Top := B.Top;
		SetBounds(E);
		SetCaption('Custom Colors');
	end;
	Inc(B.Top);
	TextAttr := ColorLabel;
	FFader := New(PCheckbox, Create(@Self, 'FADER.CHECK'));
	with PCheckbox(FFader)^ do begin
		SetHelpText('Enable the "Screen Fader" checkbox to allow PGME to perform the fade in and');
		AddHelpText('out effects. Please note that "Custom Colors" must also be enabled.');
		E.Top := B.Top;
		SetBounds(E);
		SetCaption('Screen Fader');
	end;
	Inc(B.Top);
	Inc(B.Top);
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'FONT.LABEL')))^ do begin
		SetBounds(B);
		FHAlign := AlignRight;
		SetCaption('Font File:');
	end;
	TextAttr := ColorDrop;
	FFont := New(PDropList, Create(@Self, 'FONT.DROP'));
	with PDropList(FFont)^ do begin
		SetHelpText('Use the "Font File" drop down list to select a custom text font');
		AddHelpText('for PGME to use. If one is specified, PGME will automatically');
		AddHelpText('reset to the system default text font when launching programs');
		AddHelpText('and when exit PGME.');
		D.Top := B.Top;
		SetBounds(D);
		FindFirst(GetExePath + '*' + FontExt, faAnyFile, SRec);
		while (DosError = 0) do begin
			if SRec.Attr and faDirectory <> faDirectory then
				AddItem(SRec.Name);
			FindNext(SRec);
		end;
		SetCaption('DEFAULT.FNT');
	end;
	Inc(B.Top);
	Inc(B.Top);
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'ESAVER.LABEL')))^ do begin
		SetBounds(B);
		FHAlign := AlignRight;
		SetCaption('Screen Saver:');
	end;
	TextAttr := ColorDrop;
	FSaver := New(PDropList, Create(@Self, 'ESAVER.DROP'));
	with PDropList(FSaver)^ do begin
		SetHelpText('Use the "Screen Saver" drop down list to select the desired');
		AddHelpText('screen saver. If one is not specified, PGME will automatically');
		AddHelpText('fall back to the "BLANKER.SCR" screen saver. Then if for some');
		AddHelpText('reason that cannot be found, the built in plain empty screen');
		AddHelpText('saver will be used. The screen saver cannot be turned off.');
		D.Top := B.Top;
		Dec(D.Height,2);
		if Self.GetShadow = false then Dec(D.Height);
		SetBounds(D);
		FindFirst(GetExePath + '*' + ESaverExt, faAnyFile, SRec);
		while (DosError = 0) do begin
			if SRec.Attr and faDirectory <> faDirectory then
				AddItem(SRec.Name);
			FindNext(SRec);
		end;
		SetCaption('DEFAULT.SCR');
	end;
	Inc(B.Top);
	Inc(B.Top);
	TextAttr := ColorLabel;
	FTimeout := New(PLabel, Create(@Self, 'ESAVER.DELAY'));
	with PLabel(FTimeout)^ do begin
		SetBounds(B);
		OnFormat := FormatTimeOut;
		FHAlign := AlignRight;
		SetCaption('0,1{sec,min};Timeout %0 %1:');
	end;
	TextAttr := ColorSlide;
	FSlide := New(PSlider, Create(@Self, 'ESAVER.SLIDE'));
	with PSlider(FSlide)^ do begin
		SetHelpText('Use the "Timeout" slider to select the desired');
		AddHelpText('time that PGME should wait until it automatically launches the');
		AddHelpText('screen saver.');
		E.Top := B.Top;
		SetBounds(E);
		SetMin(0);
		SetMax(60);
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
		setCOmmand(cmOK);
	end;

end;

procedure TDlgPGMOpt.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmScrollBar : if AEvent.InfoPtr = FSlide then begin
				ClearEvent(AEvent);
				FTimeout^.Update;
			end;
			cmCheckbox : if AEvent.InfoPtr = FDAC then begin
				ClearEvent(AEvent);
				FFader^.SetEnabled(FDAC^.Checked);
			end;
		end;
end;

procedure TDlgPGMOpt.BeforeShow;
var
	I : integer;
	S : String;
begin
	inherited BeforeShow;
	with Application^.FConfig do begin
		OpenSection('*');
		S := GetValue('THEME', 'DEFAULT.THM');
		if S = '' then S := '<none>';
		FTheme^.SetCaption(S);
		FFont^.SetCaption(GetValue('FONT', 'DEFAULT.FNT'));
		OpenSection(Application^.GetNameID);
		FSaver^.SetCaption(GetValue('SCREEN_SAVER', 'DEFAULT.SCR'));
		I := 0;
		GetInts('SCREEN_TIMEOUT', I, 1);
		FSlide^.SetValue(I);
		if StrBool(GetValue('USE_DAC', 'TRUE')) = true then
			FDAC^.SetState(1)
		else
			FDAC^.SetState(0);
		if StrBool(GetValue('USE_FADER', 'TRUE')) = true then
			FFader^.SetState(1)
		else
			FFader^.SetState(0);
	end;
	FFader^.SetEnabled(FDAC^.Checked);

end;

procedure TDlgPGMOpt.Save;
var
	I : integer;
	S : String;
begin
	inherited BeforeShow;
	with Application^.FConfig do begin
		OpenSection('*');
		S := FTheme^.GetCaption;
		if S = '<none>' then S := '';
		SetValue('THEME', S);
		SetValue('FONT', FFont^.GetCaption);
		OpenSection(Application^.GetNameID);
		SetValue('SCREEN_SAVER', FSaver^.GetCaption);
		I := FSlide^.GetValue;
		SetInts('SCREEN_TIMEOUT', I, 1);
		SetValue('USE_DAC', BoolStr(FDAC^.Checked));
		SetValue('USE_FADER', BoolStr(FFader^.Checked));
		Write;
	end;
end;

function TDlgPGMOpt.Modified;
var
	I : integer;
	X : boolean;
begin
    X := False;
	with Application^.FConfig do begin
		OpenSection('*');
		X := X or (GetValue('THEME', FTheme^.GetCaption) <> Trim(FTheme^.GetCaption));
		X := X or (GetValue('FONT', FFont^.GetCaption) <> Trim(FFont^.GetCaption));
		OpenSection(Application^.GetNameID);
		X := X or (GetValue('SCREEN_SAVER', FSaver^.GetCaption) <> Trim(FSaver^.GetCaption));
		I := FSlide^.GetValue;
		GetInts('SCREEN_TIMEOUT', I, 1);
		X := X or (I <> FSlide^.GetValue);
		X := X or (StrBool(GetValue('USE_DAC', BoolStr(FDAC^.Checked))) <> FDAC^.Checked);
		X := X or (StrBool(GetValue('USE_FADER', BoolStr(FFader^.Checked))) <> FFader^.Checked);
	end;
	Modified := X;
end;

{$ENDIF}
