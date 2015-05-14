{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TMenuCache --- }

{$IFDEF INTERFACE}
const
	class_TMenuCache = 'TMenuCache';
	
type
	PMenuCache = ^TMenuCache;
	TMenuCache = object(TPersistent)
    public { protected }
    		FNoMenuStr,
    		FNoNameStr,
    		FNoTitleStr,
    		FNoDescStr : String;
    		FModified : boolean;
    		FCache : PListItem;
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
		procedure Load; virtual;
		procedure Save; virtual;
		procedure Purge; virtual;

		procedure ApplyLanguage(AConfig : PConfigFile; APrefix : String); virtual;
		procedure SetPaths(APaths : String); virtual;		
		procedure Update; virtual;

		function Modified : boolean; virtual;
		{ Results based on current active menu }
		function FileName : String; virtual; { Menu Filename }
		function Title : String; virtual;	 { Menu Short Title }
		function Description : String; virtual;	{ Menu Long Title }
		function Writable : boolean; virtual; { Is Menu writable } 
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}

{$I TCacheItems.inc}

function TMenuCache.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TMenuCache) then 
		ObjectClass := class_TMenuCache
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TMenuCache.Create;
begin
	inherited Create('ETERNITY.CACHE');
	FNoMenuStr := '';
	FNoNameStr := '';
	FNoTitleStr := '';
	FNoDescStr := '';
	FCache := New(PCacheItem, Create('MENUDATA'));
	FCache^.Sorted := True;
end;

destructor TMenuCache.Destroy;
begin
	Dispose(FCache, Destroy);
	inherited Destroy;
end;

procedure TMenuCache.ApplyLanguage;
begin
	AConfig^.OpenSection('STRINGS');
	APrefix := APrefix + '.';
	FNoMenuStr := AConfig^.GetValue(APrefix + 'MENU.TEXT.EMPTY', '');
	FNoNameStr := AConfig^.GetValue(APrefix + 'MENU.TEXT.UNNAMED', 'no name');
	FNoTitleStr := AConfig^.GetValue(APrefix + 'MENU.TEXT.UNTITLED', 'Untitled Menu');
	FNoDescStr := AConfig^.GetValue(APrefix + 'MENU.TEXT.UNDESCRIBED', 'Untitled Menu');
end;

procedure TMenuCache.SetPaths;
var
	ADir, Hold : String;
	P, N : PListItem;
begin
	{$IFNDEF TEMPLATES}
		if APaths = '' then 
			APaths := GetExePath;
	{$ENDIF}
	Hold := ';';
	while APaths <> '' do begin
		ADir := Dir(PullStr(';', APaths));
		Hold := Hold + ADir + ';';
		if FCache^.FindID(ADir) = nil then
			FCache^.Add(New(PCacheDir, Create(ADir)));
	end;
	P := FCache^.First;
	while Assigned(P) do begin
		N := P^.Next;
		if Pos(';' + P^.GetNameID + ';', Hold) < 1 then begin
			FCache^.Remove(P);
			Dispose(P, Destroy);
			FModified := True;
		end;
		P := N;
	end;
end;

procedure TMenuCache.Load;
var
	F : TFile;
	S : String;
	OK : boolean;
begin
	FCache^.Clear;
	FModified := False;
	F.Create;
	F.Assign(GetExePath + 'ETERNITY.DAT');
	F.Reset; 
	OK := IOResult = 0;
	if OK then begin
		FillChar(S, Sizeof(S), 0);
		S[0] := Chr(Length(GetNameID) + 1);
		F.ReadRecord(S[1], Length(GetNameID));
		OK := (IOResult = 0) and (S = GetNameID + #0);
	end;
	if OK then OK := PCacheItem(FCache)^.Read(F);
	F.Destroy;
	if Not OK then Purge;
end;

procedure TMenuCache.Save;
var
	F : TFile;
	OK : boolean;
	S : String;
begin
	F.Create;
	F.Assign(GetExePath + 'ETERNITY.DAT');
	F.Rewrite;
	OK := IOResult = 0;
	if OK then begin
		S := GetNameID + #0;
		F.WriteRecord(S[1], Length(S));
		OK := (IOResult = 0);
	end;
	PCacheItem(FCache)^.Write(F);
	FModified := False;
	if Not OK then Purge;
end;

procedure TMenuCache.Purge;
begin
	FModified := True;
	FCache^.Clear;
end;

procedure TMenuCache.Update;
var
	P, T, N : PListItem;
	S : String;
	Drive : byte;
	Fixed : boolean;
	SRec : TSearchRec;
begin
	{ Search for Menu Files }
	P := FCache^.First;
	while Assigned(P) do begin
		FindFirst(P^.GetNameID + '*.MNU', faAnyFile, SRec);
		S := P^.GetNameID;
		N := P^.Next;
		Drive := Ord(S[1]) - 64;
		Fixed := not (IsRemote(Drive) or isRemovable(Drive) or isCDROM(Drive));
		while DosError = 0 do begin
			if SRec.Attr and faDirectory <> faDirectory then begin
				T := P^.FindID(SRec.Name);
				if T = nil then begin
					FModified := True;
					T := New(PCacheMenu, Create(SRec.Name));
					PCacheMenu(T)^.Sorted := True;		
					PCacheMenu(T)^.LastMod := -1;
					PCacheMenu(T)^.Fixed := Fixed;
					P^.Add(T);
				end else begin
					if (PCacheMenu(T)^.LastMod <> SRec.Time) or 
					(PCacheMenu(T)^.Fixed <> Fixed) then begin
						FModified := True;
						T^.Clear;
						PCacheMenu(T)^.LastMod := -1;
						PCacheMenu(T)^.Fixed := Fixed;
					end;
				end;		
			end;			
			FindNext(SRec);
		end;
		P := N;
	end;
end;

function TMenuCache.Modified;
begin
	Modified := FModified;
end;

function TMenuCache.FileName;
begin
	FileName := '';
end;

function TMenuCache.Title;
begin
	Title := FNoTitleStr;
end;

function TMenuCache.Description;
begin
	Description := FNoDescStr;	
end;

function TMenuCache.Writable;
begin
	Writable := True;
end;
{$ENDIF}
