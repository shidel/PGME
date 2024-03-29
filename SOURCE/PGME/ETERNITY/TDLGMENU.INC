{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ --- TDlgMenu --- }

{$IFDEF INTERFACE}
const
	class_TDlgMenu = 'TDlgMenu';

type
	PDlgMenu = ^TDlgMenu;
	TDlgMenu = object (TDIALOG)
    public { protected }
			FPath : String;
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
    	FEditLong, FEditShort : PEdit;
    	FLabelFile : PLabel;
    	FLockToggle : PToggle;
    	FLockAccept : PCheckBox;
    	FOK : PButton;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl; AName : String; ANewMenu : boolean);
		procedure SetData(AItem : PListItem); virtual;
		procedure UpdateFileName; virtual;
	end;

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgMenu.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TDlgMenu) then
		ObjectClass := class_TDlgMenu
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgMenu.Create;
const
	ColorLabel = $70;
	ColorEdit = $1E;
	ColorFile = $71;
	ColorOK = $2E;
	ColorToggle = $0F;
	ColorAccept = $74;
var
	B, T : TBounds;
begin
	inherited Create(AParent, AName);
	FAutoEndModal := True;
	Bounds(1,1,55,15, B);
	if Not ANewMenu then begin
		if AParent^.Height < 16 then
			SetShadow(False)
		else
			Inc(B.Height);
	end;
	SetBounds(B);
	Bounds(3,2,47,1, B);
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'LONG.LABEL')))^ do begin
		SetBounds(B);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		SetCaption('Long menu title description:');
	end;
	Inc(B.Top);
	TextAttr := ColorEdit;
	FEditLong := New(PEdit, Create(@Self, 'LONG.EDIT'));
	with PEdit(FEditLong)^ do begin
		SetHelpText('Use the "Long menu title description" to enter the full title of');
		AddHelpText('the menu.');
		SetBounds(B);
		SetAnchors(abLeft or abRight or abTop);
		SetCaption('');
	end;
	Inc(B.Top, 2);
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'SHORT.LABEL')))^ do begin
		SetBounds(B);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		SetCaption('Short menu name:');
	end;
	Inc(B.Top);
	TextAttr := ColorEdit;
	FEditShort := New(PEdit, Create(@Self, 'SHORT.EDIT'));
	with PEdit(FEditShort)^ do begin
		SetHelpText('Use the "Short menu name" to enter the short version of the title for');
		AddHelpText('the menu.');
		SetBounds(B);
		SetAnchors(abLeft or abRight or abTop);
		SetCaption('');
	end;
	Inc(B.Top, 2);
	if ANewMenu then begin
		TextAttr := ColorLabel;
		with PLabel(New(PLabel, Create(@Self, 'FILE.LABEL')))^ do begin
			SetBounds(B);
			SetAnchors(abLeft or abRight or abTop);
			FHAlign := AlignLeft;
			SetCaption('Menu filename:');
		end;
		Inc(B.Top);
		TextAttr := ColorFile;
		FLabelFile := New(PLabel, Create(@Self, 'FILE.NAME'));
		with PLabel(FLabelFile)^ do begin
			SetHelpText('The "Menu filename" is readonly and displays the path and filename');
			AddHelpText('of the new menu that PGME will create.');
			SetBounds(B);
			SetAnchors(abLeft or abRight or abTop);
			FHAlign := AlignLeft;
			SetCaption('');
		end;
		FLockToggle := nil;
		FLockAccept := nil;
	end else begin
		FLabelFile := nil;
		TextAttr := ColorLabel;
		T := B;
		T.Width := B.Width - 12;
		with PLabel(New(PLabel, Create(@Self, 'LOCK.LABEL')))^ do begin
			SetBounds(T);
			SetAnchors(abLeft or abRight or abTop);
			FHAlign := AlignLeft;
			SetCaption('Lock menu and prevent changes:');
		end;
		TextAttr := ColorToggle;
		FLockToggle := New(PToggle, Create(@Self, 'LOCK.TOGGLE'));
		with FLockToggle^ do begin
			SetHelpText('The "Lock Menu" toggle will change the selected menu to readonly by');
			AddHelpText('set its writable flag to false. When this flag is set, you will');
			AddHelpText('not be able to add or remove programs on the menu or delete the menu');
			AddHelpText('from within PGME. You will not even be able to turn the flag back');
			AddHelpText('off or even open this dialog box for the menu. The only way to make');
			AddHelpText('the menu writable again is to manually edit the menu file and');
			AddHelpText('enable its writable flag using a text editor.');
			T.Left := T.Left + T.Width;
			T.Width := B.Width - T.Width;
			SetBounds(T);
			SetAnchors(abRight or abTop);
		end;
		Inc(B.Top, 2);
		TextAttr := ColorAccept;
		FLockAccept := New(PCheckBox, Create(@Self, 'LOCK.ACCEPT'));
		with FLockAccept^ do begin
			SetHelpText('You must also check the "Lock Accept" check box to make the');
			AddHelpText('menu readonly.');
			T := B;
			{Inc(T.Left, 4);
			Dec(T.Width, T.Left - B.Left);}
			SetBounds(T);
			SetAnchors(abRight or abTop or abLeft);
			SetEnabled(False);
			SetCaption('Really. Permanently lock menu.');
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
		if ANewMenu then
			SetCaption('Create')
		else
			SetCaption('Update');
		SetCommand(cmOK);

	end;
