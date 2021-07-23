{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TMemo --- }

{$IFDEF INTERFACE}
const
	class_TMemo = 'TMemo';
	
type
	PMemo = ^TMemo;
	TMemo = object(TPanel)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TMemo.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TMemo) then 
		ObjectClass := class_TMemo
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TMemo.Create;
begin
	inherited Create(AParent, AName);
end;
{$ENDIF}
