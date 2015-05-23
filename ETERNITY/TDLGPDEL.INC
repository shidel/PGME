{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgPDel --- }

{$IFDEF INTERFACE}
const
	class_TDlgPDel = 'TDlgPDel';

type 
	PDlgPDel = ^TDlgPDel;
	TDlgPDel = object (TDIALOG)
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
function TDlgPDel.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgPDel) then 
		ObjectClass := class_TDlgPDel
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgPDel.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.PROG.DELETE');
	Bounds(1,1,55,15, B);
	SetBounds(B);					
	SetTitle('Remove Program');
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgPDel.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgPDel.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgPDel.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
