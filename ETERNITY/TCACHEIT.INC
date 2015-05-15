{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

type
	PCacheItem = ^TCacheItem;
	TCacheItem = object (TListItem)
		constructor Create(AName : String);
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
		SimpleExec,
		ReturnAfter : boolean;
		constructor Create(AName : String);
		destructor Destroy; virtual;
		function	NewChild(AID : String) : PListItem; virtual;
		function    ReadSelf(var F : TFile) : boolean; virtual;
		function    WriteSelf(var F : TFile) : boolean; virtual;
		function ClassName : String;
		function Filename : String;
		function GroupName : String;
		function Title: String;
		procedure SetClassName(AValue : String);
		procedure SetFilename(AValue : String);
		procedure SetGroupName(AValue : String);
		procedure SetTitle(AValue : String);
	end;

	PCacheProc = ^TCacheProc;
	TCacheProc = object (TCacheItem)
		constructor Create(AName : String);
		function	NewChild(AID : String) : PListItem; virtual;
	end;

constructor TCacheItem.Create(AName : String);
begin
	inherited Create(AName);
	Sorted := True;
end;

function TCacheItem.NewChild(AID : String) : PListItem;
begin
	if not MemCheck(Sizeof(TCacheDir)) then Halt(8);
	NewChild := New(PCacheDir, Create(AID));
end;

function TCacheItem.ReadSelf(var F : TFile) : boolean;
begin
	SetNameID(F.GetString);
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
	if not MemCheck(Sizeof(TCacheMenu)) then Halt(8);
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
	ShortTitle := PtrStr(FShortTitle);
end;

function TCacheMenu.LongTitle : String;
begin
	LongTitle := PtrStr(FLongTitle);
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
	if not MemCheck(Sizeof(TCacheApp)) then Halt(8);
	NewChild := New(PCacheApp, Create(AID));
end;

constructor TCacheApp.Create(AName : String);
begin
	inherited Create(AName);
	Sorted := False;

	FClassName := nil;
	FFilename := nil;
	FGroupName := nil;
	FTitle:= nil;
	MaxMemory := false;
	SimpleExec := true;
	ReturnAfter := True;
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

function TCacheApp.ClassName : String;
begin
	ClassName := PtrStr(FClassName);
end;

function TCacheApp.Filename : String;
begin
	Filename := PtrStr(FFilename);
end;

function TCacheApp.GroupName : String;
begin
	GroupName := PtrStr(FGroupName);
end;

function TCacheApp.Title: String;
begin
	Title := PtrStr(FTitle);
end;

procedure TCacheApp.SetClassName(AValue : String);
begin
	FreeStr(FClassName);
	FClassName := StrPtr(AValue);
end;

procedure TCacheApp.SetFilename(AValue : String);
begin
	FreeStr(FFilename);
	FFilename := StrPtr(AValue);
end;

procedure TCacheApp.SetGroupName(AValue : String);
begin
	FreeStr(FGroupName);
	FGroupName := StrPtr(AValue);
end;

procedure TCacheApp.SetTitle(AValue : String);
begin
	FreeStr(FTitle);
	FTitle := StrPtr(AValue);
end;

function TCacheApp.ReadSelf(var F : TFile) : boolean; 
var
	OK : boolean;
begin
	OK := inherited ReadSelf(F);
	if OK then begin
		F.ReadBoolean(MaxMemory);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.ReadBoolean(SimpleExec);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.ReadBoolean(ReturnAfter);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		SetClassName (F.GetString);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		SetFileName (F.GetString);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		SetGroupName (F.GetString);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		SetTitle (F.GetString);
		OK := F.IOResult = 0;
	end;
	ReadSelf := OK;
end;

function TCacheApp.WriteSelf(var F : TFile) : boolean; 
var
	OK : boolean;
	S : String;
begin
	OK := inherited WriteSelf(F);
	if OK then begin
		F.WriteBoolean(MaxMemory);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.WriteBoolean(SimpleExec);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		F.WriteBoolean(ReturnAfter);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		S := ClassName;
		F.PutString(S);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		S := FileName;
		F.PutString(S);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		S := GroupName;
		F.PutString(S);
		OK := F.IOResult = 0;
	end;
	if OK then begin
		S := Title;
		F.PutString(S);
		OK := F.IOResult = 0;
	end;
	WriteSelf := OK;
end;

constructor TCacheProc.Create(AName : String);
begin
	inherited Create(AName);
	Sorted := False;
end;

function TCacheProc.NewChild(AID : String) : PListItem;
begin
	if not MemCheck(Sizeof(TCacheProc)) then Halt(8);
	NewChild := New(PCacheProc, Create(AID));
end;


