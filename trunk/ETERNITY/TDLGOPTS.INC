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
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl);
		destructor Destroy; virtual;
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
		FD[0].IntegerValue := 0;
		FD[1].ID := FmtInteger;
		FD[1].IntegerValue := 1;
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
	with PLabel(New(PLabel, Create(@Self, 'THEME.LABEL')))^ do begin
		SetBounds(B);
		FHAlign := AlignRight;
		SetCaption('Theme File:');
	end;
	TextAttr := ColorDrop;
	with PDropList(New(PDropList, Create(@Self, 'THEME.DROP')))^ do begin
		SetBounds(D);
		SetCaption('DEFAULT.THM');
		FindFirst(GetExePath + '*' + ThemeExt, faAnyFile, SRec);
		while (DosError = 0) do begin
			if SRec.Attr and faDirectory <> faDirectory then
				AddItem(SRec.Name);
			FindNext(SRec);
		end;
	end;
	Inc(B.Top);	
	Inc(B.Top);	
	TextAttr := ColorLabel;
	with PCheckbox(New(PCheckbox, Create(@Self, 'DAC.CHECK')))^ do begin
		E.Top := B.Top;
		SetBounds(E);
		SetCaption('Custom Colors');
	end;
	Inc(B.Top);	
	TextAttr := ColorLabel;
	with PCheckbox(New(PCheckbox, Create(@Self, 'FADER.CHECK')))^ do begin
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
	with PDropList(New(PDropList, Create(@Self, 'FONT.DROP')))^ do begin
		D.Top := B.Top;
		SetBounds(D);
		SetCaption('DEFAULT.FNT');
		FindFirst(GetExePath + '*' + FontExt, faAnyFile, SRec);
		while (DosError = 0) do begin
			if SRec.Attr and faDirectory <> faDirectory then
				AddItem(SRec.Name);
			FindNext(SRec);
		end;
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
	with PDropList(New(PDropList, Create(@Self, 'ESAVER.DROP')))^ do begin
		D.Top := B.Top;
		Dec(D.Height,2);
		if Self.GetShadow = false then Dec(D.Height);
		SetBounds(D);
		SetCaption('DEFAULT.SCR');
		FindFirst(GetExePath + '*' + ESaverExt, faAnyFile, SRec);
		while (DosError = 0) do begin
			if SRec.Attr and faDirectory <> faDirectory then
				AddItem(SRec.Name);
			FindNext(SRec);
		end;
	end;
	Inc(B.Top);	
	Inc(B.Top);	
	TextAttr := ColorLabel;
	FTimeout := New(PLabel, Create(@Self, 'ESAVER.LABEL'));
	with PLabel(FTimeout)^ do begin
		SetBounds(B);
		OnFormat := FormatTimeOut;
		FHAlign := AlignRight;
		SetCaption('0,1{sec,min};Timeout %0 %1:');
	end;
	TextAttr := ColorSlide;
	with PSlider(New(PSlider, Create(@Self, 'ESAVER.SLIDE')))^ do begin
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
		SetAnchors(abBottom);
		SetBounds(B);
		SetCaption('OK');
		SetAllowedOutside(True);
		setCOmmand(cmOK);
	end;
	
end;

destructor TDlgPGMOpt.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgPGMOpt.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgPGMOpt.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
