{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TBox --- }

{$IFDEF INTERFACE}
const
	class_TBox = 'TBox';
	
type
	PBox = ^TBox;
	TBox = object (TPanel)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TBox.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TBox) then 
		ObjectClass := class_TBox
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TBox.Create;
begin
	inherited Create(AParent, AName);
	FUseCaption := False;
	FBorder := True;
	FShadow := True;
	FStyle := bxDouble;
	AdjustMargins;
	FAttr.Click := FAttr.Normal;
end;
{$ENDIF}
