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
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl);
		destructor Destroy; virtual;
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
	inherited Create(AParent, 'DIALOG.MENU.EDIT');
	Bounds(1,1,55,15, B);
	SetBounds(B);					
	SetTitle('Menu Options');
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgMEdit.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgMEdit.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgMEdit.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
