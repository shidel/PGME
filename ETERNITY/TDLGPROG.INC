{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgProg --- }

{$IFDEF INTERFACE}
const
	class_TDlgProg = 'TDlgProg';

type 
	PDlgProg = ^TDlgProg;
	TDlgProg = object (TDIALOG)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgProg.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgProg) then 
		ObjectClass := class_TDlgProg
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgProg.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, AName);
	Bounds(1,1,55,15, B);
	SetBounds(B);					
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgProg.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgProg.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgProg.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
