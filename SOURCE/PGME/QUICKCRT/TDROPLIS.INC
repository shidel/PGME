{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ ---TDropList --- }

{$IFDEF INTERFACE}
const
	class_TDropList = 'TDropList';

type
	PDropList = ^TDropList;
	TDropList = object(TControl)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		procedure SetBounds(AValue : TBounds); virtual;
		procedure AdjustChildren; virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure SetFromList; virtual;
	public
			FEdit : TEdit;
			FDrop : TButton;
			FList : TPopUpList;
			FMaxDrop : integer;
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure Draw; virtual;
		function AddItem( AValue : String) : PListItem; virtual;
		procedure SetSorted(AValue : boolean); virtual;
		procedure SetCaption(AValue: String); virtual;
		function GetCaption : String; virtual;
		procedure SetReadOnly(AValue: Boolean); virtual;
		function GetReadOnly : Boolean; virtual;
		procedure ClearItems; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDropList.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TDropList) then
		ObjectClass := class_TDropList
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDropList.Create;
var
	B : TBounds;
	C : integer;
begin
	C := TextAttr;
	inherited Create(AParent, AName);
	TextAttr := C;
	Bounds(1,1,1,1,B);
	FEdit.Create(@Self, 'EDIT');
	with FEdit do begin
		SetFreeOnDestroy(False);
		FRBounds := B;
	end;
	TextAttr := C;
	FDrop.Create(@Self, 'BUTTON');
	with FDrop do begin
		FCaptionAsChar := True;
		SetFreeOnDestroy(False);
		FCaption := #$20#$1F#$20;
		FRBounds := B;
		FCommand := cmDoDropList;
	end;
	TextAttr := C;
	FList.Create(@Self, 'LIST');
	with FList do begin
		SetFreeOnDestroy(False);
		FList.SetAllowEmpty(False); { FList.FList }
	end;
	FMaxDrop := 5;

end;

destructor TDropList.Destroy;
begin
	FList.Destroy;
	FDrop.Destroy;
	FEdit.Destroy;
	inherited Destroy;
end;

procedure TDropList.AdjustChildren;
var
	AValue : TBounds;
begin
	GetBounds(AValue);
	AValue.Top := 1;
	AValue.Left := 1;

	Dec(AValue.Width, Length(FDrop.GetCaption));
	FEdit.SetBounds(AValue);
	AValue.Left := AValue.Width + 1;
	AValue.Width := Length(FDrop.GetCaption);
	FDrop.SetBounds(AValue);

	AValue.Left := 3;
	Inc(AValue.Top);
	AValue.Height := FMaxDrop;
	AValue.Width := Width;
	if not FList.GetShadow then Dec(AValue.Width, 2);
	FList.SetBounds(AValue);
end;

procedure TDropList.Draw;
begin
	inherited Draw;
	AdjustChildren;
end;

procedure TDropList.SetBounds(AValue : TBounds);
begin
	FMaxDrop := AValue.Height - 1;
	AValue.Height := 1;
	inherited SetBounds(AValue);
	AdjustChildren;
end;

procedure TDropList.SetFromList;
var
	S : String;
	P : PListItem;
begin
	S := '';
	P := FList.GetCurrent;
	if Assigned(P) then begin
		S := P^.GetNameID;
	end else begin
		if not FList.FList.GetAllowEmpty then
			FList.FList.Up(True);
		P := FList.GetCurrent;
		if Assigned(P) then
			S := P^.GetNameID;
	end;
	FEdit.SetCaption(S);
	FEdit.Update;
end;

procedure TDropList.CheckLocal(var AEvent : TEvent);
var
	R : integer;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmDown : if (AEvent.InfoPtr = @FEdit) or
			(Application^.FFocused = @Self) then begin
				PutCommand(cmDoDropList, @FDrop);
				ClearEvent(AEvent);
			end;
			cmDoDropList : if (AEvent.InfoPtr = @FDrop) then begin
				FList.FList.ScrollTo(FEdit.GetCaption);
				FList.Update;
				R := FList.ShowModal;
				ClearEvent(AEvent);
				if R = mrOK then begin
					SetFromList;
					PutCommand(cmDropList, @Self);
				end;
			end;
			cmEdit : begin
				if (AEvent.InfoPtr = @FEdit) and GetReadOnly then begin
					ClearEvent(AEvent);
					PutCommand(cmDoDropList, @FDrop);
				end else
				if AEvent.InfoPtr = @FEdit then begin
					FList.FList.ScrollTo(FEdit.GetCaption);
					ClearEvent(AEvent);
					PutCommand(cmEdit, @Self);
				end;
			end;
		end;
	inherited CheckLocal(AEvent);
end;

function TDropList.AddItem( AValue : String) : PListItem;
begin
	if FList.FList.FItems.FindID(AValue) = nil then
		AddItem := FList.AddItem(AValue)
	else
		AddItem := nil;
end;

procedure TDropList.SetSorted(AValue : boolean);
begin
	FList.SetSorted(AValue);
end;

procedure TDropList.SetCaption(AValue: String);
begin
	FEdit.SetCaption(AValue);
	FList.ScrollTo(AValue);
end;

function TDropList.GetCaption : String;
begin
	GetCaption := FEdit.GetCaption;
end;

procedure TDropList.SetReadOnly(AValue: Boolean);
begin
	FEdit.SetReadOnly(AValue);
end;

function TDropList.GetReadOnly : Boolean;
begin
	GetReadOnly := FEdit.GetReadOnly;
end;

procedure TDropList.ClearItems;
begin
	FList.ClearItems;
end;
{$ENDIF}
