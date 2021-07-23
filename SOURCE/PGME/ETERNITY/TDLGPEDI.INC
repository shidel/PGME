{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgPEdit --- }

{$IFDEF INTERFACE}
const
	class_TDlgPEdit = 'TDlgPEdit';

type 
	PDlgPEdit = ^TDlgPEdit;
	TDlgPEdit = object (TDlgProg)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl);
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgPEdit.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgPEdit) then 
		ObjectClass := class_TDlgPEdit
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgPEdit.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.PROG.EDIT', False);
	SetTitle('Edit Program');
	SetHelpText('The "Edit Program" dialog box is for updating the currently');
	AddHelpText('selected program.');
end;

{$ENDIF}