end;

procedure TDlgMenu.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmToggle : if Assigned(FLockToggle) and (AEvent.InfoPtr = FLockToggle) then begin
				FLockAccept^.SetEnabled(FLockToggle^.GetOn);
				if not FLockToggle^.GetOn then
					FLockAccept^.SetState(0);
			end;
		else
			UpdateFileName;
		end;
end;

procedure TDlgMenu.BeforeShow;
begin
	inherited BeforeShow;
	FPath := '';
	Application^.FConfig.OpenSection(GetPathID);
	FPath := Application^.FConfig.GetValue('PATH.MENU', '');
	FPath := PullStr(';', FPath);
	if FPath = '' then
		FPath := GetExePath;
	UpdateFileName;
end;

procedure TDlgMenu.SetData(AItem : PListItem);
begin
	FEditLong^.SetCaption('');
	FEditShort^.SetCaption('');
	if Not Assigned(AItem) then begin
		if Assigned(PGME.FMenuCache) then begin
			FEditLong^.SetCaption(PMenuCache(PGME.FMenuCache)^.FNoDescStr);
			FEditShort^.SetCaption(PMenuCache(PGME.FMenuCache)^.FNoTitleStr);
		end;
	end else with PCacheMenu(AItem^.Data)^ do begin
		FEditLong^.SetCaption(LongTitle);
		FEditShort^.SetCaption(ShortTitle);
		if Assigned(FLabelFile) then
			FLabelFile^.SetCaption(PCacheDir(Parent)^.GetNameID + GetNameID);
	end;
	UpdateFileName;
	if Assigned(FLockToggle) then FLockToggle^.SetOn(False);
	if Assigned(FLockAccept) then FLockAccept^.SetState(0);
	if Assigned(FLockAccept) then FLockAccept^.SetEnabled(False);
end;

procedure TDlgMenu.UpdateFileName;
const
	Vowels : String = 'yuoiea';
var
	I : Integer;
	K : LongInt;
	S, T : String;
	F, P : boolean;
begin
	if FEditLong^.GetCaption <> PMenuCache(PGME.FMenuCache)^.FNoDescStr then
		if FEditShort^.GetCaption = PMenuCache(PGME.FMenuCache)^.FNoTitleStr then begin
			S := FEditLong^.GetCaption;
			if Pos(' ', S) < 5 then
				Delete(S, 1, Pos(' ', S));
			S := WCase(Trim(S));
			if S <> '' then
				FEditShort^.SetCaption(S);
		end;
	if Not Assigned(FLabelFile) then exit;
	S := OnlyAlphaNum(WCase(FEditShort^.GetCaption));
	if S = '' then
		OnlyAlphaNum(WCase(FEditLong^.GetCaption));

	S := ReduceStr(S, 8);

	if S = '' then S := '00000000';

	S := Copy(UCase(S), 1, 8);
	if FileExists(FPath + S + EMenuExt) then begin
		K := 0;
		repeat
			T := '-' + HexStr(K);
			T := Copy(S, 1, 8 - Length(T)) + T;
			Inc(K);
		until not FileExists(FPath + T + EMenuExt);
		S := T;
	end;

	if FLabelFile^.GetCaption <> FPath + S + EMenuExt then
		FLabelFile^.SetCaption(FPath + S + EMenuExt);
end;
{$ENDIF}
