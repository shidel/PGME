{ Copyright 2015 Jerome Shidel }

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
	TDlgPGMCfg = object (TDIALOG)
    public { protected }
    	FTimeout : PLabel;
    	FLang : PDropList;
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
		procedure Save; virtual;
	public
		constructor Create(AParent : PControl);
		destructor Destroy; virtual;
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
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
	ColorOK    = $2E;
var
	B, E : TBounds;
	SRec : TSearchRec;
begin	
	inherited Create(AParent, 'DIALOG.CONFIG');
	FAutoEndModal := True;
	if PControl(Parent)^.Height <= 16 then 
		SetShadow(False);
	Bounds(1,1,56,8, B);
	SetBounds(B);					
	SetTitle('Eternity Config Settings');
	SetHelpText('The Program Manager Eternity program config settings dialog box.');
	AddHelpText('This is where you can change the language and other');
	AddHelpText('settings of PGME.');
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
		SetBounds(E);
		SetCaption('DEFAULT.LNG');
		FindFirst(GetExePath + '*' + LanguageExt, faAnyFile, SRec);
		while (DosError = 0) do begin
			if SRec.Attr and faDirectory <> faDirectory then
				AddItem(SRec.Name);
			FindNext(SRec);
		end;
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
		SetCommand(cmOK);
	end;
	
end;

destructor TDlgPGMCfg.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgPGMCfg.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgPGMCfg.BeforeShow;
begin
	inherited BeforeShow;
	with Application^.FConfig do begin
		OpenSection('*');
		FLang^.SetCaption(GetValue('LANGUAGE', 'DEFAULT.LNG'));
	end;
end;

procedure TDlgPGMCfg.Save;
begin
	with Application^.FConfig do begin
		OpenSection('*');
		SetValue('LANGUAGE', FLang^.GetCaption);
		Write;
	end;
end;
{$ENDIF}
