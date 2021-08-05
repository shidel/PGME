{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ --- TConfigFile --- }

{$IFDEF INTERFACE}
const
	class_TConfigFile = 'TConfigFile';

type
	PConfigFile = ^TConfigFile;
	TConfigFile = object(TPersistent)
	private
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FFileName : String;
		FList : TListItem;
		FModified : boolean;
		FSectionKey : PListItem;
		FIsValid : boolean;
		FAutoAdd : boolean;
		FAutoMerge : boolean;
		FAutoClear : boolean;
		function ReadConfig : boolean; virtual;
	public
		constructor Create(AName : String);
		destructor Destroy; virtual;
		function GetFileName : String; virtual;
		procedure SetFileName ( AValue : String ) ; virtual;
		function GetModified : boolean; virtual;
		procedure SetModified (AValue : boolean); virtual;
		function Read : boolean; virtual;
		function Write : boolean; virtual;
		procedure OpenSection ( AValue : String );
		function FindKey ( AItem : String ) : PListItem; virtual;
		function GetValue(AItem, ADefault : String) : String; virtual;
		procedure SetValue(AItem, AValue : String); virtual;
		procedure GetInts(AItem : String; var AInts; ACount : Integer); virtual;
		procedure SetInts(AItem : String; var AInts; ACount : integer); virtual;
		function GetHexStr(AItem, ADefault : String) : String; virtual;
		procedure SetHexStr(AItem, AValue : String); virtual;
		function GetSetValue(AItem : String; ADefault : integer; ASet : Array of String) : integer; virtual;
		procedure SetSetValue(AItem : String; AIndex : integer; ASet : Array of String); virtual;
		function IsValid : boolean; virtual;
		function GetAutoAdd : boolean; virtual;
		procedure SetAutoAdd (AValue : boolean); virtual;
		procedure Clear; virtual;
		procedure DeleteSection; virtual;
		procedure DeleteKey(AItem : String); virtual;
	end;

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TConfigFile.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TConfigFile) then
		ObjectClass := class_TConfigFile
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TConfigFile.Create(AName : String);
begin
	inherited Create(AName);
	FFileName := GetExePath + GetExeBase + ConfigExt;
	FModified := false;
	FList.Create('ROOT');
	FList.SetFreeOnDestroy(False);
	FList.Sorted := True;
	FSectionKey := nil;
	FIsValid := false;
	FAutoAdd := false;
	FAutoMerge := True;
	FAutoClear := True;
end;

destructor TConfigFile.Destroy;
begin
	FList.Destroy;
	inherited Destroy;
end;

function TConfigFile.GetFileName : String;
begin
	GetFileName := FFileName;
end;

procedure TConfigFile.SetFileName ( AValue : String ) ;
begin
	if FFileName = AValue then exit;
	FFileName := AValue;
end;

function TConfigFile.ReadConfig : boolean;
var
	F : TFile;
	S, K, V : String;
	Key, Item, Value : PListItem;
	Loc : integer;
	Plus : boolean;
