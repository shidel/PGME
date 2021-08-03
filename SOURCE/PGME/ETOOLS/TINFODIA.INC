{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$IFDEF INTERFACE}
const
	class_TInfoDialog = 'TInfoDialog';

type
	PInfoDialog = ^TInfoDialog;
	TInfoDialog = object (TDialog)
	public { protected }
			FKeys : PDropList;
			FValue : PEdit;
			FList : PListItem;
		function  ObjectClass ( AName : String ) : String; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
	end;

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TInfoDialog.ObjectClass ( AName : String ) : String;
begin
	if (AName = '') or (AName = class_TInfoDialog) then
		ObjectClass := class_TInfoDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TInfoDialog.Create;
var
	B : TBounds;
begin
	inherited Create(AParent, AName);
	Bounds(1,1,50,14, B);
	SetBounds(B);
	SetTitle('Font Information');
	SetHelpText('Font Information Dialog.');
	AddHelpText('');
	AddHelpText('With this dialog you can view and modify font metadata for the current');
	AddHelpText('font. Please note that blank fields are automatically removed.');

	TextAttr := $70;
	with PLabel(New(PLabel, Create(@Self, 'KEYS.LABEL')))^ do begin
		Bounds(3, 3, Self.Width - 8, 1, B);
		SetBounds(B);
		FHAlign := AlignLeft;
		SetCaption('Font Information Fields:');
	end;

	TextAttr := $1E;
	FKeys := New(PDropList, Create(@Self, 'KEYS.DROPLIST'));
	with FKeys^ do begin
		SetHelpText('"Fields" Dropdown list. Use to select any of the fonts metadata');
		AddHelpText('fields to be modified. Also, manually entering text will automatically');
		AddHelpText('add that field to the fonts metadata.');
		Bounds(3, 4, Self.Width - 8, 10, B);
		SetBounds(B);
	end;

	TextAttr := $70;
	with PLabel(New(PLabel, Create(@Self, 'VALUES.LABEL')))^ do begin
		Bounds(3, 6, Self.Width - 8, 1, B);
		SetBounds(B);
		FHAlign := AlignLeft;
		SetCaption('Field Value:');
	end;

	TextAttr := $1E;
	FValue := New(PEdit, Create(@Self, 'VALUES.EDIT'));
	with FValue^ do begin
		SetHelpText('"Value" Edit. Use to select modify the selected fields metadata.');
		AddHelpText('If left blank, it will cause the field to be automatically');
		AddHelpText('removed.');
		Bounds(3, 7, Self.Width - 8, 1, B);
		SetBounds(B);
	end;

	TextAttr := $2E;
	with PButton(New(PButton, Create(@Self, 'OK')))^ do begin
		SetHelpText('5;"%0" Button. To accept the changes and modify fonts metadata.');
		Bounds((Self.Width - 2) div 2 - 5, Self.Height - 4 , 10, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abBottom);
		SetCaption('OK');
		SetCommand(cmOK);
	end;

	FList := New(PListItem, Create('ITEMS'));
end;

destructor TInfoDialog.Destroy;
begin
	inherited Destroy;
	Dispose(FList, Destroy);
	FList := nil;
end;

procedure TInfoDialog.CheckLocal(var AEvent : TEvent);
var
	V : integer;
	P : PListItem;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmEdit : begin
				if AEvent.InfoPtr = FValue then begin
					{ Value Edited }
					ClearEvent(AEvent);
					P := FList^.First;
					while Assigned(P) and (P^.GetNameID <> FKeys^.GetCaption) do
						P := P^.Next;
					if Assigned(P) and Assigned(P^.First)  then
						P^.First^.SetNameID(FValue^.GetCaption)
					else
						FValue^.SetCaption('');
				end else if AEvent.InfoPtr = FKeys then begin
					{ Key Edited }
					FKeys^.SetCaption(Trim(Ucase(FKeys^.GetCaption)));
					P := FList^.First;
					while Assigned(P) and (P^.GetNameID <> FKeys^.GetCaption) do
						P := P^.Next;
					if Assigned(P) and Assigned(P^.First)  then
						FValue^.SetCaption(P^.First^.GetNameID)
					else begin
						FValue^.SetCaption('');
						FKeys^.AddItem(FKeys^.GetCaption);
						P := New(PListItem, Create(FKeys^.GetCaption));
						FList^.Add(P);
						P^.Add(New(PListItem, Create('')));
					end;

					ClearEvent(AEvent);
				end;

			end;
			cmDropList : begin
				if AEvent.InfoPtr = FKeys then begin
					{ Change Value To Key's Value }
					ClearEvent(AEvent);
					P := FList^.First;
					while Assigned(P) and (P^.GetNameID <> FKeys^.GetCaption) do
						P := P^.Next;
					if Assigned(P) and Assigned(P^.First)  then
						FValue^.SetCaption(P^.First^.GetNameID)
					else
						FValue^.SetCaption('');
				end;
			end;
		end;
	inherited CheckLocal(AEvent);
end;

{$ENDIF}
