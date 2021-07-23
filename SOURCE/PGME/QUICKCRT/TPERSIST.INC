{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TPersistent --- }

{$IFDEF INTERFACE}
const
	class_TPersistent : TObjectClass = 'TPersistent';

type
	PPersistent = ^TPersistent;
	TPersistent = object ( TObject )
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TPersistent.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TPersistent) then 
		ObjectClass := class_TPersistent
	else
		ObjectClass := inherited ObjectClass(AName);
end;

{$ENDIF}
