{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgMAdd --- }

{$IFDEF INTERFACE}
const
	class_TDlgMAdd = 'TDlgMAdd';

type 
	PDlgMAdd = ^TDlgMAdd;
	TDlgMAdd = object (TDlgMenu)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
 	public
		constructor Create(AParent : PControl);
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgMAdd.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgMAdd) then 
		ObjectClass := class_TDlgMAdd
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgMAdd.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.MENU.ADD', True);
	SetTitle('Create New Menu');
	SetHelpText('The "Create New Menu" dialog box.');
	AddHelpText('');
	AddHelpText('This is where you can create a new menu and set its title');
	AddHelpText('and other settings.');
	with PButton(FOK)^ do begin
		SetHelpText('5;Use the "%0" button to create the new menu, apply these settings');
		AddHelpText('and close the dialog box.');
	end;
end;

{$ENDIF}
