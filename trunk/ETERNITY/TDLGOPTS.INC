{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgPGMOpt --- }

{$IFDEF INTERFACE}
const
	class_TDlgPGMOpt = 'TDlgPGMOpt';

type 
	PDlgPGMOpt = ^TDlgPGMOpt;
	TDlgPGMOpt = object (TDIALOG)
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
function TDlgPGMOpt.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgPGMOpt) then 
		ObjectClass := class_TDlgPGMOpt
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgPGMOpt.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.OPTIONS');
	Bounds(1,1,55,15, B);
	SetBounds(B);					
	SetTitle('Eternity Options');
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgPGMOpt.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgPGMOpt.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgPGMOpt.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
