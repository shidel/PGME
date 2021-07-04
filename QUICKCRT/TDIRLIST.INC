{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TTDirList --- }

{$IFDEF INTERFACE}
const
	class_TDirList = 'TDirList';
	class_TDirItem = 'TDirItem';
	
	
type
	PDirItem = ^TDirItem;
	TDirItem = object (TListItem)
	private
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		Attr : byte;
	public
	end;
	
	PDirList = ^TDirList;
	TDirList = object(TListView)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FValid : boolean;
		FFilter, FDir, FTextParent, FTextSubDir : String;
		FFileAttrs : array[0..5] of TTextAttrs;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure SetDir(AValue : String); virtual;
		function GetDir : String; virtual;
		procedure SetFilter(AValue : String); virtual;
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		function  GetItemAttr(var AItem, ASubItem : PListItem; AColumn, AState : integer) : integer; virtual;
		function DirSizeStr(AValue : LongInt) : string; virtual;
		function DirDateStr(AValue : LongInt) : string; virtual;
		function DirTimeStr(AValue : LongInt) : string; virtual;
		function DirWhenStr(AValue : LongInt) : string; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}

function TDirItem.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDirItem) then 
		ObjectClass := class_TDirItem
	else
		ObjectClass := inherited ObjectClass(AName);
end;
{--- TDirList ---}
function TDirList.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDirList) then 
		ObjectClass := class_TDirList
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDirList.Create(AParent : PControl; AName : String);
var
	I : integer;
begin
	inherited Create(AParent, AName);
	SetSorted(False);
	FValid := False;
	FDir := Dir('');
	FFilter := 'All Files (*.*)';
	FTextParent := '<PARENT>';
	FTextSubDir := '<SUBDIR>';
	for I := 0 to 5 do 
		FFileAttrs[I] := FNAttr;
	FFileAttrs[0].Normal := (FFileAttrs[0].Normal and $F0) or $07;
	FFileAttrs[1].Normal := (FFileAttrs[1].Normal and $F0) or $0D;
	FFileAttrs[2].Normal := (FFileAttrs[2].Normal and $F0) or $0F;
	FFileAttrs[3].Normal := (FFileAttrs[3].Normal and $F0) or $0C;
	FFileAttrs[4].Normal := (FFileAttrs[4].Normal and $F0) or $08;
	FFileAttrs[5].Normal := (FFileAttrs[5].Normal and $F0) or $0B;
	for I := 0 to 4 do 
		with FFileAttrs[I] do
			if GetBlink then
				begin
					Hover := (Normal and $80) shr 4 + (Normal and $07) shl 4;
					Focused := (Normal and $07) + $08 + (Normal and $07) shl 4;
					Click := $09 or ((Normal and $80) shr 4) + (Normal and $07) shl 4;
				end
			else
				begin
					Hover := (Normal and $80) shr 4 + (Normal and $0F) shl 4;
					Focused := (Normal and $07) + $08 + (Normal and $07) shl 4;
					Click := $09 or ((Normal and $80) shr 4) + (Normal and $07) shl 4;
				end;
end;

procedure TDirList.SetDir(AValue : String);
var
	Search : TSearchRec;
	Temp : TDirItem;
	Item : PDirItem;
	S, C, A : String;
	I : integer;
