{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TButton --- }

{$IFDEF INTERFACE}
const
	class_TButton = 'TButton';
	
type
	PButton = ^TButton;
	TButton = object(TLabel)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure SetTextAttr(AValue : integer); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TButton.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TButton) then 
		ObjectClass := class_TButton
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TButton.Create;
begin
	inherited Create(AParent, AName);
	FTextAlign := True;
end;

procedure TButton.SetTextAttr(AValue : integer);
begin
	inherited SetTextAttr(AValue);
	if GetBlink then
		begin
			FAttr.Hover := (FAttr.Normal and $F0) shr 4 + (FAttr.Normal and $07) shl 4;
			FAttr.Click := $09 or ((FAttr.Normal and $80) shr 4) + (FAttr.Normal and $07) shl 4;
		end
	else
		begin
			FAttr.Hover := (FAttr.Normal and $F0) shr 4 + (FAttr.Normal and $0F) shl 4;
			FAttr.Click := $09 or ((FAttr.Normal and $80) shr 4) + (FAttr.Normal and $07) shl 4;
		end;
end;
{$ENDIF}
