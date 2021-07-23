{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgMove --- }

{$IFDEF INTERFACE}
const
	class_TDlgMove = 'TDlgMove';

type 
	PDlgMove = ^TDlgMove;
	TDlgMove = object (TDialog)
    public { protected }
    	FName  : PLabel;
    	FCheck : PCheckBox;
    	FMenuDrop : PDropList;
    	FOK : PButton;
    	
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl);
		destructor Destroy; virtual;
		function Origin : Pointer; virtual;
		function Destination : Pointer; virtual;
		function MoveItem : boolean;
	end;   
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgMove.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgMove) then 
		ObjectClass := class_TDlgMove
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgMove.Create;
const
	ColorLabel = $70;
	ColorName = $71;
	ColorCheck = $70;
	ColorDrop = $1E;
	ColorOK = $2E;
var
	B, T : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.MOVE');
	Bounds(1,1,50,15, B);
{	SetStyle(bxSingle);}
	SetBounds(B);
	FAutoEndModal := True;
	SetTitle('Duplicate or Move Program');
	SetHelpText('The "Duplicate or Move" dialog box.');
	AddHelpText('');
	AddHelpText('This is dialog box provides a means to copy or move a menu entry from');
	AddHelpText('one menu onto another.');
	TextAttr := ColorLabel;
	Bounds(3, 2, B.Width - 8, 1, T);
	with PLabel(New(PLabel, Create(@Self, 'NAME.LABEL')))^ do begin
		SetBounds(T);
		SetCaption('Program Title:');
		SetAnchors(abTop or abRight or abLeft);
		FHAlign := alignLeft;
	end;
	TextAttr := ColorName;
	Inc(T.Top);
	FName := PLabel(New(PLabel, Create(@Self, 'NAME.TEXT')));
	with FName^ do begin
		SetBounds(T);
		SetCaption('a program');
		SetAnchors(abTop or abRight or abLeft);
		FHAlign := alignLeft;
	end;
	TextAttr := ColorLabel;
	Inc(T.Top, 2);
	with PLabel(New(PLabel, Create(@Self, 'TARGET.LABEL')))^ do begin
		SetBounds(T);
		SetCaption('Target Menu:');
		SetAnchors(abTop or abRight or abLeft);
		FHAlign := alignLeft;
	end;
	TextAttr := ColorDrop;
	Inc(T.Top);
	T.Height := 10;
	FMenuDrop := PDropList(New(PDropList, Create(@Self, 'TARGET.LIST')));
	with FMenuDrop^ do begin
		SetBounds(T);
		SetSorted(True);
		FEdit.SetReadOnly(True);
		SetHelpText('Use the "Target Menu" drop down list to select the menu where');
		AddHelpText('you wish to copy or move the programs menu entry.');
	end;
	TextAttr := ColorCheck;
	Inc(T.Top, 2);
	T.Height := 1;
	FCheck := PCheckBox(New(PCheckBox, Create(@Self, 'CHECKBOX.MOVE')));
	with FCheck^ do begin
		SetBounds(T);
		SetCaption('Move instead of copy.');
		SetAnchors(abTop or abRight or abLeft);
		FHAlign := alignLeft;
		SetHelpText('Use the "Move" checkbox to move the programs menu entry');
		AddHelpText('instead of duplicating it.');
	end;
	TextAttr := ColorOK;
	FOK := New(PButton, Create(@Self, 'BUTTON.OK'));
	with PButton(FOK)^ do begin
		Bounds(B.Left + (B.Width - 5) div 2 - 6 , B.Height - 4, 14, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abRight or abBottom);
		SetCaption('OK');
		SetCommand(cmOK);		
		SetHelpText('5;Use the "%0" button to copy or move the program');
		AddHelpText('and close the dialog box.');
	end;
end;

destructor TDlgMove.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgMove.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgMove.BeforeShow;
var
	D, M, I : PListItem;
begin
	inherited BeforeShow;
	FCheck^.SetState(0);
	FName^.SetCaption(PGME.FPrograms^.FCurrent^.GetNameID);
	FMenuDrop^.BeginUpdate;
	FMenuDrop^.ClearItems;
	D := PMenuCache(PGME.FMenuCache)^.FCache^.First;
	while Assigned(D) do begin
		M := D^.First;
		while Assigned(M) do begin
			if PCacheMenu(M)^.Available and PCacheMenu(M)^.Writable then begin
				I := FMenuDrop^.AddItem(PCacheMenu(M)^.ShortTitle);
				I^.Data := M;
			end;
			M := M^.Next;
		end;
		D := D^.Next;
	end;
	FMenuDrop^.SetCaption(PGME.FMenus^.FCurrent^.GetNameID);
	FMenuDrop^.EndUpdate;
end;

function TDlgMove.Origin : Pointer;
begin
	Origin := PMenuCache(PGME.FMenus^.FCurrent^.Data);
end;

function TDlgMove.Destination : Pointer;
begin
	Destination := PMenuCache(FMenuDrop^.FList.FList.FCurrent^.Data);
end;

function TDlgMove.MoveItem : boolean;
begin
	MoveItem := FCheck^.Checked;
end;
{$ENDIF}
