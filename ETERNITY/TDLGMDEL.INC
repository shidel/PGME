{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgMDel --- }

{$IFDEF INTERFACE}
const
	class_TDlgMDel = 'TDlgMDel';

type 
	PDlgMDel = ^TDlgMDel;
	TDlgMDel = object (TDIALOG)
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
function TDlgMDel.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgMDel) then 
		ObjectClass := class_TDlgMDel
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgMDel.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.MENU.DELETE');
	Bounds(1,1,55,15, B);
	SetBounds(B);					
	SetTitle('Delete Menu');
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgMDel.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgMDel.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgMDel.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
