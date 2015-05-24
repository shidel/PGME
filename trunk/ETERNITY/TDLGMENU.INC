{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgMenu --- }

{$IFDEF INTERFACE}
const
	class_TDlgMenu = 'TDlgMenu';

type 
	PDlgMenu = ^TDlgMenu;
	TDlgMenu = object (TDIALOG)
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
function TDlgMenu.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgMenu) then 
		ObjectClass := class_TDlgMenu
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgMenu.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, AName);
	Bounds(1,1,55,15, B);
	SetBounds(B);					
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgMenu.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgMenu.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgMenu.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
