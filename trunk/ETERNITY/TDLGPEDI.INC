{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgPEdit --- }

{$IFDEF INTERFACE}
const
	class_TDlgPEdit = 'TDlgPEdit';

type 
	PDlgPEdit = ^TDlgPEdit;
	TDlgPEdit = object (TDIALOG)
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
function TDlgPEdit.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgPEdit) then 
		ObjectClass := class_TDlgPEdit
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgPEdit.Create;
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.PROG.EDIT');
	Bounds(1,1,55,15, B);
	SetBounds(B);					
	SetTitle('Edit Program');
	SetHelpText('');
	AddHelpText('');
end;

destructor TDlgPEdit.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgPEdit.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgPEdit.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
