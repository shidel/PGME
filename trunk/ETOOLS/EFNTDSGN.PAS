{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Font_Designer; { X Series }

uses QCrt, QErrors, QStrings, QDos, QFiles, QMore, QFmtStr,
	QObjects, QControls, QFileDlg, QBios;

{$I ..\ETERNITY\LICENSE.INC}

const
	class_TFontDesigner = 'TFontDesigner';
	
	AppTitle = 'Eternity Font Designer';
	AppID = 'EFNTDSGN';
	
	cmDemo  = cmUser + 4;

type 
	TETool = object (TApplication)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
			PCharMap : PCharacterMap;
			PEFont : PFont;
			PLoad, PSave : PFileDialog;
			PInfo : PDialog;
			PWLabel, PHLabel, PSLabel, PCLabel, PALabel : PLabel;
		procedure BeforeShow; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		function GetCharDialog : PCharacterMap; virtual;
		function GetLoadDialog : PFileDialog; virtual;
		function GetSaveDialog : PFileDialog; virtual;
		function GetInfoDialog : PDialog; virtual;
		procedure CreateAllForTemplate; virtual; { Creates all when building templates }
	public
		constructor Create;
		destructor Destroy; virtual;
	end;   
	
{ --- Global Variables --- }
var
	ETool : TETool;
	
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
	B : TBounds;
begin
	if Not Assigned(PInfo) then begin
		PInfo := New(PDialog, Create(@Self, 'DIALOG.INFO'));
		with PDialog(PInfo)^ do begin
			Bounds(1,1,40,13, B);
			SetBounds(B);
			SetTitle('Font Information');	
		end; 
		ApplySettings(PInfo);
	end;
	GetInfoDialog := PInfo;
end;

procedure TETool.CreateAllForTemplate;
begin
	GetCharDialog;
	GetLoadDialog;
	GetSaveDialog;
	GetInfoDialog;
end;

constructor TETool.Create;
const
	BWide = 16;
var
	P, T, L : PControl;
	B : TBounds;
begin
	inherited Create(nil, AppID);
	FInfo.Settitle(AppTitle);
	TextAttr := $1F;
	P := New(PWindow, Create(@Self, 'Main'));
	with PWindow(P)^ do begin
		SetTitle(AppTitle);
		SetShadow(False);
		SetCanClose(True);
		with PControl(Parent)^ do
			Bounds(1,1,Width, Height, B);
		SetBounds(B);
		SetAnchors(alClient);
		SetMainWindow(True);
	end;

	TextAttr := $2E;
	T := New(PButton, Create(P, 'QUIT'));
	with PButton(T) ^ do begin
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
	PLine(T)^.SetStyle(lnSingle or lnVertical);
	T^.SetAllowedOutside(True);
	T^.SetAnchors(abTop or abBottom or abRight);
	T^.SetBounds(B);

	{ Side Line Bottom }
	Bounds(P^.Width - BWide - 2, P^.Height - 3, BWide + 2, 1, B);
	T := New(PLine, Create(P, 'LINE.BOTTOM'));
	PLine(T)^.SetStyle(lnSingle or lnHorizontal);
	T^.SetAllowedOutside(True);
	T^.SetAnchors(abLeft or abBottom or abRight);
	T^.SetBounds(B);
	
	TextAttr := $2F;
	T := New(PButton, Create(P, 'LOAD'));
	with PButton(T) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Load');
		SetCommand(cmOpenFile); 
		L := T;
	end;

	TextAttr := $2F;
	T := New(PButton, Create(P, 'SAVE'));
	with PButton(T) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Save');
		SetCommand(cmSaveFileAs); 
		L := T;
	end;

	TextAttr := $2F;
	T := New(PButton, Create(P, 'CHAR'));
	with PButton(T) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Character Map');
		SetCommand(cmCharMap); 
		L := T;
	end;
	
	TextAttr := $2F;
	T := New(PButton, Create(P, 'INFO'));
	with PButton(T) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Information');
		SetCommand(cmFileInfo); 
		L := T;
	end;

	TextAttr := $2F;
	T := New(PButton, Create(P, 'DEMO'));
	with PButton(T) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide, 1, B);
		SetBounds(B);
		SetCaption('Preview');
		SetCommand(cmDemo); 
		L := T;
	end;
	
	{ Side Line Top }
	TextAttr := $1F;
	Bounds(P^.Width - BWide - 2, L^.Top + 1, BWide + 2, 1, B);
	T := New(PLine, Create(P, 'LINE.UPPER'));
	PLine(T)^.SetStyle(lnSingle or lnHorizontal);
	T^.SetAllowedOutside(True);
	T^.SetAnchors(abLeft or abTop or abRight);
	T^.SetBounds(B);
	L := T;

	PEFont := New(PFont, Create('EDIT_FONT'));

	TextAttr := $17;
	PWLabel := New(PLabel, Create(P, 'WIDE'));
	with PLabel(PWLabel) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2, 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0[Z2]; Width: %0');
		L := PWLabel;
	end;
	
	PHLabel := New(PLabel, Create(P, 'Height'));
	with PLabel(PHLabel) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2 , 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0[Z2];Height: %0');
		L := PHLabel;
	end;

	PSLabel := New(PLabel, Create(P, 'SIZE'));
	with PLabel(PSLabel) ^ do begin
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
	PLine(T)^.SetStyle(lnSingle or lnHorizontal);
	T^.SetAllowedOutside(True);
	T^.SetAnchors(abLeft or abTop or abRight);
	T^.SetBounds(B);
	L := T;

	TextAttr := $17;
	PALabel := New(PLabel, Create(P, 'ASCII'));
	with PLabel(PALabel) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2, 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0[X1];ASCII: "%0"');
		L := PALabel;
	end;
	
	PCLabel := New(PLabel, Create(P, 'Code'));
	with PLabel(PCLabel) ^ do begin
		SetAnchors(alRight);  
		Bounds(1, L^.Top + 1, BWide - 2 , 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetCaption('0[L5];VALUE: 0x%0h');
		L := PCLabel;
	end;

	{ Side Line Lower }
	TextAttr := $1F;
	Bounds(P^.Width - BWide - 2, L^.Top + 1, BWide + 2, 1, B);
	T := New(PLine, Create(P, 'LINE.LOWER'));
	PLine(T)^.SetStyle(lnSingle or lnHorizontal);
	T^.SetAllowedOutside(True);
	T^.SetAnchors(abLeft or abTop or abRight);
	T^.SetBounds(B);
	L := T;
	
	PCharMap := nil;
	PLoad := nil;
	PSave := nil;
	PInfo := nil;

	FClrScr := True;
end;

destructor TETool.Destroy;
begin
	if Assigned(PEFont) then Dispose(PEFont, Destroy);
	inherited Destroy;
end;

procedure TETool.BeforeShow;
begin
	inherited BeforeShow;
	GotoXY(1, WhereY);
	TextAttr := $07;
	ClrEOL;
end;	
	
procedure TETool.CheckLocal(var AEvent : TEvent); 
var
	T : integer;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmOpenFile : begin 
				GetLoadDialog;
				T := PLoad^.ShowModal;
			end;
			cmSaveFile, cmSaveFileAs : begin 
				GetSaveDialog;
				T := PSave^.ShowModal;
			end;
			cmFileInfo : begin
				GetInfoDialog; 
				T := PInfo^.ShowModal;
			end;
			cmCharMap : begin
				GetCharDialog;
				T := PCharMap^.ShowModal;
				if T > mrNoResult then begin
					AEvent.what := evKeyboard;
					AEvent.ShiftCode := $FFFF;
					AEvent.Original := T;
					AEvent.KeyCode := T;
					PutEvent(AEvent);
				end;
				ClearEvent(AEvent);
			end;
			cmDemo : begin
			end;
		end;
	inherited CheckLocal(AEvent);
end;

procedure Execute;
begin
	Write('Starting ' + AppTitle + '...');
	ETool.Create;
	ETool.Execute;
	ETool.Destroy;   
	TextAttr := $08;
	GotoXY(1,WhereY);
	Write('Goodbye.');
	ClrEol;	
	TextAttr := $07;
	WriteLn;
end;
		 
begin
	Execute;
end.
