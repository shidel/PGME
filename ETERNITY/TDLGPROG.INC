{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgProg --- }

{$IFDEF INTERFACE}
const
	class_TDlgProg = 'TDlgProg';

type 
	PDlgLaunchFwd =^TDlgLaunchFwd;
	TDlgLaunchFwd = object(TDialog)
		procedure SetData(ADlg : PDialog); virtual;
	end;
	
	PDlgProg = ^TDlgProg;
	TDlgProg = object (TDIALOG)
    public { protected }	
    		FEditTitle, FEditTones : PEdit;
    		FMaxAvail, FReturn : PToggle;
    		FOK : PButton;
    		FFile, FClass : String;
    		FSimple, FPGMVars : boolean;
    		FCommands : TListItem;
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl; AName : String; ANewProg : boolean);
		destructor Destroy; virtual;
		procedure SetData(AItem : PListItem); virtual;
		procedure UpdateData; virtual;
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
procedure TDlgLaunchFwd.SetData;
begin
end;

function TDlgProg.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgProg) then 
		ObjectClass := class_TDlgProg
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgProg.Create;
const
	ColorLabel = $70;
	ColorEdit = $1E;
	ColorFile = $71;
	ColorOK = $2E;
	ColorToggle = $0F;
	ColorAccept = $74;
	ColorLaunch = $3E;
var
	B, T : TBounds;
begin	
	inherited Create(AParent, AName);
	Bounds(1,1,55,15, B);
	FEditTones := nil;
	if AParent^.Height > 18 then begin
		Inc(B.Height, 2);
	end;
	SetBounds(B);					
	FAutoEndModal := True;
	SetBounds(B);
	Bounds(3,2,47,1, B);		
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'TITLE.LABEL')))^ do begin
		SetBounds(B);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		SetCaption('Program Title:');
	end;
	Inc(B.Top);
	TextAttr := ColorEdit;
	FEditTitle := New(PEdit, Create(@Self, 'TITLE.EDIT'));
	with PEdit(FEditTitle)^ do begin
		SetHelpText('Use the "Program Title" field to enter the full title of');
		AddHelpText('the programs menu title.');
		SetBounds(B);
		SetAnchors(abLeft or abRight or abTop);
		SetCaption('');
	end;
	Inc(B.Top, 2);
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'FREE.LABEL')))^ do begin
		T := B;
		T.Width := T.Width - 12;
		SetBounds(T);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		SetCaption('Free all memory:');
	end;
	TextAttr := ColorToggle;
	FMaxAvail := New(PToggle, Create(@Self, 'FREE.TOGGLE'));
	with PToggle(FMaxAvail)^ do begin
		SetHelpText('The "Free all memory" toggle will inform PGME to free as much memory');
		AddHelpText('as possible before launching this menu item. This will free up about');
		AddHelpText('10k more of ram for the program. It is probably never really needed,');
		AddHelpText('but is provided anyway. Please note, that this happens automatically');
		AddHelpText('for programs that multiple programs in its execution process. It');
		AddHelpText('will also occur for any menu entry that includes custom DOS commands.');
		T.Left := T.Left + T.Width;
		T.Width := 12;
		SetBounds(T);
		SetAnchors(abRight or abTop);
	end;
	Inc(B.Top, 2);
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'RETURN.LABEL')))^ do begin
		T := B;
		T.Width := T.Width - 12;
		SetBounds(T);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		SetCaption('Return to PGM Eternity:');
	end;
	TextAttr := ColorToggle;
	FReturn := New(PToggle, Create(@Self, 'RETURN.TOGGLE'));
	with PToggle(FReturn)^ do begin
		SetHelpText('The "Return to PGM Eternity" toggle is rather straight forward.');
		AddHelpText('After the menu entry has completed running, the "ON" setting');
		AddHelpText('will return you to PGME. If it is "OFF", you will return to the');
		AddHelpText('command line.');
		T.Left := T.Left + T.Width;
		T.Width := B.Width + B.Left - T.Left;
		SetBounds(T);
		SetAnchors(abRight or abTop);
		SetOn(True);
	end;
	
	Inc(B.Top, 2);
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'LAUNCH.LABEL')))^ do begin
		T := B;
		T.Width := T.Width - 14;
		SetBounds(T);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		if ANewProg then 
			SetCaption('Select program launch process:')
		else
			SetCaption('Change program launch process:');		
	end;
	TextAttr := ColorLaunch;
	with PButton(New(PButton, Create(@Self, 'LAUNCH.BUTTON')))^ do begin
		if ANewProg then 
			SetHelpText('Use the "Select" program launch process button to configure')
		else
			SetHelpText('Use the "Modify" program launch process button to alter');
		AddHelpText('the program(s) you wish to execute when PGME');
		AddHelpText('launches this programs menu item.');
		T.Left := T.Left + T.Width;
		T.Width := B.Width + B.Left - T.Left;
		SetBounds(T);
		SetAnchors(abRight or abTop);
		if ANewProg then 
			SetCaption('Select')
		else
			SetCaption('Modify');
		SetCommand(cmBrowseExec)
	end;
	
	if Height > 15 then begin
		Inc(B.Top, 2);
		TextAttr := ColorLabel;
		with PLabel(New(PLabel, Create(@Self, 'TONE.LABEL')))^ do begin
			T := B;
			T.Width := 14;
			SetBounds(T);
			SetAnchors(abLeft or abRight or abTop);
			FHAlign := AlignLeft;
			SetCaption('Launch Tones:')
		end;
		TextAttr := ColorEdit;
		FEditTones := New(PEdit, Create(@Self, 'TONE.EDIT'));
		with PEdit(FEditTones)^ do begin
			SetHelpText('Use the "Launch Tones" field to set the tones played when');
			AddHelpText('this menu entry is launched. If this is left blank, PGME');
			AddHelpText('will just play the default launching tones.');
			AddHelpText('Custom tones are "Frequency:Duration[,...]" The');
			AddHelpText('duration is value of 0 or larger and is 1/18th of a second');
			AddHelpText('(or about 1000 bpm). If you wish this entry to make no');
			AddHelpText('sound, just use "0:0" (No Frequency:No Duration) as its tone.');
			T.Left := T.Left + T.Width;
			T.Width := B.Width + B.Left - T.Left;
			SetBounds(T);
			SetAnchors(abRight or abTop);
		end;
	end;

	TextAttr := ColorOK;
	Inc(B.Top, 2);
	FOK := New(PButton, Create(@Self, 'BUTTON.OK'));
	with PButton(FOK)^ do begin
		B.Left := B.Left + B.Width div 2 - 6;
		B.Width := 14;
		SetBounds(B);
		SetAnchors(abLeft or abRight or abTop);
		if ANewProg then 
			SetCaption('Create')
		else
			SetCaption('Update');
		SetCommand(cmOK);
	end;
	FCommands.Create('COMMANDS');
