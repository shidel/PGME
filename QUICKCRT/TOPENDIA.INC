{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TOpenDialog --- }

{$IFDEF INTERFACE}
const
	class_TOpenDialog = 'TOpenDialog';

type
	POpenDialog = ^TOpenDialog;
	TOpenDialog = object (TFileDialog)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
{--- TOpenDialog --- }
function TOpenDialog.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TOpenDialog) then 
		ObjectClass := class_TOpenDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TOpenDialog.Create;
var
	B : TBounds;
begin
	inherited Create(AParent, AName);
	SetTitle('Open File');

	FBOK.SetCaption('Open');

	FBMKDir.SetVisible(False);

	with FPath do begin
		B := FCBounds;
		Inc(B.Width, 14);
		B.Height := 10;
		SetBounds(B);
	end;

	with FDirs do begin
		B := FCBounds;
		Inc(B.Width, 14);
		SetBounds(B);
	end;

	SetHelpText('This is a standard "Open File" dialog box.');
	AddHelpText('');
	AddHelpText('You may use the dropdown path list to select one of the systems current');
	AddHelpText('active directories. Or, you may type in one for direct access. Also,' );
	AddHelpText('recently entered paths are stored into the list for quick selection.');
	AddHelpText('');
	AddHelpText('The filter list can be used filter the directory listings by file types.' );
	AddHelpText('They will be sorted in the order listed. Also, if a filename is entered manually' );
	AddHelpText('that contains wildcards, it will be added as a user filter.');
	AddHelpText('');
end;
{$ENDIF}
