{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TEdit --- }

{$IFDEF INTERFACE}
const
	class_TEdit = 'TEdit';
	
type
	PEdit = ^TEdit;
	TEdit = object(TPanel)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FMaxLength : integer;
		FEditAttr : integer;
		FReadOnly : boolean;
		procedure TriggerHook; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		procedure SetReadOnly(AValue: Boolean); virtual;
		function GetReadOnly : boolean; virtual;	
		procedure Draw; virtual;	
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TEdit.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TEdit) then 
		ObjectClass := class_TEdit
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TEdit.Create;
begin
	inherited Create(AParent, AName);
	FMaxLength := 255;
	FUseCaption := False;
	FTextAlign := True;
	if GetBlink then
		begin
			FAttr.Hover := (FAttr.Normal and $0F) + (FAttr.Normal and $07) shl 4;
			FAttr.Click := $09 or ((FAttr.Normal and $80) shr 4) + (FAttr.Normal and $07) shl 4;
		end
	else
		begin
			FAttr.Hover := (FAttr.Normal and $0F) + (FAttr.Normal and $0F) shl 4;
			FAttr.Click := $09 or ((FAttr.Normal and $80) shr 4) + (FAttr.Normal and $07) shl 4;
		end;
	FEditAttr := FAttr.Normal;
	FReadOnly := False;
end;

procedure TEdit.TriggerHook;
var
	S : String;
	R, C : boolean;
begin
	Update;
	if FReadOnly then
		R := True
	else begin
		S := GetCaption;
		gotoXY(1,1);
		TextAttr := FEditAttr;
		C := CheckTab;
		CheckTab := True;
		EditSender := @Self;
		R := EditLn(S, FMaxLength, FCBounds.Width);
		CheckTab := C;
		if R then SetCaption(S);
		Update;
	end;
	MouseVerify;
	if R then begin 
		if FCommand <> cmNone then 
			SendCommand
		else
			PutCommand(cmEdit, @Self);
	end; 
end;

procedure TEdit.ApplyTheme( AConfig : PConfigFile );
begin
	inherited ApplyTheme(AConfig); 
	AConfig^.GetInts('EDITING', FEditAttr, SizeOf(FEditAttr) div SizeOf(FEditAttr));
end;

procedure TEdit.SetReadOnly(AValue: Boolean); 
begin
	if AValue = FReadOnly then exit;
	FReadOnly := AValue;
end;

function TEdit.GetReadOnly : boolean; 
begin
	GetReadOnly := FReadOnly;
end;

procedure TEdit.Draw; 
var
	X, Y : integer;
	W, H : integer;
	S : String;
begin
	inherited Draw;
	S := Formatted;
	W := Lo(WindMax) - Lo(WindMin) + 1;
	H := Hi(WindMax) - Hi(WindMin) + 1;
	case FHAlign of
		AlignLeft   : X := 0;
		AlignCenter : X := W shr 1 - Length(S) shr 1;
		AlignRight  : X := W - Length(S);
	end;
	case FVAlign of
		AlignTop    : Y := 0;
		AlignMiddle : Y := H shr 1;
		AlignBottom : Y := H - 1;
	end;
	if X < 0 then X := 0;
	if Y < 0 then Y := 0;
	GotoXY(1 + X, 1 + Y);
	FWrite(CropStr(S, W));
end;
{$ENDIF}