begin
	ReadConfig := False;
	{$IFDEF DEVLOG_CONFIG}
	WriteLog('++ Begin Read Config file ' + GetFileName );
	LogInc;
	{$ENDIF}
	F.Create;
	F.Assign(GetFileName);
	F.Reset;
	if F.IOResult <> 0 then begin
		F.Destroy;
		exit;
	end;
	Key := nil;
	Item := nil;
	Value :=nil;
	K := '';
	V := '';
	while not F.EOF do
		begin
			F.ReadLn(S, SizeOf(S) - 1 );
			S := LTrim(S);
			while ( Length(S) > 0 ) and ( S[1] = #9 ) do
				Delete(S, 1, 1);
			if S <> '' then begin
				if (Pos('[', S) = 1) and (Pos(']', S) > 2) then begin
					S := UCase(Copy(S, 2, Pos(']', S) - 2));
					Key := FList.FindID(S);
					if not assigned(Key) then begin
						Key := New(PListItem, Create(S));
						Key^.Sorted := True;
						FList.Add(Key);
						Item := nil;
						Value := nil;
					end;
				end else if (Pos('=', S) > 1) and Assigned(Key) then begin
					Loc := Pos('=', S);
					Plus := Loc - 1 = Pos('+', S);
					V := Copy(S, Loc + 1, Length(S) - Loc );
					if Plus then
						K := Trim(UCase(Copy(S, 1, Loc - 2)))
					else
						K := Trim(UCase(Copy(S, 1, Loc - 1)));
					if (K <> '') then begin
						Item := Key^.FindID(K);
						if not Assigned(Item) then begin
							Item := New(PListItem, Create(K));
							Key^.Add(Item);
						end;
					end;
					if  Assigned(Item) then begin
						if Not Plus then Item^.Clear;
						Value := New(PListItem, Create(V));
						Item^.Add(Value);
					end; { Or its Broken }
				end; { Or It's Broken }
			end;
		end;
	F.Destroy;
	{$IFDEF DEVLOG_CONFIG}
	LogDec;
	WriteLog('++ Done read Config file ' + GetFileName );
	{$ENDIF}
	ReadConfig := True;
	FIsValid := True;
end;

function TConfigFile.Read : boolean;
var
	R : boolean;
	Merge : String;
begin
	FSectionKey := nil;
	FIsValid := False;
	FModified := False;
	if FAutoClear then FList.Clear;
	repeat
		R := ReadConfig;
		Merge := '';
		if FAutoMerge then begin
			OpenSection('*');
			Merge := GetValue('MERGE', '');
			SetValue('MERGE', ''); { prevent reloading same merged file }
			if Merge <> '' then begin
				SetFileName(Merge);
			end;
		end;
	until (Not R) or (Not FAutoMerge) or (Merge = '');
	Read := R;
end;

function TConfigFile.Write : boolean;
var
	F : TFile;
	S, K, NS : String;
	Key, Item, Value : PListItem;
begin
	FIsValid := False;
	Write := False;
	F.Create;
	{$IFDEF DEVLOG_CONFIG}
	WriteLog('++ Begin write Config file ' + GetFileName );
	LogInc;
	{$ENDIF}
	F.Assign(GetFileName);
	F.Rewrite;
	if F.IOResult <> 0 then begin
		{$IFDEF DEVLOG_CONFIG}
		LogDec;
		WriteLog('++ END with IO Error ' + IntStr(F.IOResult)  + GetFileName );
		{$ENDIF}
		F.Destroy;
		exit;
	end;
	NS := '';
	Key := FList.First;
	while Assigned(Key) do begin
		K := '[' + Key^.GetNameID + ']';
		Item := Key^.First;
		while Assigned(Item) do begin
			if K <> '' then begin
				F.WriteLn(K);
				K := '';
			end;
			S := #9 + Item^.GetNameID + '=';
			Value := Item^.First;
			while Assigned(Value) do begin
				S := S + Value^.GetNameID;
				F.WriteLn(S);
				S := #9#9'+=';
				if (Item^.First <> Item^.Last) and
				((Value = Item^.First) or (Value = Item^.Last)) then begin
					F.WriteLn(NS);
				end;
				Value := Value^.Next;
			end;
			Item := Item^.Next;
		end;
		Key := Key^.Next;
		if (Assigned(Key)) and (K = '') then begin
			S := '';
			F.WriteLn(S);
		end;
	end;
	F.Destroy;
	FModified := False;
	{$IFDEF DEVLOG_CONFIG}
	LogDec;
	WriteLog('++ Done write Config file ' + GetFileName );
	{$ENDIF}
	Write := True;
	FIsValid :=True;
end;

function TConfigFile.GetModified : boolean;
begin
	GetModified := FModified;
end;

function TConfigFile.IsValid : boolean;
begin
	IsValid := FIsValid;
end;

procedure TConfigFile.SetModified (AValue : boolean);
begin
	if AValue = FModified then exit;
	FModified := AValue;
end;

procedure TConfigFile.OpenSection ( AValue : String );
begin
	AValue := Trim(uCase(AValue));
	if Assigned(FSectionKey) and (FSectionKey^.GetNameID = AValue) then exit;
	FSectionKey := FList.FindID(AValue);
	if not assigned(FSectionKey) then begin
		FSectionKey := New(PListItem, Create(AValue));
		FSectionKey^.Sorted := True;
		FList.Add(FSectionKey);
		FSectionKey := FList.FindID(AValue);
	end;
end;

function TConfigFile.FindKey ( AItem : String ) : PListItem;
begin
	if Assigned(FSectionKey) then
		FindKey := FSectionKey^.FindID(UCase(Trim(AItem)))
	else
		FindKey := nil;
end;

function TConfigFile.GetValue(AItem, ADefault : String) : String;
{$IFDEF TEMPLATES} {$IFDEF ONDISK}
	const
		FLastSection : pointer = nil;
{$ENDIF} {$ENDIF}
var
	P : PListItem;
	{$IFDEF TEMPLATES} {$IFDEF ONDISK}
	F : TFIle;
	S : String;
	{$ENDIF} {$ENDIF}
begin
	P := FindKey(AItem);
	if Assigned(P) and Assigned(P^.First) then
		GetValue := P^.First^.GetNameID
	else
		GetValue := ADefault;
	{$IFDEF TEMPLATES}
		{$IFDEF ONDISK}
			if (not Assigned(P)) and FAutoAdd then
				SetValue(AItem, ADefault)
			else if Assigned(FSectionKey) then begin
				F.Create;
				F.Assign(Dir(PathOf(FFileName)) + 'ONDISK.' + FileExtension(FFileName));
				F.Reset;
				if F.IOResult <> 0 then begin
					F.Close;
					F.Rewrite;
				end;
				F.SeekEOF;
				if FLastSection <> FSectionKey then begin
					S := '[' + FSectionKey^.GetNameID + ']' + #$0d#$0a;
					F.Write(S);
				end;
				FLastSection := FSectionKey;
				S := #9 + AItem + '=' + ADefault + #$0d#$0a;
				F.Write(S);
				F.Close;
				F.Destroy;
			end;
		{$ELSE}
			if (not Assigned(P)) then
				SetValue(AItem, ADefault);
		{$ENDIF}
	{$ELSE}
		if (not Assigned(P)) and FAutoAdd then
			SetValue(AItem, ADefault);
	{$ENDIF}
end;

procedure TConfigFile.SetValue(AItem, AValue : String);
var
	P : PListItem;
begin
	if not assigned(FSectionKey) then exit;
	P := FindKey(AItem);
	if not Assigned(P) then begin
		P := New(PListItem, Create(UCase(Trim(AItem))));
		FSectionKey^.Add(P);
	{$IFDEF TEMPLATES}
	{end else begin
		if (P^.Count = 1) and (P^.First^.GetNameID = AValue) then
			Exit;}
	{$ENDIF}
	end;
	P^.Clear;
	P^.Add(New(PListItem, Create(AValue)));
	FModified := True;
end;

procedure TConfigFile.GetInts(AItem : String; var AInts; ACount : Integer);
type
	Ints = array[0..$FF] of integer;
var
	S, T : String;
	I, L : integer;
	P : PListItem;
begin
	if Not Assigned(FSectionKey) then exit;
	P := FSectionKey^.FindID(UCase(Trim(AItem)));
	if (not assigned(P)) or (not assigned(P^.First)) then begin
		{$IFDEF TEMPLATES}
		SetInts(AItem, AInts, ACount);
		{$ENDIF}
		exit;
	end;
	S := P^.First^.GetNameID;
	for I := 0 to ACount - 1 do begin
		L := Pos(',', S) - 1;
		if L < 1 then L := Length(S);
		T := Trim(Copy(S, 1, L));
		Delete(S, 1, L + 1);
		Ints(AInts)[I] := StrInt(T);
	end;
end;

procedure TConfigFile.SetInts(AItem : String; var AInts; ACount : integer);
type
	Ints = array[0..$FF] of integer;
var
	S : String;
	I : integer;
begin
	S := '';
	for I := 0 to ACount - 1 do begin
		if S <> '' then S := S + ',';
		S := S + '$' + HexStr(Ints(AInts)[I]);
	end;
	SetValue(AItem, S);
end;

procedure TConfigFile.SetHexStr(AItem, AValue : String);
begin
	SetValue(AItem, StrHex(AValue, false));
end;

function TConfigFile.GetHexStr(AItem, ADefault : String) : String;
var
	I : integer;
	S, T, O : String;
begin
	O := '';
	S := GetValue(AItem, ADefault);
	while (S <> '') do begin
		I := Pos('$', S);
		if I > 0 then begin
			T := Copy(S, 1, I - 1);
			Delete(S, 1, I);
		end else begin
			T := S;
			S := '';
		end;
		if T <> '' then begin
			O := O + Char(StrInt('$' + T));
		end;
	end;
	GetHexStr := O;
end;

function  TConfigFile.GetSetValue(AItem : String; ADefault : integer; ASet : Array of String) : integer;
var
    I : integer;
begin
    GetSetValue := ADefault;
    if ADefault < 0 then
        AItem := UCase(GetValue(AItem, ''))
    else
        AItem := UCase(GetValue(AItem, ASet[ADefault]));
    for I := low(ASet) to High(ASet) do
        if AItem = ASet[I] then begin
            GetSetValue := I;
            Break;
        end
end;

procedure  TConfigFile.SetSetValue(AItem : String; AIndex : integer; ASet : Array of String);
begin
    SetValue(AItem, ASet[AIndex]);
end;

function TConfigFile.GetAutoAdd : boolean;
begin
	GetAutoAdd := FAutoAdd;
end;

procedure TConfigFile.SetAutoAdd (AValue : boolean);
begin
	FAutoAdd := AValue;
end;

procedure TConfigFile.Clear;
begin
	FList.Clear;
end;

procedure TConfigFile.DeleteSection;
begin
	if Not Assigned(FSectionKey) then exit;
	FList.Remove(FSectionKey);
	Dispose(FSectionKey, Destroy);
	FSectionKey := nil;
	FModified := True;
end;

procedure TConfigFile.DeleteKey;
var
	P : PListItem;
begin
	P := FindKey(AItem);
	if not Assigned(P) then exit;
	PListItem(P^.Parent)^.Remove(P);
	Dispose(P, Destroy);
	FModified := true;
end;
{$ENDIF}
