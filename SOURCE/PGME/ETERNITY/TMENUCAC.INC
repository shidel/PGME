{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ --- TMenuCache --- }

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
    		FModified, FRebuild : boolean;
    		FCache : PListItem;
    		FWatch : String;
		function  ObjectClass ( AName : String ) : String; virtual;
		procedure UpdateMenu(AMenu : PListItem); virtual;
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
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}

const
    cHideModes : array[0..2] of string = ('OFF', 'ON', 'AUTO');

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
	FRebuild := False;
	FWatch := GetEnv('DOSDIR');
	if FWatch <> '' then FWatch := TailDelim(FWatch) + 'APPINFO';
	if DirExists(FWatch) then
	    FWatch := TailDelim(FWatch) + '*.LSM'
	else
	    FWatch := '';
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
	{ Add new paths }
	Hold := ';';
	while APaths <> '' do begin
		ADir := Dir(PullStr(';', APaths));
		Hold := Hold + ADir + ';';
		if FCache^.FindID(ADir) = nil then
			FCache^.Add(New(PCacheDir, Create(ADir)));
	end;

	{ remove dead paths }
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
const
	FileID = 'ETERNITY.CACHE'#0;
var
	F : TFile;
	S : String;
	OK : boolean;
	WD, WH, WC : String;
begin
	FCache^.Clear;
	FModified := False;
	F.Create;
	F.Assign(GetExePath + 'ETERNITY.DAT');
	F.Reset;
	OK := IOResult = 0;
	if OK then begin
		FillChar(S, Sizeof(S), 0);
		S[0] := Chr(Length(FileID));
		F.ReadRecord(S[1], Length(FileID));
		OK := (IOResult = 0) and (S = FileID);
	end;
	if OK then begin
	    WD := F.GetString;
	    OK := (F.IOResult = 0) and (WD = FWatch);
	end;
	if OK then begin
	    WH := F.GetString;
	    OK := (F.IOResult = 0);
	    if OK and (WD <> '') then begin
	        WC := DirScanID(WD, faAnyFile, 16);
	        OK := WC = WH;
	    end;
	end;
	if OK then OK := PCacheItem(FCache)^.Read(F);
	F.Destroy;
	if Not OK then begin
	    Purge;
	    FRebuild := True;
	end;
end;

procedure TMenuCache.Save;
const
	FileID = 'ETERNITY.CACHE'#0;
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
		S := FileID;
		F.WriteRecord(S[1], Length(S));
		OK := (IOResult = 0);
	end;
	if OK then begin
	    F.PutString(FWatch);
		OK := (IOResult = 0);
	end;
	if OK then begin
	    if FWatch = '' then
    	    F.PutString(FWatch)
    	else begin
        	S := DirScanID(FWatch, faAnyFile, 16);
    	    F.PutString(S);
    	end;
		OK := (IOResult = 0);
	end;
	if OK then OK := PCacheItem(FCache)^.Write(F);
	FModified := not OK;
	if Not OK then Purge;
end;

procedure TMenuCache.Purge;
begin
	FModified := True;
	FCache^.Clear;
end;

procedure TMenuCache.Update;
var
	P, N, T, M : PListItem;
	S : String;
	Drive : byte;
	Fixed : boolean;
	SRec : TSearchRec;
