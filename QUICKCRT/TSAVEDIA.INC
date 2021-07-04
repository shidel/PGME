{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TSaveDialog --- }

{$IFDEF INTERFACE}
const
	class_TSaveDialog = 'TSaveDialog';

type
	PSaveDialog = ^TSaveDialog;
	TSaveDialog = object (TFileDialog)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;

{$ENDIF}

{$IFDEF IMPLEMENTATION}
{--- TSaveDialog --- }
function TSaveDialog.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TSaveDialog) then 
		ObjectClass := class_TSaveDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TSaveDialog.Create;
begin
	inherited Create(AParent, AName);
	SetTitle('Save File As');
	FMustExist := False;
	FBOK.SetCaption('Save As');

	SetHelpText('This is a standard "Save File As" dialog box.');
	AddHelpText('');
	AddHelpText('You may use the dropdown path list to select one of the systems current');
	AddHelpText('active directories. Or, you may type in one for direct access. Also,' );
	AddHelpText('recently entered paths are stored into the list for quick selection.');
	AddHelpText('');
	AddHelpText('When a specified directory does not exist, the Make Dir button will' );
	AddHelpText('become available. ' );
	AddHelpText('');
	AddHelpText('The filter list can be used filter the directory listings by file types.' );
	AddHelpText('They will be sorted in the order listed. Also, if a filename is entered manually' );
	AddHelpText('that contains wildcards, it will be added as a user filter.');
	AddHelpText('');

end;
{$ENDIF}