begin
	ClearItems;
	if Assigned(FColumns) then begin
		FColumns^[0].Width := FRBounds.Width - 1;
		for I := 1 to FColumnCount - 1 do
		 	Dec(FColumns^[0].Width, FColumns^[I].Width);
	end;

	Temp.Create('DIR');
	Temp.Sorted := True;
	AValue := Dir(AValue);
	FDir := AValue;
	FindFirst(AValue + '*.*', faAnyFile, Search);
	FValid := DosError = 0;
	while DosError = erNone do begin
		if (Search.Attr and faDirectory = faDirectory) then begin
			if (Search.Name =  '.') then begin
				{ ignored }
			end else if (Search.Name =  '..')  then begin
				if Length(AValue) > 3 then begin
					Item := New(PDirItem, Create(Search.Name));
					if (FColumnCount > 0) then 
						Item^.Add(New(PDirItem, Create(FTextParent)));
					Item^.Attr := Search.Attr;
					Temp.Add(Item);
				end
			end else begin
				Item := New(PDirItem, Create(Search.Name));
				if (FColumnCount > 0) then 
					Item^.Add(New(PDirItem, Create(FTextSubDir)));
				Item^.Attr := Search.Attr;
				Temp.Add(Item);
			end;
		end;
		FindNext(Search);
	end;
	while Assigned(Temp.First) do 
		FItems.Add(Temp.First);
	
	S := Copy(FFilter, Pos('(', FFilter) + 1, Length(FFilter));
	S := Copy(S, 1, Pos(')', S) - 1);
	while S <> '' do begin
		C := PullStr(';', S);
		FindFirst(AValue + C, faAnyFile, Search);
		FValid := DosError = 0;
		while DosError = erNone do begin
			if (Search.Attr and faDirectory <> faDirectory) then begin
				if Search.Attr and faHidden <> faHidden then 
					Item := New(PDirItem, Create(Search.Name))
				else
					Item := New(PDirItem, Create(lcase(Search.Name)));			
				Item^.Add(New(PDirItem, Create(DirSizeStr(Search.Size))));
				A := 'rash';
				Item^.Attr := Search.Attr;
				if Search.Attr and faReadOnly <> faReadOnly then A[1] := '.';
				if Search.Attr and faArchive <> faArchive then A[2] := '.';
				if Search.Attr and faSystem <> faSystem then A[3] := '.';
				if Search.Attr and faHidden <> faHidden then A[4] := '.';
				Item^.Add(New(PDirItem, Create(A)));
				Item^.Add(New(PDirItem, Create(DirDateStr(Search.Time))));
				Item^.Add(New(PDirItem, Create(DirTimeStr(Search.Time))));
				Item^.Add(New(PDirItem, Create(DirWhenStr(Search.Time))));
				{GetMem(Item^.Data, Sizeof(TSearchRec));
				TSearchRec(Item^.Data^) := Search; }
				Temp.Add(Item);
			end;
			FindNext(Search);
		end;
		while Assigned(Temp.First) do 
			FItems.Add(Temp.First);	
	end;
	
	Temp.Destroy;
	AdjustSize;
	Update;
end;

function TDirList.GetDir : String;
begin
	GetDir := FDir;
end;

procedure TDirList.SetFilter(AValue : String);
begin
	if AValue = FFilter then exit;
	FFilter := AValue;
	SetDir(FDir);
end;

function TDirList.GetItemAttr(var AItem, ASubItem : PListItem; AColumn, AState : integer) : integer; 
var
	I : integer;
begin
	if (FColumnCount = 0) or (not Assigned(AItem)) then 
		case AState of
			1 : GetItemAttr := FNAttr.Hover;
			2 : GetItemAttr := FNAttr.Click;
			3 : GetItemAttr := FNAttr.Focused;
			-1 : GetItemAttr := FNAttr.Disabled;
		else
			GetItemAttr := Normal; 
		end
	else
		I := 0;
		if PDirItem(AItem)^.Attr and faArchive = faArchive then I := 2;
		if PDirItem(AItem)^.Attr and faReadOnly = faReadOnly then I := 1;
		if PDirItem(AItem)^.Attr and faSystem = faSystem then I := 3;
		if PDirItem(AItem)^.Attr and faHidden = faHidden then I := 4;
		if PDirItem(AItem)^.Attr and faDirectory = faDirectory then I := 5;
		
		case AState of
			1 : GetItemAttr := FFileAttrs[I].Hover;
			2 : GetItemAttr := FFileAttrs[I].Click;
			3 : GetItemAttr := FFileAttrs[I].Focused;
			-1 : GetItemAttr := FFileAttrs[I].Disabled;
		else
			GetItemAttr := FFileAttrs[I].Normal; 
		end;
