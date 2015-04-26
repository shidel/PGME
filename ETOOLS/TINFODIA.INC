{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$IFDEF INTERFACE}
const
	class_TInfoDialog = 'TInfoDialog';

type
	PInfoDialog = ^TInfoDialog;
	TInfoDialog = object (TDialog)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
	
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TInfoDialog.ObjectClass ( AName : String ) : String; 
begin
	if (AName = '') or (AName = class_TInfoDialog) then 
		ObjectClass := class_TInfoDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TInfoDialog.Create;
var
	B : TBounds;
begin
	inherited Create(AParent, AName);
	Bounds(1,1,40,13, B);
	SetBounds(B);
	SetTitle('Font Information');	
end;

{$ENDIF}
