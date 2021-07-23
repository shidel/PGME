{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TComponent --- }

{$IFDEF INTERFACE}
const
	class_TComponent = 'TComponent';
	
type
	PComponent = ^TComponent;
	TComponent = object(TListItem)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PComponent; AName : String);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TComponent.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TComponent) then 
		ObjectClass := class_TComponent
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TComponent.Create;
begin
	inherited Create(AName);
	if Assigned(AParent) then AParent^.Add(@Self);
end;
{$ENDIF}
