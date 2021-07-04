{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TSlider --- }

{$IFDEF INTERFACE}
const
	class_TSlider = 'TSlider';
	
type
	PSlider = ^TSlider;
	TSlider = object(TScrollBar)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TSlider.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TSlider) then 
		ObjectClass := class_TSlider
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TSlider.Create;
begin
	inherited Create(AParent, AName);
	FEnds := False;
	FEndOver := True;
	FThumb := #$DB#$DB#$DB;
end;
{$ENDIF}