begin
	{ Search for Menu Files }
	P := FCache^.First;
	while Assigned(P) do begin

		{ Scan each directory for files, add if needed }
		FindFirst(P^.GetNameID + '*' + EMenuExt, faAnyFile, SRec);
		S := P^.GetNameID;
		Drive := Ord(S[1]) - 64;
		Fixed := not (IsRemote(Drive) or isRemovable(Drive) or isCDROM(Drive));
		while DosError = 0 do begin
			if SRec.Attr and faDirectory <> faDirectory then begin
				T := P^.FindID(SRec.Name);
				if T = nil then begin
					{ New Menu File Found }
					FModified := True;
					if not MemCheck(Sizeof(TCacheMenu)) then Halt(8);
					T := New(PCacheMenu, Create(SRec.Name));
					PCacheMenu(T)^.Fixed := Fixed;
					PCacheMenu(T)^.Available := True;
					P^.Add(T);
				end else begin
					if (PCacheMenu(T)^.LastMod <> SRec.Time) or
					(PCacheMenu(T)^.Fixed <> Fixed) then begin
						{ Menu File has been modified }
						FModified := True;
						PCacheMenu(T)^.LastMod := -1;
						PCacheMenu(T)^.Fixed := Fixed;
						PCacheMenu(T)^.Available := True;
					end;
				end;
			end;
			FindNext(SRec);
		end;

		{ remove missing files, if are on a fixed disk }
		M := P^.First;
		while Assigned(M) do begin
			N := M^.Next;
			if PCacheMenu(M)^.Available <> FileExists(P^.GetNameID + M^.GetNameID) then begin
				FModified := True;
				PCacheMenu(M)^.Available := FileExists(P^.GetNameID + M^.GetNameID);
			end;
			if (Not PCacheMenu(M)^.Available ) then begin
				if PCacheMenu(M)^.Fixed then begin
					{ Fixed disk menu file doesn't exits, remove from cache }
					FModified := True;
					P^.Remove(M);
					Dispose(M, Destroy);
				end;
			end else if PCacheMenu(M)^.LastMod = -1 then
				UpdateMenu(M);
			M := N;
		end;
		P := P^.Next;
	end;
end;

procedure TMenuCache.UpdateMenu(AMenu : PListItem);
var
	Temp : TConfigFile;
	SRec : TSearchRec;
	S, H : String;
	M, A, P, V : PListItem;
	HideItem, HideMenu : boolean;
	I, ICnt : LongInt;
begin
	AMenu^.Clear;
	Temp.Create(AMenu^.GetNameID);
	Temp.SetFileName(AMenu^.Parent^.GetNameID + AMenu^.GetNameID);
	Temp.Read;
	Temp.OpenSection('*');
	HideMenu := False;
	if Temp.GetValue('TYPE', '') = ftMenu then begin
		{ Validated that is a menu file }

		{ Set Short Title }
		S := Trim(Temp.GetValue('TITLE.' + Application^.FLanguageID, ''));
		if S = '' then
			S := Trim(Temp.GetValue('TITLE', ''));
		if S = '' then
			S := Trim(Temp.GetValue('TITLE.EN_US', ''));
		if S = '' then
			S := Copy(SRec.Name, 1, Length(AMenu^.GetNameID) - 4);
		{PCacheMenu(AMenu)^.ID := Temp.GetValue('ID', AMenu^.GetNameID)); }
		PCacheMenu(AMenu)^.Writable := StrBool(Temp.GetValue('WRITABLE', 'YES'));
		PCacheMenu(AMenu)^.SetShortTitle(S);
		S := UCase(Trim(Temp.GetValue('WAIT', 'NO')));
		if (IntStr(StrInt(S)) = S) then
		    PCacheMenu(AMenu)^.DefaultWait := StrInt(S)
		else if (S = 'PAUSE') or (S = 'FOREVER') or (S = 'YES') then
		    PCacheMenu(AMenu)^.DefaultWait := -1
		else
		    PCacheMenu(AMenu)^.DefaultWait := 0;
		S := UCase(Trim(Temp.GetValue('HIDE', 'NO')));
		if (S = 'AUTO') then
		    PCacheMenu(AMenu)^.AutoHide := 2
		else if (S = 'ON') then begin
		    PCacheMenu(AMenu)^.AutoHide := 1;
		    HideMenu := True;
		end else
		    PCacheMenu(AMenu)^.AutoHide := 0;
		{ Set Long Title }
		S := Trim(Temp.GetValue('DESCRIPTION.' + Application^.FLanguageID, ''));
		if S = '' then
			S := Trim(Temp.GetValue('DESCRIPTION', ''));
		if S = '' then
			S := Trim(Temp.GetValue('DESCRIPTION.EN_US', ''));
		if S = '' then
			S := PCacheMenu(AMenu)^.ShortTitle;
		PCacheMenu(AMenu)^.SetLongTitle(S);

		{ Read Program Entries }
		ICnt := 0;
		if not HideMenu then begin
            M := Temp.FList.First;
            while assigned(M) do begin
                Temp.OpenSection(M^.GetNameID);
                S := Temp.GetValue('FILE', '');
                H := Ucase(trim(Temp.GetValue('HIDE', '')));
                if (S <> '') and (H = 'AUTO') then begin
                    if PathOf(S) = '' then
                        H := TailDelim(GetEXEPath) + S
                    else
                        H := FileExpand(S);
                    HideItem := (H = '') or (not FileExists(H));
                end else if StrBool(H) then
                    HideItem := True
                else
                    HideItem := False;

                if (Not HideItem) and ((S <> '')
                or (Temp.FindKey('COMMANDS') <> nil)) then begin
                    { It is a program entry }
                    Inc(ICnt);
                    if not MemCheck(Sizeof(TCacheApp)) then Halt(8);
                    A := New(PCacheApp, Create(M^.GetNameID));
                    with PCacheApp(A)^ do begin
                         { if S = '' then
                            S := 'unknown'; }
                        SetFileName(Trim(uCase(S)));
                        S := Trim(Temp.GetValue('TITLE.'
                        + Application^.FLanguageID, ''));
                        if S = '' then
                            S := Trim(Temp.GetValue('TITLE', ''));
                        if S = '' then
                            S := Trim(Temp.GetValue('TITLE.EN_US', ''));
                        if S = '' then
                            S := GetNameID;
                        SetTitle(S);
                        SetClassName(Trim(Temp.GetValue('CLASS', '')));
                        SetGroupName(Trim(Temp.GetValue('GROUP', '')));
                        HideMode := Temp.GetSetValue('GROUP', 0, cHideModes);
                        SetTone(Trim(Temp.GetValue('TONE', '')));
                        MaxMemory := StrBool(Temp.GetValue('MAXAVAIL', 'NO'));
                        SimpleExec := StrBool(Temp.GetValue('SIMPLE', 'YES'));
                        S := UCase(Trim(Temp.GetValue('WAIT', 'DEFAULT')));
                        ReturnAfter := StrBool(Temp.GetValue('RETURN', 'YES'));
                        CPUCacheOff := StrBool(Temp.GetValue('CPUCACHE', ''));
                        PGMVars := StrBool(Temp.GetValue('PGMEVARS', 'NO'));
                        if (IntStr(StrInt(S)) = S) then
                            ReturnWait := StrInt(S)
                        else if (S = 'DEFAULT') then
                            ReturnWait := -2
                        else if (S = 'PAUSE') or (S = 'FOREVER')
                        or ( S = 'YES' ) then
                            ReturnWait := -1
                        else
                            ReturnWait := 0;
                    end;
                    { Read App's Process }
                    V := M^.FindID('COMMANDS');
                    if Assigned(V) then
                        V := V^.First;
                    I := 0;
                    while Assigned(V) do begin
                        S := Trim(V^.GetNameID);
                        if S <> '' then begin
                            Inc(I);
                            if not MemCheck(Sizeof(TCacheProc)) then Halt(8);
                            P := new(PCacheProc, Create(S));
                            A^.Add(P);
                        end;
                        V := V^.Next;
                    end;
                    AMenu^.Add(A);
                end;
                M := M^.next;
            end;

	    end;

		HideMenu := (ICnt = 0) and (PCacheMenu(AMenu)^.AutoHide <> 0);

	end else begin
	    HideMenu := True;
	end;

	if HideMenu then begin
		PCacheDir(AMenu^.Parent)^.Remove(AMenu);
		Dispose(AMenu, Destroy);
		AMenu := nil;
	end;

	if Assigned(AMenu) then begin
		FindFirst(AMenu^.Parent^.GetNameID + AMenu^.GetNameID, faAnyFile, SRec);
		if DosError = 0 then PCacheMenu(AMenu)^.LastMod := SRec.Time;
	end;
	Temp.Destroy;
end;

function TMenuCache.Modified;
begin
	Modified := FModified;
end;

{$ENDIF}
