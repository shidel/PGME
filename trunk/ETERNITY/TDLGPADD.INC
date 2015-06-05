{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgPAdd --- }

{$IFDEF INTERFACE}
const
	class_TDlgPAdd = 'TDlgPAdd';

type 
	PDlgPAdd = ^TDlgPAdd;
	TDlgPAdd = object (TDlgProg)
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
function TDlgPAdd.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgPAdd) then 
		ObjectClass := class_TDlgPAdd
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgPAdd.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.PROG.ADD', True);
	SetTitle('New Program');
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgPAdd.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgPAdd.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgPAdd.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
