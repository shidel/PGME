{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$IFDEF INTERFACE}
const
	class_TNewFontDialog = 'TNewFontDialog';

type
	PNewFontDialog = ^TNewFontDialog;
	TNewFontDialog = object (TDialog)
	public { protected }
			PSLabel : PLabel;
			PSize : PEdit;
			PClone, PStretch : PCheckBox;
			POK : PButton;
		function  ObjectClass ( AName : String ) : String; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
	
	
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TNewFontDialog.ObjectClass ( AName : String ) : String; 
begin
	if (AName = '') or (AName = class_TNewFontDialog) then 
		ObjectClass := class_TNewFontDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TNewFontDialog.Create;
var
	P : PControl;
	B : TBounds;
	I : integer;
begin
	inherited Create(AParent, AName);
	Bounds(1,1,40,12, B);
	SetBounds(B);
	SetTitle('Create New Font');	
	SetHelpText('Create New Font Dialog.');
	AddHelpText('');
	AddHelpText('With this dialog you can start a new font. (Any changes to an unsaved');
	AddHelpText('font will be lost)');

	TextAttr := $70;
	PSLabel := New(PLabel, Create(@Self, 'SIZE.LABEL'));
	with PSLabel^ do begin
		OnFormat := FormatMaxHeight;
		Bounds(3,2, 24, 1, B);
		FHAlign := AlignLeft;
		SetBounds(B);
		SetAnchors(abLeft or abTop);
		SetCaption('0[Z2]; Font Height (Max. %0):');
	end;
	TextAttr := $1E;
	PSize := New(PEdit, Create(@Self, 'SIZE.EDIT'));
	with PEdit(PSize)^ do begin
		SetHelpText('"Edit" field. Enter the desired height of the new font.');
		Bounds(Self.Width - 12,2, 6, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abTop or abRight) ;
		SetCaption('16');
	end;
	TextAttr := $70;
	PClone := New(PCheckBox, Create(@Self, 'CLONE'));
	with PClone^ do begin
		SetHelpText('"Clone" Checkbox. When checked, the new font will be based on');
		AddHelpText('a ROM BIOS font of the specified or smaller size. If it is not,');
		AddHelpText('it will be a blank font.');
		Bounds(4,4, Self.Width - 8, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abTop or abRight);
		SetCaption('Clone ROM BIOS font.');
		SetState(1);
	end;
	PStretch := New(PCheckBox, Create(@Self, 'STRETCH'));
	with PStretch^ do begin
		SetHelpText('"Stretch" Checkbox. Is only available when Cloning the ROM font.');
		AddHelpText('When checked, the new font will be stretched or shrunk to match');
		AddHelpText('the requested size. This is slow and make take minute or so to');
		AddHelpText('complete.');
		Inc(B.Top);
		SetBounds(B);
		SetAnchors(abLeft or abTop or abRight);
		SetCaption('Stretch it. (Slow)');
		SetState(1);
		FAttr.Disabled := $78;
	end;
	TextAttr := $2E;
	POk := New(PButton, Create(@Self, 'OK'));
	with POK^ do begin
		SetHelpText('5;"%0" Button. To accept the settings and create the new font.');	
		Bounds((Self.Width - 2) div 2 - 5, Self.Height - 4 , 10, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abBottom);
		SetCaption('OK');
		SetCommand(cmOK);
	end;
end;

procedure TNewFontDialog.CheckLocal(var AEvent : TEvent); 
var
	V : integer;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmCheckBox : if AEvent.InfoPtr = PClone then begin
				PStretch^.SetEnabled(PClone^.GetState > 0);
			end;
			cmEdit : begin
				if AEvent.InfoPtr = PSize then begin
				 	V := StrInt(Trim(PSize^.GetCaption));
				 	if V < 1 then V := 1;
				 	if V > PETool(Application)^.PEditor^.GetMaxHeight then 
				 		V := PETool(Application)^.PEditor^.GetMaxHeight;
					PSize^.SetCaption(IntStr(V));	
					ClearEvent(AEvent);
					PutCommand(cmEdit, @Self);
				end;
			end;
		end; 
	inherited CheckLocal(AEvent);
end;

{$ENDIF}