end;

procedure TDirList.ApplyTheme( AConfig : PConfigFile );
var
	I : integer;	
begin
	inherited ApplyTheme ( AConfig );
	if Assigned(FColumns) then begin 
		AConfig^.GetInts('COLORS.NORMAL', FFileAttrs[0], SizeOf(TTextAttrs) div SizeOf(Integer));
		AConfig^.GetInts('COLORS.READONLY', FFileAttrs[1], SizeOf(TTextAttrs) div SizeOf(Integer));
		AConfig^.GetInts('COLORS.ARCHIVE', FFileAttrs[2], SizeOf(TTextAttrs) div SizeOf(Integer));
		AConfig^.GetInts('COLORS.SYSTEM', FFileAttrs[3], SizeOf(TTextAttrs) div SizeOf(Integer));
		AConfig^.GetInts('COLORS.HIDDEN', FFileAttrs[4], SizeOf(TTextAttrs) div SizeOf(Integer));
		AConfig^.GetInts('COLORS.DIRECTORY', FFileAttrs[5], SizeOf(TTextAttrs) div SizeOf(Integer));
	end;
end;

procedure TDirList.ApplyLanguage( AConfig : PConfigFile );
var
	I : integer;	
begin
	inherited ApplyLanguage ( AConfig );
	if Assigned(FColumns) then begin 
		FTextParent := AConfig^.GetValue(GetPathID + '.PARENT', FTextParent );
		FTextSubDir := AConfig^.GetValue(GetPathID + '.SUBDIR', FTextSubDir );
	end;
end;

function TDirList.DirSizeStr(AValue : LongInt) : String;
begin
	DirSizeStr := Comma(IntStr(AValue));
end;

function TDirList.DirDateStr(AValue : LongInt) : string;
var
	DT : TDateTime;
	FD : array[0..2] of TFormatData;
begin
	Unpacktime(AValue, DT);
	FD[0].ID := FmtInteger;
	FD[0].IntegerValue := DT.Year;
	FD[1].ID := FmtInteger;
	FD[1].IntegerValue := DT.Month;
	FD[2].ID := FmtInteger;
	FD[2].IntegerValue := DT.Day;
	DirDateStr := FormatStr(Application^.FFileDateFmt, FD, 3);
end;

function TDirList.DirTimeStr(AValue : LongInt) : string;
var
	DT : TDateTime;
	FD : array[0..4] of TFormatData;
begin
	Unpacktime(AValue, DT);
	FD[0].ID := FmtInteger;
	FD[0].IntegerValue := DT.Hour;
	FD[2].ID := FmtInteger;
	FD[2].IntegerValue := DT.Min;
	FD[3].ID := FmtInteger;
	FD[3].IntegerValue := DT.Sec;
	FD[4].ID := FmtBoolean;
	FD[4].BooleanValue := DT.Hour >= 12;
	if DT.Hour >= 12 then DT.Hour := DT.Hour - 12;
	if DT.Hour = 0 then DT.Hour := 12;
	FD[1].ID := FmtInteger;
	FD[1].IntegerValue := DT.Hour;
	DirTimeStr := FormatStr(Application^.FFileTimeFmt, FD, 5); 
end;

function TDirList.DirWhenStr(AValue : LongInt) : string;
var
	DT : TDateTime;
	Y, M, D, W : word;
begin
	Unpacktime(AValue, DT);
	GetDate(Y, M, D, W);
	if (DT.Year = Y) and (DT.Month = M) and (DT.Day = D) then
		DirWhenStr := DirTimeStr(AValue)
	else
		DirWhenStr := DirDateStr(AValue);
end;
{$ENDIF}
