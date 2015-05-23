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
	TDlgMAdd = object (TDIALOG)
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
	inherited Create(AParent, 'DIALOG.MENU.ADD');
	Bounds(1,1,55,15, B);
	SetBounds(B);					
	SetTitle('New Menu');
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgMAdd.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgMAdd.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgMAdd.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
