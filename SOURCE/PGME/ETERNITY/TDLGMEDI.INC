{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgMEdit --- }

{$IFDEF INTERFACE}
const
	class_TDlgMEdit = 'TDlgMEdit';

type 
	PDlgMEdit = ^TDlgMEdit;
	TDlgMEdit = object (TDlgMenu)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl);
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgMEdit.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgMEdit) then 
		ObjectClass := class_TDlgMEdit
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgMEdit.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.MENU.EDIT', False);
	SetTitle('Menu Options');
	SetHelpText('The "Menu Options" dialog box.');
	AddHelpText('');
	AddHelpText('This is where you can make changes to the current menu title');
	AddHelpText('and other settings.');
	with PButton(FOK)^ do begin
		SetHelpText('5;Use the "%0" button to apply the new settings to the current menu');
		AddHelpText('and close the dialog box.');
	end;
end;
{$ENDIF}
