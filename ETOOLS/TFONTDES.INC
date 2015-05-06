{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

const
	class_TFontDesigner = 'TFontDesigner';

type 
	PETool = ^TETool;
	TETool = object (TApplication)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
			PEditor : PFontEdit;
			PCharMap : PCharacterMap;
			PNew : PNewFontDialog;
			PLoad, PSave : PFileDialog;
			PInfo : PInfoDialog;
			PSaveBtn : PButton;
			PTooBig : PMessageBox;
			PResize : PResizeDialog;
			PWLabel, PHLabel, PSLabel, PCLabel, PALabel : PLabel;
			PBtnChar, PBtnDemo, PBtnInfo, PBtnSize : PButton;
			PBtnPrev, PBtnNext : PButton;
			PBtnUp, PBtnDown, PBtnLeft, PBtnRight : PButton;
		procedure BeforeShow; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		function GetCharDialog : PCharacterMap; virtual;
		function GetNewDialog : PNewFontDialog; virtual;
		function GetLoadDialog : PFileDialog; virtual;
		function GetSaveDialog : PFileDialog; virtual;
		function GetInfoDialog : PDialog; virtual;
		function GetTooBigDialog : PDialog; virtual;
		function GetResizeDialog : PDialog; virtual;
		procedure UpdateFontHeader; virtual;
		procedure CreateAllDialogs(ConserveMemory : boolean); virtual; 
		procedure SetButtonState; virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
	end;   
	
{ --- Global Variables --- }
var
	ETool : TETool;
	
procedure FormatMaxHeight (AVisual : PControl; var AValue : String); far;
var
	D : TFormatData;
begin
	if Assigned( PETool(Application)^.PNew ) then
		with PETool(Application)^.PNew^ do begin
			if (AVisual = PControl(PSLabel)) then begin
				D.ID := fmtInteger;
				D.IntegerValue := PETool(Application)^.PEditor^.GetMaxHeight;
				AValue := FormatStr(AValue, D, 1);
			end;
		end;
	if Assigned( PETool(Application)^.PResize ) then
		with PETool(Application)^.PResize^ do begin
			if (AVisual = PControl(PSLabel)) then begin
				D.ID := fmtInteger;
				D.IntegerValue := PETool(Application)^.PEditor^.GetMaxHeight;
				AValue := FormatStr(AValue, D, 1);
			end;
		end;
end;

procedure FormatFontStatus (AVisual : PControl; var AValue : String); far;
var
	D : TFormatData;
begin
	if Not Assigned( PETool(Application)^.PEditor ) then exit;
	with PETool(Application)^ do begin
		if AVisual = PControl(PWLabel) then begin
			D.ID := fmtInteger;
			D.IntegerValue := PEditor^.FFont^.GetWidth;
			AValue := FormatStr(AValue, D, 1);
		end else 
		if AVisual = PControl(PHLabel) then begin
			D.ID := fmtInteger;
			D.IntegerValue := PEditor^.FFont^.GetHeight;
			AValue := FormatStr(AValue, D, 1);
		end else 
		if AVisual = PControl(PSLabel) then begin
			D.ID := fmtInteger;
			D.IntegerValue := PEditor^.FFont^.GetSize;
			AValue := FormatStr(AValue, D, 1);
		end else 
		if AVisual = PControl(PCLabel) then begin
			D.ID := fmtString;
			if PEditor^.FCurrent >= 0 then 
				D.StringValue := HexByte(PEditor^.FCurrent)
			else
				D.StringValue := '??';
			AValue := FormatStr(AValue, D, 1);
		end else 
		if AVisual = PControl(PALabel) then begin
			D.ID := fmtString;
			if PEditor^.FCurrent >= 0 then 
				D.StringValue := Char(PEditor^.FCurrent)
			else
				D.StringValue := '';
			AValue := FormatStr(AValue, D, 1);
		end;
	end;
end;
	
{ --- TETool --- }
function TETool.ObjectClass ( AName : String ) : String; 
begin
	if (AName = '') or (AName = class_TFontDesigner) then 
		ObjectClass := class_TFontDesigner
	else
		ObjectClass := inherited ObjectClass(AName);
end;

function TETool.GetCharDialog;
begin
	if Not Assigned(PCharMap) then begin
		PCharMap := New(PCharacterMap, Create(@Self, 'Characters')); 
		ApplySettings(PCharMap);
	end;
	GetCharDialog := PCharMap;
end;

function TETool.GetNewDialog;
begin
	if Not Assigned(PNew) then begin
		PNew := New(PNewFontDialog, Create(@Self, 'DIALOG.NEW'));
		ApplySettings(PNew);
	end;
	GetNewDialog := PNew;
end;

function TETool.GetResizeDialog;
begin
	if Not Assigned(PResize) then begin
		PResize := New(PResizeDialog, Create(@Self, 'DIALOG.RESIZE'));
		ApplySettings(PResize);
	end;
	GetResizeDialog := PResize;
end;

function TETool.GetLoadDialog;
begin
	if Not Assigned(PLoad) then begin
		PLoad := New(POpenDialog, Create(@Self, 'DIALOG.LOAD'));
		with POpenDialog(PLoad)^ do begin
			SetTitle('Load Font');	
			AddFilter('Font Files (*.FNT)');
			AddFilter('All Files (*.*)');
		end;
		ApplySettings(PLoad);
	end;
	GetLoadDialog := PLoad;
end;

function TETool.GetSaveDialog;
begin
	if Not Assigned(PSave) then begin
		PSave := New(PSaveDialog, Create(@Self, 'DIALOG.SAVE'));
		with PSaveDialog(PSave)^ do begin
			SetTitle('Save Font As');	
			AddFilter('Font Files (*.FNT)');
			AddFilter('All Files (*.*)');
		end;	
		ApplySettings(PSave);
	end;
	GetSaveDialog := PSave;
end;

function TETool.GetInfoDialog;
var
	P, T : PListItem;
	S, K : String;
begin
	if Not Assigned(PInfo) then begin
		PInfo := New(PInfoDialog, Create(@Self, 'DIALOG.INFO'));
		ApplySettings(PInfo);
	end;
	PInfo^.FList^.Clear;
	PInfo^.FKeys^.ClearItems;
	P := PEditor^.FFont^.FHeader.First;
	while Assigned(P) do begin
		S := Trim(P^.GetNameID);
		K := UCase(Copy(S, 1, Pos('=', S) - 1));
		S := Copy(S, Length(K) + 2, Length(S));
		if (K <> '') then begin
			PInfo^.FKeys^.AddItem(K);
			T := New(PListItem, Create(K));
			PInfo^.FList^.Add(T);
			T^.Add(New(PListItem, Create(S)));
		end;
		P := P^.Next;
	end;
	PInfo^.FKeys^.SetCaption('');
	PInfo^.FValue^.SetCaption('');
	GetInfoDialog := PInfo;
end;

function TETool.GetTooBigDialog;
var
	B : TBounds;
begin
	if Not Assigned(PTooBig) then begin
		TextAttr := $4F;
		PTooBig := New(PMessageBox, Create(@Self, 'DIALOG.TOOBIG'));
		with PTooBig^ do begin
			SetBackground(4);
			SetTextAttr($4F);
			Bounds(1,1,60,11, B);
			SetBounds(B);
			SetTitle('Oh, Me So Sorry!');
			SetMessage(' ');
			AppendMessage('');
			AppendMessage('But, that font is to tall to edit with the current');
			AppendMessage('screen height. I advise modifying the EFNTDSGN.CFG');
			AppendMessage('file so this program can have more screen text');
			AppendMessage('lines for editing.');
			AppendMessage(' ');
			AppendMessage('Maybe change it to FONT=PGM808.FNT'); 
		end;
		ApplySettings(PTooBig);
	end;
	GetTooBigDialog := PInfo;
end;

procedure TETool.CreateAllDialogs;
begin
	GetCharDialog;
	if ConserveMemory then FreeDialog(PDialog(PCharMap));
	GetNewDialog;
	if ConserveMemory then FreeDialog(PDialog(PNew));
	GetLoadDialog;
	if ConserveMemory then FreeDialog(PDialog(PLoad));
	GetSaveDialog;
	if ConserveMemory then FreeDialog(PDialog(PSave));
	GetInfoDialog;
	if ConserveMemory then FreeDialog(PDialog(PInfo));
	GetResizeDialog;
	if ConserveMemory then FreeDialog(PDialog(PResize));
	GetTooBigDialog;
	if ConserveMemory then FreeDialog(PDialog(PTooBig));
end;

constructor TETool.Create;
var
	BWide : integer;
	P, T, L : PControl;
	B : TBounds;
begin
	inherited Create(nil, AppID);
	FInfo.Settitle(AppTitle);
	
	SetHelpText('Welcome to the Eternity Font Designer for the Program Manager Eternity Edition.');
	AddHelpText('');
	AddHelpText('With this program you can create custom fonts to use with PGME and');
	AddHelpText('other text based DOS programs. You can even create a custom font');
	AddHelpText('that can be converted using QFNT2TSR to replace the built-in');
	AddHelpText('VGA BIOS text font.');
	BWide := 16;
	PCharMap := nil;
	PNew := nil;
	PLoad := nil;
	PSave := nil;
	PInfo := nil;

	TextAttr := $1F;
	P := New(PWindow, Create(@Self, 'MAIN'));
	with PWindow(P)^ do begin
		SetHelpText('Welcome to the Eternity Font Designer for the Program Manager Eternity Edition.');
		AddHelpText('');
		AddHelpText('With this program you can create custom fonts to use with PGME and');
		AddHelpText('other text based DOS programs. You can even create a custom font');
		AddHelpText('that can be converted using QFNT2TSR to replace the built-in');
		AddHelpText('VGA BIOS text font.');
		SetTitle(AppTitle);
		SetShadow(False);
		SetCanClose(True);
		with PControl(Parent)^ do
			Bounds(1,1,Width, Height, B);
		SetBounds(B);
		SetAnchors(alClient);
		SetMainWindow(True);
	end;

	TextAttr := $0F;
	PEditor := New(PFontEdit, Create(P, 'EDITOR'));
	with PEditor^ do begin
		SetHelpText('Character Editor Panel: Clicking the mouse button inside the character editor');
		AddHelpText('panel will toggle bits for that character on and off.');
		Bounds( 6, 3, 10, 10,B);
		SetBounds(B);
	end;
	
	TextAttr := $2E;
	T := New(PButton, Create(P, 'QUIT'));
	with PButton(T) ^ do begin
		SetHelpText('5;"%0" Button: Close the Eternity Font Designer.');
		AddHelpText('(Unsaved changes will be lost)');
		AddHelpText('The default keyboard shortcut is "Alt+Q".');
		SetAnchors(alBottom or alRight);  
		Bounds(1, 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Exit');
		SetCommand(cmQuit);  
	end;

	TextAttr := $1F;
	{ Side Line }
	Bounds(P^.Width - BWide - 2, 0, 1, P^.Height, B);
	T := New(PLine, Create(P, 'LINE.RIGHT'));
	with PLine(T)^ do begin
		SetStyle(lnSingle or lnVertical);
		SetAllowedOutside(True);
		SetAnchors(abTop or abBottom or abRight);
		SetBounds(B);
	end;
	
	{ Side Line Bottom }
	Bounds(P^.Width - BWide - 2, P^.Height - 3, BWide + 2, 1, B);
	T := New(PLine, Create(P, 'LINE.BOTTOM'));
	with PLine(T)^ do begin
		SetStyle(lnSingle or lnHorizontal);
		SetAllowedOutside(True);
		SetAnchors(abLeft or abBottom or abRight);
		SetBounds(B);
	end;
	
	TextAttr := $2F;
	T := New(PButton, Create(P, 'NEW'));
	with PButton(T) ^ do begin
		SetHelpText('5;"%0" Button: Create a new blank or system BIOS based font.');
		AddHelpText('(Unsaved changes will be lost)');
		AddHelpText('The default keyboard shortcut is "C".');
		SetAnchors(alRight);  
		Bounds(1, 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Create Font');
		SetCommand(cmNewFile); 
		SetKeyCode(kbC, kbNone, kfMatches); 
		L := T;
	end;

	TextAttr := $2F;
	T := New(PButton, Create(P, 'LOAD'));
	with PButton(T) ^ do begin
		SetHelpText('5;"%0" Button: Load an existing QuickCRT font from a file.');
		AddHelpText('(Unsaved changes will be lost)');
		AddHelpText('The default keyboard shortcut is "L".');
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Load Font');
		SetCommand(cmOpenFile); 
		SetKeyCode(kbL, kbNone, kfMatches); 
		L := T;
	end;

	TextAttr := $2F;
	T := New(PButton, Create(P, 'SAVE'));
	with PButton(T) ^ do begin
		SetHelpText('5;"%0" Button: Saves the current font to a new or existing file.');
		AddHelpText('The default keyboard shortcut is "A".');
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Save Font');
		SetCommand(cmSaveFileAs);
		SetEnabled(False);
		SetKeyCode(kbA, kbNone, kfMatches); 
		L := T;
		PSaveBtn := PButton(T);
	end;

	{ Side Line Top }
	TextAttr := $1F;
	Bounds(P^.Width - BWide - 2, L^.Top + 1, BWide + 2, 1, B);
	T := New(PLine, Create(P, 'LINE.UPPER'));
	with PLine(T)^ do begin
		SetStyle(lnSingle or lnHorizontal);
		SetAllowedOutside(True);
		SetAnchors(abLeft or abTop or abRight);
		SetBounds(B);
		L := T;
	end;

	TextAttr := $17;
	PWLabel := New(PLabel, Create(P, 'WIDTH'));
	with PLabel(PWLabel) ^ do begin
		OnFormat := FormatFontStatus;
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2, 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0[Z2]; Width: %0');
		L := PWLabel;
	end;
	
	PHLabel := New(PLabel, Create(P, 'HEIGHT'));
	with PLabel(PHLabel) ^ do begin
		OnFormat := FormatFontStatus;
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2 , 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0[Z2];Height: %0');
		L := PHLabel;
	end;

	PSLabel := New(PLabel, Create(P, 'SIZE'));
	with PLabel(PSLabel) ^ do begin
		OnFormat := FormatFontStatus;
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2, 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0;  Size: %0');
		L := PSLabel;
	end;

	{ Side Line Middle }
	TextAttr := $1F;
	Bounds(P^.Width - BWide - 2, L^.Top + 1, BWide + 2, 1, B);
	T := New(PLine, Create(P, 'LINE.MID'));
	with PLine(T)^ do begin
		SetStyle(lnSingle or lnHorizontal);
		SetAllowedOutside(True);
		SetAnchors(abLeft or abTop or abRight);
		SetBounds(B);
		L := T;
	end;

	TextAttr := $17;
	PALabel := New(PLabel, Create(P, 'ASCII'));
	with PLabel(PALabel) ^ do begin
		OnFormat := FormatFontStatus;
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2, 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0[X1];ASCII: "%0"');
		L := PALabel;
	end;
	
	PCLabel := New(PLabel, Create(P, 'CODE'));
	with PLabel(PCLabel) ^ do begin
		OnFormat := FormatFontStatus;
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2 , 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0[Z2];VALUE: 0x%0h');
		L := PCLabel;
	end;

	{ Side Line Lower }
	TextAttr := $1F;
	Bounds(P^.Width - BWide - 2, L^.Top + 1, BWide + 2, 1, B);
	T := New(PLine, Create(P, 'LINE.LOWER'));
	with PLine(T)^ do begin
		SetStyle(lnSingle or lnHorizontal);
		SetAllowedOutside(True);
		SetAnchors(abLeft or abTop or abRight);
		SetBounds(B);
		L := T;
	end;
	
	{ Some Dialog Buttons }

	Bounds(P^.Width - BWide - 2, P^.Height - 6, 8, 1, B);
	BWide := 9;
	TextAttr := $6F;
	PBtnInfo := New(PButton, Create(P, 'BUTTON.INFO'));
	with PButton(PBtnInfo)^ do begin
		SetHelpText('5;"%0" Button: Opens the Info Dialog box. Where changes to the');
		AddHelpText('fonts meta data can be made. The default keyboard shortcut is "I".');
		SetAnchors(abRight or abBottom);  
		Dec(B.Left, BWide);
		SetBounds(B);
		SetCaption('Info');
		SetCommand(cmFileInfo); 
		SetKeyCode(kbI, kbNone, kfMatches); 
		SetEnabled(False);
		L := T;
	end;	
	PBtnDemo := New(PButton, Create(P, 'BUTTON.DEMO'));
	with PButton(PBtnDemo)^ do begin
		SetHelpText('5;"%0" Button: Previews the font.');
		AddHelpText('The default keyboard shortcut is "D".');
		SetAnchors(abRight or abBottom);  
		Dec(B.Left, BWide);
		SetBounds(B);
		SetCaption('Demo');
		SetCommand(cmDemo); 
		SetKeyCode(kbD, kbNone, kfMatches); 
		SetEnabled(False);
		L := T;
	end;
	PBtnChar := New(PButton, Create(P, 'BUTTON.CHAR'));
	with PButton(PBtnChar) ^ do begin
		SetHelpText('5;"%0" Button: Opens the Character Map Dialog box. Where you can quickly');
		AddHelpText('select a different character to edit. The default keyboard shortcut is "M".');
		SetAnchors(abRight or abBottom);  
		Dec(B.Left, BWide);
		SetBounds(B);
		SetCaption('Map');
		SetCommand(cmCharMap);
		SetKeyCode(kbM, kbNone, kfMatches); 
		SetEnabled(False);
		L := T;
	end;
	PBtnSize := New(PButton, Create(P, 'BUTTON.SIZE'));
	with PButton(PBtnSize) ^ do begin
		SetHelpText('5;"%0" Button: Opens the Resize Dialog box. Where you can');
		AddHelpText('change the fonts bit height. The default keyboard shortcut is "S".');
		SetAnchors(abRight or abBottom);  
		Dec(B.Left, BWide);
		SetBounds(B);
		SetCaption('Size');
		SetCommand(cmResize); 
		SetEnabled(False);
		SetKeyCode(kbS, kbNone, kfMatches); 
		L := T;
	end;
	
	{ Prev/Next Char Buttons }
	BWide := 16;
	Bounds(30 - BWide - 1, P^.Height - 8, 14, 1, B);
	TextAttr := $5F;
	PBtnPrev := New(PButton, Create(P, 'BUTTON.PREV'));
	with PButton(PBtnPrev)^ do begin
		SetHelpText('5;"%0" Button: Selects the charcater before the current one for editing.');
		AddHelpText('The default keyboard shortcut is "PageUp".');
		SetAnchors(abRight or abBottom);  
		Inc(B.Left, BWide);
		SetBounds(B);
		SetCaption('Previous');
		SetCommand(cmPrevChar); 
		SetRespond(cmPageUp); 
		SetEnabled(False);
		L := T;
	end;	
	PBtnNext := New(PButton, Create(P, 'BUTTON.NEXT'));
	with PButton(PBtnNext)^ do begin
		SetHelpText('5;"%0" Button: Selects the charcater after the current one for editing.');
		AddHelpText('The default keyboard shortcut is "PageDown".');
		SetAnchors(abRight or abBottom);  
		Inc(B.Left, BWide);
		SetBounds(B);
		SetCaption('Next');
		SetCommand(cmNextChar); 
		SetRespond(cmPageDown); 
		SetEnabled(False);
		L := T;
	end;	

	{ Shifting Buttons }
	BWide := 9;
	Bounds(44 - BWide div 2, 4, BWide, 1, B);
	TextAttr := $3F;
	PBtnUp := New(PButton, Create(P, 'BUTTON.UP'));
	with PButton(PBtnUp)^ do begin
		SetHelpText('5;"%0" Button: Shifts the current charcater up one line.');
		AddHelpText('The default keyboard shortcut is "Up Arrow".');
		SetAnchors(abRight);  
		SetBounds(B);
		SetCaption('Up');
		SetCommand(cmShiftUp); 
		SetRespond(cmUp); 
		SetEnabled(False);
		L := T;
	end;	
	Bounds(44 - BWide div 2, 8, BWide, 1, B);
	PBtnDown := New(PButton, Create(P, 'BUTTON.DOWN'));
	with PButton(PBtnDown)^ do begin
		SetHelpText('5;"%0" Button: Shifts the current charcater down one line.');
		AddHelpText('The default keyboard shortcut is "Down Arrow".');
		SetAnchors(abRight);  
		SetBounds(B);
		SetCaption('Down');
		SetCommand(cmShiftDown); 
		SetRespond(cmDown); 
		SetEnabled(False);
		L := T;
	end;	
	Bounds(44 - BWide div 2 - BWide, 6, BWide, 1, B);
	PBtnLeft := New(PButton, Create(P, 'BUTTON.LEFT'));
	with PButton(PBtnLeft)^ do begin
		SetHelpText('5;"%0" Button: Shifts the current charcater left one pixel.');
		AddHelpText('The default keyboard shortcut is "Left Arrow".');
		SetAnchors(abRight);  
		SetBounds(B);
		SetCaption('Left');
		SetCommand(cmShiftLeft); 
		SetRespond(cmLeft); 
		SetEnabled(False);
		L := T;
	end;	
	Bounds(44 - BWide div 2 + BWide, 6, BWide, 1, B);
	PBtnRight := New(PButton, Create(P, 'BUTTON.RIGHT'));
	with PButton(PBtnRight)^ do begin
		SetHelpText('5;"%0" Button: Shifts the current charcater right one pixel.');
		AddHelpText('The default keyboard shortcut is "Right Arrow".');
		SetAnchors(abRight);  
		SetBounds(B);
		SetCaption('Right');
		SetCommand(cmShiftRight); 
		SetRespond(cmRight); 
		SetEnabled(False);
		L := T;
	end;	
	
	TextAttr := $2F;
	with PButton(New(PButton, Create(P, 'BUTTON.HELP')))^ do begin
		SetHelpText('5;"%0" Button: Opens the Help Window.');
		AddHelpText('The default keyboard shortcut is "Alt+H".');
		SetAnchors(abRight or abBottom);  
		Bounds(80 - 18 - 10, P^.Height - 2, 10, 1, B);
		SetBounds(B);
		SetCaption('Help');
		SetCommand(cmHelp); 
	end;	
	
	FClrScr := True;
	
	FStartTone := '400:3,200:3,400:3,500:3,400:3,500:3,400:3,200:3,200:3' +
	'0:300,200:3,400:3,500:3';
	FEndTone := '500:1,100:1,200:1';
	
end;

destructor TETool.Destroy;
begin
	inherited Destroy;
end;

procedure TETool.BeforeShow;
begin
	inherited BeforeShow;
	GotoXY(1, WhereY);
	TextAttr := $07;
	ClrEOL;
	if ParamStr(1) <> '' then begin 
		PEditor^.Load(ParamStr(1));
		SetButtonState;
	end;
end;	
	
procedure TETool.UpdateFontHeader;
var
	P : PListItem;
begin
	if not Assigned(PEditor^.FFont^.FFontPtr) then exit;
	if Not Assigned(PInfo) then exit;
	PEditor^.FFont^.FHeader.Clear;
	P := PInfo^.FList^.First;
	while Assigned(P) do begin
		if P^.GetNameID <> '' then begin
			if Assigned(P^.First) and (P^.First^.GetNameID <> '') then begin
				PEditor^.FFont^.FHeader.Add(
					New(PListItem, Create(P^.GetNameID + '=' + P^.First^.GetNameID)));
			end;
		end;
		P := P^.Next;
	end;
	PInfo^.FList^.Clear;
	PInfo^.FKeys^.ClearItems;
end;

procedure TETool.SetButtonState;
var
	AEnabled : boolean;
begin
	AEnabled := Assigned(PEditor^.FFont^.FFontPtr);
	StartDrawing;
	PSaveBtn^.SetEnabled(AEnabled);
	PBtnChar^.SetEnabled(AEnabled);
	PBtnDemo^.SetEnabled(AEnabled);
	PBtnInfo^.SetEnabled(AEnabled);
	PBtnSize^.SetEnabled(AEnabled);
	PBtnPrev^.SetEnabled(AEnabled);
	PBtnNext^.SetEnabled(AEnabled);
	PBtnUp^.SetEnabled(AEnabled);
	PBtnDown^.SetEnabled(AEnabled);
	PBtnLeft^.SetEnabled(AEnabled);
	PBtnRight^.SetEnabled(AEnabled);
	FinishDrawing;
end;

procedure TETool.CheckLocal(var AEvent : TEvent); 
var
	T : integer;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmNewFile : begin 
				ClearEvent(AEvent);
				GetNewDialog;
				T := PNew^.ShowModal;
				StartDrawing;
				if T = mrOK then begin
					PEditor^.NewFont(StrInt(PNew^.PSize^.GetCaption), PNew^.PClone^.GetState > 0,
					PNew^.PStretch^.GetState > 0);
				end;
				SetButtonState;
				Update;
				FinishDrawing;
			end;
			cmResize : begin 
				ClearEvent(AEvent);
				GetResizeDialog;
				T := PResize^.ShowModal;
				StartDrawing;
				if T = mrOK then begin
					if PResize^.PStretch^.GetState > 0 then
						PEditor^.Stretch(StrInt(PResize^.PSize^.GetCaption))
					else
						PEditor^.Resize(StrInt(PResize^.PSize^.GetCaption))
				end;
				Update;
				FinishDrawing;
			end;
			cmTooBig : begin
				ClearEvent(AEvent);
				GetTooBigDialog;
				PTooBig^.ShowModal;
			end;
			cmOpenFile : begin 
				ClearEvent(AEvent);
				GetLoadDialog;
				T := PLoad^.ShowModal;
				StartDrawing;
				if T = mrOK then begin
					PEditor^.Load(PLoad^.GetFileName);
					SetButtonState;
				end;
				Update;
				FinishDrawing;
			end;
			cmSaveFile, cmSaveFileAs : begin 
				ClearEvent(AEvent);
				GetSaveDialog;
				T := PSave^.ShowModal;
				if T = mrOK then begin
					PEditor^.Save(PSave^.GetFileName);
				end;
			end;
			cmFileInfo : begin
				ClearEvent(AEvent);
				GetInfoDialog; 
				T := PInfo^.ShowModal;
				if (T = mrOK) and  Assigned(PEditor^.FFont^.FFontPtr) then
					UpdateFontHeader;
			end;
			cmCharMap : begin
				ClearEvent(AEvent);
				GetCharDialog;
				T := PCharMap^.ShowModal;
				StartDrawing;			
				if (T >= 0) and (T <=  255) then 
					PEditor^.FCurrent := T;							
				PEditor^.Update;
				PALabel^.Update;
				PCLabel^.Update;
				FinishDrawing;
			end;
			cmPrevChar : if PEditor^.FCurrent > 0 then begin
				Dec(PEditor^.FCurrent);
				StartDrawing;
				PEditor^.Update;
				PALabel^.Update;
				PCLabel^.Update;
				FinishDrawing;
			end;
			cmNextChar : if (PEditor^.FCurrent > -1) and (PEditor^.FCurrent < 255) then begin
				Inc(PEditor^.FCurrent);
				StartDrawing;
				PEditor^.Update;
				PALabel^.Update;
				PCLabel^.Update;
				FinishDrawing;
			end;
			cmDemo : begin
				ClearEvent(AEvent);
				PEditor^.Demo;
				MouseVerify;
			end;
			cmShiftUp : begin
				ClearEvent(AEvent);
				PEditor^.ShiftUp;
			end;
			cmShiftDown : begin
				ClearEvent(AEvent);
				PEditor^.ShiftDown;
			end;
			cmShiftLeft : begin
				ClearEvent(AEvent);
				PEditor^.ShiftLeft;
			end;
			cmShiftRight : begin
				ClearEvent(AEvent);
				PEditor^.ShiftRight;
			end;
		end;
	inherited CheckLocal(AEvent);
end;
