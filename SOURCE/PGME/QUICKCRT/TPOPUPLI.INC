{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TPopUpList --- }

{$IFDEF INTERFACE}
const
	class_TPopUpList = 'TPopUpList';
	
type
	PPopUpList = ^TPopUpList;
	TPopUpList = object(TPopUp)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
			FList : TListView;
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		function AddItem(AValue : String) : PListItem; virtual;
		function GetCurrent : PListItem; virtual;
		procedure ClearItems; virtual;
		procedure SetSorted(AValue : boolean); virtual;
		procedure ScrollTo(AValue : String); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TPopUpList.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TPopUpList) then 
		ObjectClass := class_TPopUpList
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TPopUpList.Create;
var
	B : TBounds;
	C : integer;
begin
	C := TextAttr;
	inherited Create(AParent, AName);
	TextAttr := C;
	FList.Create(@Self, 'LIST');
	FList.SetFreeOnDestroy(False);
	Bounds(1,1,1,1,B);
	FList.SetBounds(B);
	FList.SetAnchors(alClient);
	FList.SetScrollEdge(True);
{	FList.FVScroll.FVisible := False; }
end;

destructor TPopUpList.Destroy;
begin
	FList.Destroy;
	inherited Destroy;
end;

function TPopUpList.AddItem(AValue : String) : PListItem;
begin
	AddItem := FList.AddItem(AValue);
end;

procedure TPopUpList.SetSorted(AValue : boolean);
begin
	FList.SetSorted(AValue);
end;

procedure TPopUpList.CheckLocal(var AEvent : TEvent); 
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmListView, cmListViewDouble : if AEvent.InfoPtr = @FList then begin
				ClearEvent(AEvent); 
				FResult := mrOK;
				FStopModal := True;
			end;
			cmListViewChange : if Assigned(Parent) and 
			(AEvent.InfoPtr = @FList) then begin
				{ Kinda Kludgy, but works }
				if Parent^.ClassOf(class_TDropList) then begin
					PDropList(Parent)^.SetFromList;
					ClearEvent(AEvent); 
				end;
			end;
		end;
	inherited CheckLocal(AEvent);
end;

function TPopUpList.GetCurrent : PListItem;
var
	P : PListItem;
begin
	P := FList.GetCurrent;
	GetCurrent := P;
end;

procedure TPopUpList.ClearItems;
begin
	FList.ClearItems;
end;

procedure TPopUpList.ScrollTo(AValue : String);
begin
	FList.ScrollTo(AValue);
end;

{$ENDIF}
