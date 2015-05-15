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
		Fixed : boolean;
		function    ReadSelf(var F : TFile) : boolean; virtual;
		function    WriteSelf(var F : TFile) : boolean; virtual;
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
			Add(T);
			OK := PCacheItem(T)^.Read(F);
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
	ReadSelf := OK;
end;

function TCacheMenu.WriteSelf(var F : TFile) : boolean; 
var
	OK : boolean;
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
	WriteSelf := OK;
end;

function TCacheMenu.NewChild(AID : String) : PListItem; 
begin
	NewChild := New(PCacheItem, Create(AID));
end;

