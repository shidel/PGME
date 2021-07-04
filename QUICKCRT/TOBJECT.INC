{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TObject --- }

{$IFDEF INTERFACE}
const
	class_TObject : TObjectClass = 'TObject';

type
	PObject = ^TObject;
	TObject = object		
	private
		{$IFDEF STRINGPTRS}
		PNameID : PString;
		{$ELSE}
		FNameID : String[StringMax];
		{$ENDIF}
	public { protected }
		Parent : PObject;
		FFreeOnDestroy : boolean;
		procedure SetNameID ( AValue : String ); virtual;	
		function  GetFreeOnDestroy : boolean;
		procedure SetFreeOnDestroy(AValue : boolean);
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AName : String);
		destructor  Destroy; virtual;		
		function  GetNameID : String; 
		function  GetPathID : String; 
		function  GetClassID : String; 
		function  IsChild(AObject : PObject) : boolean; 
		function  IsChildOf(AObject : PObject) : boolean; 
		function  ClassOf( AClass : String ) : boolean; 
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TObject.ObjectClass(AName : String) : String; 
begin
	ObjectClass := class_TObject;
end;

constructor TObject.Create;
begin
	MemCheck(0);
	{$IFDEF STRINGPTRS}
	PNameID := StrPtr(AName);
	{$ELSE}
	FNameID := AName;
	{$ENDIF}
	Parent := nil;
	FFreeOnDestroy := True;
	{$IFDEF DEVLOG_CREATE}
		if GetClassID <> class_TListItem then
			WriteLog('Create ' + GetClassID + ' ' + GetPathID );
		{$IFDEF DEVLOG_CREATEINDENT}
			if GetClassID <> class_TListItem then LogInc;
		{$ENDIF}
	{$ENDIF}
end;

destructor TObject.Destroy;
begin
	MemCheck(0);
	{$IFDEF DEVLOG_CREATE}
		{$IFDEF DEVLOG_CREATEINDENT}
			if GetClassID <> class_TListItem then LogDec;
		{$ENDIF}
		if GetClassID <> class_TListItem then
			WriteLog('Destroy ' + GetClassID + ' ' + GetPathID);
	{$ENDIF}
	{$IFDEF STRINGPTRS}
	FreeStr(PNameID);
	{$ENDIF}
end;

function TObject.GetFreeOnDestroy : boolean;
begin
	GetFreeOnDestroy := FFreeOnDestroy;
end;

function TObject.GetNameID : String; 
begin
	{$IFDEF STRINGPTRS}
	if Assigned(PNameID) then 
		GetNameID := PtrStr(PNameID)
	else
		GetNameID := '';
	{$ELSE}
	GetNameID := FNameID;
	{$ENDIF}
end;

function TObject.GetPathID : String; 
begin
	if Assigned(Parent) then 			
		GetPathID := Parent^.GetPathID + '.' + GetNameID
	else
		GetPathID := GetNameID;
end;

procedure TObject.SetNameID ( AValue : String ); 
begin
	{$IFDEF STRINGPTRS}
	if Assigned(PNameID) then begin
		if PtrStr(PNameID) = AValue then exit;
		FreeStr(PNameID);
	end;
	PNameID := StrPtr(AValue);
	{$ELSE}
	FNameID := AValue;
	{$ENDIF}
end;

function TObject.GetClassID : String; 
begin
	GetClassID := ObjectClass('');
end;

procedure TObject.SetFreeOnDestroy(AValue : boolean);
begin
	FFreeOnDestroy := AValue;
end;

function TObject.IsChild(AObject : PObject) : boolean;
begin
	while Assigned(AObject) and (AObject <> @Self) do
		AObject := AObject^.Parent;
	IsChild := AObject = @Self;
end;

function TObject.IsChildOf(AObject : PObject) : boolean;
var
	Temp : PObject;
begin
	Temp := @Self;
	while Assigned(AObject) and (AObject <> Temp) do
		Temp := Temp^.Parent;
	IsChildOf := AObject = Temp;
end;

function  TObject.ClassOf( AClass : String ) : boolean;
begin
	ClassOf := ObjectClass(AClass) = AClass;
end;

{$ENDIF}
