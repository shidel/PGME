{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgAbout --- }

{$IFDEF INTERFACE}
const
	class_TDlgAbout = 'TDlgAbout';

type 
	PDlgAbout = ^TDlgAbout;
	TDlgAbout = object (TDialog)
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
function TDlgAbout.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgAbout) then 
		ObjectClass := class_TDlgAbout
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgAbout.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.ABOUT', True);
	SetTitle('Create New Menu');
	SetHelpText('The "About" dialog box.');
	AddHelpText('');
	AddHelpText('This is dialog box provides some information regarding PGME.');
	AddHelpText('Additional information is available in the BLAHBLAH.TXT file');
	AddHelpText('and online at http://pgme.sourceforge.net');
	with PButton(FOK)^ do begin
		SetHelpText('5;Use the "%0" button to close the dialog box.');
	end;
end;

destructor TDlgAbout.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgAbout.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgAbout.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
