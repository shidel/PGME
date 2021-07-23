{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$IFDEF INTERFACE}
const
	class_TResizeDialog = 'TResizeDialog';

type
	PResizeDialog = ^TResizeDialog;
	TResizeDialog = object (TDialog)
	public { protected }
			PSLabel : PLabel;
			PSize : PEdit;
			PStretch : PCheckBox;
			POK : PButton;
		function  ObjectClass ( AName : String ) : String; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
	
	
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TResizeDialog.ObjectClass ( AName : String ) : String; 
begin
	if (AName = '') or (AName = class_TResizeDialog) then 
		ObjectClass := class_TResizeDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TResizeDialog.Create;
var
	P : PControl;
	B : TBounds;
	I : integer;
begin
	inherited Create(AParent, AName);
	Bounds(1,1,40,11, B);
	SetBounds(B);
	SetTitle('Resize Font');	
	SetHelpText('Resize Font Dialog.');
	AddHelpText('');
	AddHelpText('With this dialog you can resize the current font.');

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
		SetHelpText('"Edit" field. Enter the desired height of the font.');
		Bounds(Self.Width - 12,2, 6, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abTop or abRight) ;
		SetCaption('16');
	end;
	TextAttr := $70;
	PStretch := New(PCheckBox, Create(@Self, 'STRETCH'));
	with PStretch^ do begin
		SetHelpText('"Stretch" Checkbox.');
		AddHelpText('When checked, the current font will be stretched or shrunk to match');
		AddHelpText('the requested size. This is slow and make take minute or so to');
		AddHelpText('complete.');
		Bounds(4,4, Self.Width - 8, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abTop or abRight);
		SetCaption('Stretch it. (Slow)');
		SetState(1);
		FAttr.Disabled := $78;
	end;
	TextAttr := $2E;
	POk := New(PButton, Create(@Self, 'OK'));
	with POK^ do begin
		SetHelpText('5;"%0" Button. To accept the settings and resize the current font.');	
		Bounds((Self.Width - 2) div 2 - 5, Self.Height - 4 , 10, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abBottom);
		SetCaption('OK');
		SetCommand(cmOK);
	end;
end;

procedure TResizeDialog.CheckLocal(var AEvent : TEvent); 
var
	V : integer;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
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
