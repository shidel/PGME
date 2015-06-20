{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgDOSCommand --- }

{$IFDEF INTERFACE}
const
	class_TDlgDOSCommand = 'TDlgDOSCommand';

type 
	PDlgDOSCommand = ^TDlgDOSCommand;
	TDlgDOSCommand = object (TDialog)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl);
		destructor Destroy; virtual;
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgDOSCommand.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgDOSCommand) then 
		ObjectClass := class_TDlgDOSCommand
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgDOSCommand.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.DOSCOMMAND', True);
	SetTitle('Create New Menu');
	SetHelpText('The "DOS Command" dialog box.');
	AddHelpText('');
	AddHelpText('This is where you manually enter a DOS command to the program');
	AddHelpText('execution process.');
	with PButton(FOK)^ do begin
		SetHelpText('5;Use the "%0" button to add the DOS command');
		AddHelpText('and close the dialog box.');
	end;
end;

destructor TDlgDOSCommand.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgDOSCommand.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgDOSCommand.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
