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
	TDlgMDel = object (TMessageBox)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl);
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
	Bounds(1,1,45,9, B);
	SetBounds(B);					
	SetTitle('Delete Menu');
	SetMessage(' ');
	AppendMessage('Are you sure you want to delete this');
	AppendMessage('menu from the computer?');
	SetHelpText('The remove program dialog box.');
	FAutoEndModal := True;
	TextAttr := $2E;
	with PButton(New(PButton, Create(@Self, 'BUTTON.NO')))^ do begin
		Bounds(Self.Width - 16,Self.Height - 4 , 12,1, B);
		SetBounds(B);					
		SetCaption('Cancel');
		SetAnchors(abRight or abBottom);
		SetCommand(cmCancel);
		SetHelpText('5;"%0" Button: Will close the dialog and leave the menu');
		AddHelpText('intact.');
	end;
	TextAttr := $4E;
	with PButton(New(PButton, Create(@Self, 'BUTTON.OK')))^ do begin
		Dec(B.Left, 14);
		SetBounds(B);					
		SetCaption('Delete');
		SetAnchors(abRight or abBottom);
		SetCommand(cmOK);
		SetHelpText('5;"%0" Button: Will cause the current selected menu and all of its');
		AddHelpText('programs to be removed from the computer.');
		AddHelpText('It will not remove it other');
		AddHelpText('menus or delete the programs from the computer or its drive. It only');
		AddHelpText('will remove this menu and its entries.');
	end;
end;

{$ENDIF}
