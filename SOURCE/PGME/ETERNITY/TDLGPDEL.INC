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
	TDlgPDel = object (TMessageBox)
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
	Bounds(1,1,45,9, B);
	SetBounds(B);					
	SetTitle('Remove Program');
	SetMessage(' ');
	AppendMessage('Are you sure you want to remove this');
	AppendMessage('program from the menu?');
	SetHelpText('The remove program dialog box.');
	FAutoEndModal := True;
	TextAttr := $2E;
	with PButton(New(PButton, Create(@Self, 'BUTTON.NO')))^ do begin
		Bounds(Self.Width - 16,Self.Height - 4 , 12,1, B);
		SetBounds(B);					
		SetCaption('Cancel');
		SetAnchors(abRight or abBottom);
		SetCommand(cmCancel);
		SetHelpText('5;"%0" Button: Will close the dialog and leave the program');
		AddHelpText('on the current menu.');
	end;
	TextAttr := $4E;
	with PButton(New(PButton, Create(@Self, 'BUTTON.OK')))^ do begin
		Dec(B.Left, 14);
		SetBounds(B);					
		SetCaption('Delete');
		SetAnchors(abRight or abBottom);
		SetCommand(cmOK);
		SetHelpText('5;"%0" Button: Will cause the current selected program to be');
		AddHelpText('removed from the current menu. It will not remove it from other');
		AddHelpText('menus or delete the program from the computer or drive. It only');
		AddHelpText('will remove it from this menu.');
	end;
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
