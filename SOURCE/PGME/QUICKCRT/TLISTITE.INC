{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TLists --- }

{$IFDEF INTERFACE}
const
	class_TListItem : TObjectClass = 'TListItem';

type
    PListItem = ^TListItem;
    TListItem = object(TPersistent)
    private
    	FLastFound : PListItem;
    	function GetIndexOfChild(AItem : PListItem) : integer; virtual;
    public { protected }
		procedure SetNameID ( AValue : String ); virtual;	
		function  ObjectClass ( AName : String ) : String; virtual;
    public
        Next, Prior, First, Last : PListItem;
        Count : integer;
        Sorted : boolean;
        Data : Pointer;
      constructor Create(AName : String);
      destructor  Destroy; virtual;
      function    Compare (ToItem : PListItem) : integer; virtual;
      function 	  GetIndex : integer; virtual;
      procedure   Add(AItem : PListItem); virtual;
      procedure   Remove(AItem : PListItem); virtual;
      function    FindID(AValue : String) : PListItem; virtual;
      procedure   DestroyData; virtual;
      procedure   Clear; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TListItem.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TListItem) then 
		ObjectClass := class_TListItem
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TListItem.Create(AName : String);
begin
	inherited Create(AName);
	FLastFound := nil;
	Next := nil;
	Prior := nil;
	First := nil;
	Last := nil;
	Data := nil;
	Count := 0;
	Sorted := False;
end;

destructor TListItem.Destroy; 	
begin
	Clear;
	DestroyData;
	inherited Destroy;
end;

procedure TListItem.SetNameID ( AValue : String );
var
	P : PObject;
begin
	if Assigned(Parent) and (Parent^.ClassOf(class_TListItem)) and
	PListItem(Parent)^.Sorted then begin
		P := Parent;
		PListItem(P)^.Remove(@Self);
		inherited SetNameID(AValue);
		PListItem(P)^.Add(@Self);
	end else 
		inherited SetNameID(AValue);
end;

function TListItem.Compare (ToItem : PListItem) : integer; 
var
	F, T : String;
begin
	F := GetNameID;
	T := ToItem^.GetNameID;
	if F < T then 
		Compare := cmpLessThan
	else if F > T then
		Compare := cmpGreaterThan
	else if F = T then
		Compare := cmpEqual
	else
		Compare := cmpUnknown;
end;

function TListItem.GetIndex : integer; 
begin
	if not Assigned(Parent) then 
		GetIndex := 0
	else
		GetIndex := PListItem(Parent)^.GetIndexOfChild(@Self);
end;

function TListItem.GetIndexOfChild(AItem : PListItem) : integer;
var
	P : PListItem;
	I : integer;
begin
	I := 0;
	P := First;
	while (Assigned(P) and (P <> AItem)) do begin
		Inc(I);
		P := P^.Next;
	end;
	if (Assigned(P) and (P = AItem)) then
		GetIndexOfChild := I
	else
		GetIndexOfChild := -1;
end;

procedure TListItem.Add(AItem : PListItem);
var	
  P : PListItem;
begin
	if not Assigned(AItem) then exit;
	if Assigned(AItem^.Parent) then 
		PListItem(AItem^.Parent)^.Remove(AItem);
	{$IFDEF DEVLOG_LISTITEMS}
		WriteLog('Add list (' + GetPathID +') + ' + AItem^.GetNameID);
	{$ENDIF}
	if (not Sorted) or (not Assigned(Last)) then
		begin
			if Assigned(Last) then
				Last^.Next := AItem;
			AItem^.Parent := @Self;
			AItem^.Prior := Last;
			Last := AItem;
			if not Assigned(First) then First := Last;
		end
	else
		begin
			if AItem^.Compare(First) = cmpLessThan then begin
				AItem^.Next := First;
				First^.Prior := AItem;
				First := AItem;
			end else if AItem^.Compare(Last) >= cmpEqual then begin
				AItem^.Prior := Last;
				Last^.Next := AItem;
				Last := AItem;
			end else begin
				P := First;
				while Assigned(P) and (AItem^.Compare(P) >= cmpEqual) do P := P^.Next;
				if not assigned(P) then begin	
					{ Should never be here if Compare function is not broken }
					Sorted := False;
					Add(AItem);
					exit;
				end;
				AItem^.Prior := P^.Prior;
				AItem^.Next := P;
				P^.Prior := AItem;
				AItem^.Prior^.Next := AItem;
			end;
			AItem^.Parent := @Self;
		end;
		
	Inc(Count);
end;

procedure TListItem.Remove(AItem : PListItem);
begin
	if not Assigned(AItem) then exit;
	if AItem^.Parent <> @Self then exit; 
	
	if FLastFound = AItem then
		FLastFound := AItem^.Next;
		
	if Assigned(AItem^.Prior) then
		AItem^.Prior^.Next := AItem^.Next;
	if Assigned(AItem^.Next) then
		AItem^.Next^.Prior := AItem^.Prior;
		
	if AItem = First then
		First := AItem^.Next;
	if AItem = Last then
		Last := AItem^.Prior;
		
	AItem^.Parent := nil;
	AItem^.Prior := nil;
	AItem^.Next := nil;
	
	Dec(Count);
end;

function TListItem.FindID( AValue : String) : PListItem; 
var
	P : PListItem;
begin
	if Sorted and Assigned(FLastFound) then begin
		P := FLastFound;
		while Assigned(P) and (P^.GetNameID >= AValue) and (Assigned(P^.Prior)) do
			P := P^.Prior;
		while Assigned(P) and (P^.GetNameID < AValue) and (Assigned(P^.Next)) do
			P := P^.Next;
		if Assigned(P) and (P^.GetNameID <> AValue) then
			P := nil;
	end else begin 
		P := First;
		while Assigned(P) and (P^.GetNameID <> AValue) do
			P := P^.Next;
	end; 
	FLastFound := P;
	FindID := FLastFound;
end;

procedure TListItem.DestroyData;
begin
	Data := nil;
end;

procedure TListItem.Clear;
var
	P, T : PListItem;
begin
	P := First;
	while Assigned(P) do begin
		T := P^.Next;
		Remove(P); 
		if P^.GetFreeOnDestroy then
			Dispose(P, Destroy)
		else
			P^.Destroy;
		P := T;
	end;
	FLastFound := nil;
	First := nil;
	Last := Nil;
end;
{$ENDIF}