end;

destructor TDlgProg.Destroy;
begin
	FCommands.Destroy;
	inherited Destroy;
end;

procedure TDlgProg.CheckLocal(var AEvent : TEvent);
var 
	P : PDialog;
begin
	inherited CheckLocal(AEvent);
	if AEvent.What = evCommand then 
		case AEvent.Command of
			cmEdit : if Assigned(FEditTones) and (AEvent.InfoPtr = FEditTones) then begin
				ClearEvent(AEvent);
				SpeakerPlay(FEditTones^.GetCaption);
			end;
			cmBrowseExec : begin
				ClearEvent(AEvent);
				P := PDialog(PGME.GetDlgLaunch);
				PDlgLaunchFwd(P)^.SetData(@Self);
				PGME.FDlgLaunch^.ShowModal;
				UpdateData;
			end;
		end;
end;

procedure TDlgProg.BeforeShow;
begin
	inherited BeforeShow;
	UpdateData;
end;

procedure TDlgProg.SetData(AItem : PListItem);
var
	P : PListItem;
begin
	FCommands.Clear;
	FPGMVars := False;
	FSimple := True;
	FClass := '';
	FFile := '';
	FEditTitle^.SetCaption('');
	FMaxAvail^.SetOn(False);
	FReturn^.SetOn(True);
	{ PGMVARS, SIMPLE, TONE, GROUP, FILENAME, CLASS }
	if Not Assigned(AItem) then begin
		if Assigned(PGME.FMenuCache) then 
			FEditTitle^.SetCaption(PMenuCache(PGME.FMenuCache)^.FNoNameStr);
	end else with PCacheApp(AItem^.Data)^ do begin
		FEditTitle^.SetCaption(Title);	
		FMaxAvail^.SetOn(MaxMemory);
		FReturn^.SetOn(ReturnAfter);
		FFile := FileName;
		FClass := ClassName;
		FPGMVars := PGMVars;
		P := PCacheApp(AItem^.Data)^.First;
		while Assigned(P) do begin
			FCommands.Add(New(PListItem, Create(P^.GetNameID)));
			P := P^.Next;
		end;
		if Assigned(FEditTones) then
			FEditTones^.SetCaption(Tone);	
	end;
	UpdateData;
end;

procedure TDlgProg.UpdateData;
var
	S : String;
begin
	FSimple := (Not FMaxAvail^.GetOn) and (FCommands.Count = 0);
	
	S := UCase(FileExtension(FFile));
	if (S = 'EXE') or (S = 'COM') then 
		FClass := 'PROGRAM'
	else if (S = 'SCR') then 
		FClass := 'ESAVER'
	else if (S = 'TXT') or (S = 'DOC') or (S = 'ME') then 
		FClass := 'DOCUMENT'
	else begin
		FClass := 'OTHER';
		FSimple := False;
	end;
	FOK^.SetEnabled(Trim(FFile) <> '');
end;

{$ENDIF}
