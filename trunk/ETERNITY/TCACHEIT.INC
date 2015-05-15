{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

type
	PCacheItem = ^TCacheItem;
	TCacheItem = object (TListItem)
		function    Read(var F : TFile) : boolean; virtual;
		function    Write(var F : TFile) : boolean; virtual;
		function    ReadSelf(var F : TFile) : boolean; virtual;
		function    WriteSelf(var F : TFile) : boolean; virtual;
		function	NewChild(AID : String) : PListItem; virtual;
	end;
	
	PCacheDir = ^TCacheDir;
	TCacheDir = object(TCacheItem)
		function	NewChild(AID : String) : PListItem; virtual;
	end;
	
	PCacheMenu = ^TCacheMenu;
	TCacheMenu = object (TCacheItem)
			LastMod : longint;	
			Fixed,
			Available,
			Writable : boolean;
			FShortTitle,
			FLongTitle : PString;
		constructor Create(AName : String);
		destructor Destroy; virtual;
		function    ReadSelf(var F : TFile) : boolean; virtual;
		function    WriteSelf(var F : TFile) : boolean; virtual;
		function	NewChild(AID : String) : PListItem; virtual;
		function ShortTitle : String;
		function LongTitle : String;
		procedure SetShortTitle(AValue : String);
		procedure SetLongTitle(AValue : String);
	end;
	
	PCacheApp = ^TCacheApp;
	TCacheApp = object (TCacheItem)
		FClassName,
		FFilename,
		FGroupName,
		FTitle	: PString;
		MaxMemory, 
		SimpleExec : boolean;
		constructor Create(AName : String);
		destructor Destroy; virtual;
		function	NewChild(AID : String) : PListItem; virtual;
	end;

	PCacheProc = ^TCacheProc;
	TCacheProc = object (TCacheItem)
		function	NewChild(AID : String) : PListItem; virtual;
	end;

function TCacheItem.NewChild(AID : String) : PListItem;
begin
	NewChild := New(PCacheDir, Create(AID));
end;

function TCacheItem.ReadSelf(var F : TFile) : boolean;
begin
	SetNameID(F.GetString);
	Writeln(GetNameID); 
	ReadSelf := F.IOResult = 0;
end;
	
function TCacheItem.WriteSelf(var F : TFile) : boolean;
var
	S : String;
begin
	S := GetNameID;
	F.PutString(S);
	WriteSelf := F.IOResult = 0;
end;
	
function TCacheItem.Read(var F : TFile) : boolean;
var
	P, T : PListItem;
	OK : boolean;
	B : Boolean;
	S : String;
begin
	OK := True;
	B := True;
	OK := ReadSelf(F);
	while OK and B do begin
		F.ReadBoolean(B);
		OK := (F.IOResult = 0);
		if OK and B then begin
			T := NewChild(S);	
			T^.Sorted := True;
			OK := PCacheItem(T)^.Read(F);
			Add(T);
		end;
	end;
	Read := OK;
end;

function TCacheItem.Write(var F : TFile) : boolean;
var
	P : PListItem;
	OK : boolean;
	B : Boolean;
	S : String;
begin
	B := True;
	OK := WriteSelf(F);

	P := First;	
	while OK and Assigned(P) do begin
		F.WriteBoolean(B);
		OK := IOResult = 0;
		if OK then 
			OK := PCacheItem(P)^.Write(F);
		P := P^.Next;
	end;
	
	if OK then begin
		B := False;
		F.WriteBoolean(B);
		OK := IOResult = 0;
	end;

	Write := OK;
end;

function TCacheDir.NewChild(AID : String) : PListItem;
begin
	NewChild := New(PCacheMenu, Create(AID));
end;

constructor TCacheMenu.Create(AName : String);
begin
	inherited Create(AName);
	
	LastMod := -1;	
	Fixed := False;
	Available := True;
	Writable := True;
	FShortTitle := nil;
	FLongTitle := nil;
end;

destructor TCacheMenu.Destroy;
begin
	FreeStr(FShortTitle);
	FreeStr(FLongTitle);
	inherited Destroy;
end;
	
function TCacheMenu.ReadSelf(var F : TFile) : boolean; 
var
	OK : boolean;
begin
	OK := inherited ReadSelf(F);
	if OK then begin
		F.ReadLongInt(LastMod);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.ReadBoolean(Fixed);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.ReadBoolean(Available);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.ReadBoolean(Writable);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		SetShortTitle (F.GetString);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		SetLongTitle (F.GetString);
		OK := F.IOResult = 0;
	end;
	ReadSelf := OK;
end;

function TCacheMenu.WriteSelf(var F : TFile) : boolean; 
var
	OK : boolean;
	S : String;
begin
	OK := inherited WriteSelf(F);
	if OK then begin
		F.WriteLongInt(LastMod);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.WriteBoolean(Fixed);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.WriteBoolean(Available);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.WriteBoolean(Writable);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		S := ShortTitle;
		F.PutString(S);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		S := LongTitle;
		F.PutString(S);
		OK := F.IOResult = 0;
	end;
	WriteSelf := OK;
end;

function TCacheMenu.ShortTitle : String;
begin
	if Assigned(FShortTitle) then
		ShortTitle := PtrStr(FShortTitle)
	else
		ShortTitle := '';
end;

function TCacheMenu.LongTitle : String;
begin
	if Assigned(FLongTitle) then
		LongTitle := PtrStr(FLongTitle)
	else
		LongTitle := '';
end;

procedure TCacheMenu.SetShortTitle(AValue : String);
begin
	FreeStr(FShortTitle);
	FShortTitle := StrPtr(AValue);
end;

procedure TCacheMenu.SetLongTitle(AValue : String);
begin
	FreeStr(FLongTitle);
	FLongTitle := StrPtr(AValue);
end;

function TCacheMenu.NewChild(AID : String) : PListItem; 
begin
	NewChild := New(PCacheApp, Create(AID));
end;

constructor TCacheApp.Create(AName : String);
begin
	inherited Create(AName);
	FClassName := nil;
	FFilename := nil;
	FGroupName := nil;
	FTitle:= nil;
	MaxMemory := false;
	SimpleExec := true;
end;

destructor TCacheApp.Destroy;
begin
	FreeStr(FClassName);
	FreeStr(FFilename);
	FreeStr(FGroupName);
	FreeStr(FTitle);
	inherited Destroy;
end;

function TCacheApp.NewChild(AID : String) : PListItem;
begin
	NewChild := New(PCacheProc, Create(AID));
end;

function TCacheProc.NewChild(AID : String) : PListItem;
begin
	NewChild := New(PCacheProc, Create(AID));
end;


