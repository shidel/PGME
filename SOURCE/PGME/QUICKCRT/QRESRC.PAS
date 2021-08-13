{ Copyright 2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

unit QRESRC; { QuickCrt Resource File Utilities }
{$I QCRT.DEF}

interface

    uses QClass, QDos, QFiles;

const
  	class_TResource : TObjectClass = 'TFile';

type
    TResourceBlockID = record   { size 16 bytes }
        BlockType  : word;       { = 0 }
        HeadBlock  : LongInt;    { = File Position of first block in chain }
        Identifier : array[0..9] of byte; { = 'XBINRSRCv1' }
    end;
    TResourceBlockData = record
        BlockType : word;        { = 1 }
        BlockSize : LongInt;     { Total size of block }
    	Attr      : byte;
    	Time      : LongInt;
    	Size      : LongInt;
    end;
    TResourceBlockName = record
    	Name      : String;
    	Zero      : byte;        { = 0, Null terminator }
    end;
    TResourceRec = record        { Maximum Sized File Block Header And Search }
        Position : longint;
        Data : TResourceBlockData;
        Name : String;
        Zero : byte;
    end;

    TResourceStatusProc = function (AFileName : String; ACompleted : word) : boolean;

    PResource = ^TResource;
    TResource = object (TObject)
	public { private }
	    FBuffer    : array[0..1023] of byte;
	    FResID     : String;
	    FHandler   : PAbstractFile;
	    FResult    : Integer;
	    FResName   : String;
	    FReadOnly  : boolean;
	    FExistOnly : boolean; { Must already have resource data }
	    FDataStart : longint;
	    FHasFooter : boolean;
	    FModified  : boolean;
	    FJunkPaths : boolean;
      procedure   SetHandler(AHandler : PAbstractFile); virtual;
      function    GetHandler : PAbstractFile; virtual;
      function    InvalidHandler : boolean; virtual;
      function    HandlerError : boolean; virtual;
      function    InvalidBlock(var T : TResourceRec; Size : word) : boolean; virtual;
      function    GetFooter : boolean; virtual;
      function    PutFooter : boolean; virtual;
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
    public
      constructor Create(AResName : String; AHandler : PAbstractFile);
      destructor  Destroy; virtual;
      function    GetResult : integer; virtual;
      function    IOResult : integer; virtual;
      function    Open : boolean; virtual;
      function    Close : boolean; virtual;
      function    FindFirst(AFileSpec : String; var ARec : TResourceRec) : boolean; virtual;
      function    FindNext(var ARec : TResourceRec) : boolean; virtual;
      function    Import(ABasePath, AFileSpec : String; AStatus : TResourceStatusProc ) : boolean; virtual;
      function    ExportResource(ABasePath : String; ARec : TResourceRec;
        AStatus : TResourceStatusProc) : boolean; virtual;
      function    Export(ABasePath, AFileSpec : String; AStatus : TResourceStatusProc ) : boolean; virtual;
      function    ExportAll(ABasePath : String; AStatus : TResourceStatusProc ) : boolean; virtual;
      function    Delete(AFileSpec : String; AStatus : TResourceStatusProc ) : boolean; virtual;
      function    DeleteAll : boolean; virtual;
    end;

implementation

    uses QStrings;


function TResource.ObjectClass(AName : String) : String;
begin
    if (AName = '') or (AName = class_TResource) then
        ObjectClass := class_TResource
    else
        ObjectClass := inherited ObjectClass(AName);
end;

constructor TResource.Create;
begin
    inherited Create('');
    FResID := 'XBINRSRCv1';
    FResName := AResName;
    FReadOnly := True;
    FExistOnly := True;
    FDataStart := -1;
    FHasFooter := False;
    FModified := False;
    FJunkPaths := False;
    SetHandler(AHandler);
end;

destructor  TResource.Destroy;
begin
    if FModified then Close;
    if Assigned(FHandler) then begin
        Dispose(FHandler, Destroy);
        FHandler := nil;
    end;
    inherited Destroy;
end;

procedure TResource.SetHandler(AHandler : PAbstractFile);
begin
    FHandler := AHandler;
    FResult := 0;
end;

function TResource.GetHandler : PAbstractFile;
begin
    GetHandler := FHandler;
end;

function TResource.InvalidHandler : boolean;
begin
    if (not Assigned(FHandler))
    or (FHandler^.GetNameID = class_TAbstractFile) then begin
        FResult := 6; { Invalid File Handle (close enough) }
        InvalidHandler := True;
        Exit;
    end;
    InvalidHandler := False;
end;

function TResource.HandlerError : boolean;
begin
    FResult := FHandler^.IOResult;
    HandlerError := FResult <> 0;
end;

function TResource.InvalidBlock(var T : TResourceRec; Size : word) : boolean;
begin
    InvalidBlock := True;
    if T.Position < 0 then begin
        FResult := 103;
        Exit;
    end;
    if Size < 16 then begin
        FResult := 13; { Invalid data }
        Exit;
    end;
    if (T.Data.BlockType = 0) then begin
        if T.Data.BlockSize <> FDataStart then
            FResult := 13
        else
            FResult := 18; { No Records }
        exit;
    end;
    if (Size < Sizeof(T.Data) + 3) or (T.Data.BlockSize < Sizeof(T.Data) + 3) then begin
        FResult := 13;
        Exit;
    end;
    { Validate Position + Block Size against File Size }
    InvalidBlock := False;
end;

function TResource.GetFooter : boolean;
var
    ID : TResourceBlockID;
    S  : String;
begin
    GetFooter := false;
    FHasFooter := false;
    FDataStart := -1;
    if InvalidHandler then exit;

    if FHandler^.FileSize >= Sizeof(ID) then begin
        FHandler^.Seek(FHandler^.FileSize - SizeOf(ID));
        if HandlerError then exit;
        FHandler^.ReadRecord(ID, Sizeof(ID));
        if HandlerError then exit;
        S := Space(Length(FResID));
        Move(ID.Identifier, S[1], Sizeof(ID.Identifier));
        if (ID.BlockType = 0) and (S = FResID) then begin
            FHasFooter := true;
            FDataStart := ID.HeadBlock;
        end;
    end;

    if FDataStart = -1 then begin
        if (FReadOnly or FExistOnly) then begin
            FResult := 5;
            exit;
        end;
        { We can append resource to an existing or new file }
        FDataStart := FHandler^.FileSize;
        Fmodified := True;
    end;

    GetFooter := True;
end;

function TResource.PutFooter : boolean;
var
    ID : TResourceBlockID;
begin
    PutFooter := false;
    if InvalidHandler then exit;
    if FReadOnly then begin
        FResult := 5;
        exit;
    end;

    if FHasFooter then
        FHandler^.Seek(FHandler^.FileSize - SizeOf(ID))
    else
        FHandler^.Seek(FHandler^.FileSize);
    if HandlerError then exit;

    FillChar(ID, Sizeof(ID), 0);
    ID.HeadBlock := FDataStart;
    Move(FResID[1], ID.Identifier, Sizeof(ID.Identifier));
    FHandler^.WriteRecord(ID, Sizeof(ID));
    if HandlerError then exit;
    FHasFooter := True;

    PutFooter := True;
end;

function TResource.GetResult : integer;
begin
    GetResult := FResult;
end;

function TResource.IOResult : integer;
begin
    IOResult := GetResult;
    FResult := 0;
end;

function TResource.Open : boolean;
var
    FM : word;
begin
    Open := False;
    if InvalidHandler then exit;
    if (FResName = '') then begin
        FResult := 2; { File not found. Not opening CON! }
        Exit;
    end;
    FM := FileMode;
    if FReadOnly then FileMode := 0;
    FHandler^.Assign(FResName);
    if FHandler^.Result = 0 then FHandler^.Reset;
    if (FHandler^.Result = 2) and (not FReadOnly) then begin { Doesn't exist }
        FHandler^.Rewrite;
        FExistOnly := False;
    end;
    FResult := FHandler^.IOResult;
    if (FResult = 0) and (not GetFooter) then begin
        if FResult = 0 then begin
            Close;
            FResult := 5 { file access denied }
        end;
    end;
    FileMode := FM;
    Open := FResult = 0;
end;

function TResource.Close : boolean;
begin
    Close := False;
    if InvalidHandler then begin
        FDataStart := -1;
        exit;
    end;
    if (not FReadOnly) and (FDataStart <> -1) and FModified then PutFooter;
    FHandler^.Close;
    FDataStart := -1;
    FHasFooter := false;
    FModified := false;
    if FResult = 0 then FResult := FHandler^.IOResult;
    Close := FResult = 0;
end;

function TResource.FindFirst(AFileSpec : String; var ARec : TResourceRec) : boolean;
var
    T : TResourceRec;
    C : Word;
begin
    FindFirst := False;
    FillChar(ARec, Sizeof(ARec), 0);
    ARec.Position := -1;
    if InvalidHandler then exit;
    if FDataStart = -1 then begin
        FResult := 103;
        Exit;
    end;
    T.Position := FDataStart;
    FHandler^.Seek(T.Position);
    if HandlerError then Exit;
    FHandler^.BlockRead(T.Data, Sizeof(T.Data) + Sizeof(T.Name), C);
    if HandlerError then Exit;
    if InvalidBlock(T, C) then Exit;
    ARec.Position := T.Position;
    ARec.Data := T.Data;
    ARec.Name := T.Name;
    FindFirst := True;
end;

function TResource.FindNext(var ARec : TResourceRec) : boolean;
var
    T : TResourceRec;
    C : Word;
begin
    { Almost Identical to FindFirst, Could consolidate these. But, eh, why? }
    FindNext := False;
    if InvalidHandler then exit;
    if (FDataStart = -1) then begin
        FResult := 103;
        Exit;
    end;
    if InvalidBlock(ARec, Sizeof(ARec)) then exit;
    T.Position := ARec.Position + ARec.Data.BlockSize;
    FHandler^.Seek(T.Position);
    if HandlerError then Exit;
    FHandler^.BlockRead(T.Data, Sizeof(T.Data) + Sizeof(T.Name), C);
    if HandlerError then Exit;
    if InvalidBlock(T, C) then Exit;
    ARec.Position := T.Position;
    ARec.Data := T.Data;
    ARec.Name := T.Name;
    FindNext := True;
end;

function TResource.Import(ABasePath, AFileSpec : String; AStatus : TResourceStatusProc ) : boolean;
const
    Zero : byte = 0;
var
    FSearch : TSearchRec;
    DB : TResourceBlockData;
    F : TFile;
    FM : word;
    Abort : boolean;
    FN : String;
    E : integer;
    C : Word;
    W : LongInt;
    LP, P : word;

begin
    Abort := False;
    Import := False;
    if ABasePath <> '' then
        QDOS.FindFirst(TailDelim(ABasePath) + AFileSpec, faAnyFile, FSearch)
    else
        QDOS.FindFirst(AFileSpec, faAnyFile, FSearch);
    if DOSError <> 0 then begin
        FResult := DOSError;
        exit;
    end else begin
        if FReadOnly then begin
            FResult := 5;
            Exit;
        end;
        FResult := 0;
        while (DOSError = 0) and (not Abort) and (FResult = 0) do begin
            if (FSearch.Attr and faDirectory <> faDirectory) and
            (FSearch.Attr and faVolumeID <> faVolumeID) then begin
                if FJunkPaths then
                    FN := FSearch.Name
                else
                    FN := PathOf(AFileSpec) + FSearch.Name;
                if Assigned(AStatus) then
                    if not AStatus(FN, 0) then
                        Abort := True;
                 if FHasFooter then
                    FHandler^.Seek(FHandler^.FileSize - SizeOf(TResourceBlockID))
                else
                    FHandler^.Seek(FHandler^.FileSize);
                if HandlerError then exit;
                FM := FileMode;
                FileMode := 0;
                F.Create;
                F.Assign(PathOf(AFileSpec) + FSearch.Name);
                F.Reset;
                FileMode := FM;
                if F.Result = 0 then begin
                    LP := 0;
                    FHasFooter := False;
                    DB.BlockType := 1; { File }
                    DB.Attr := FSearch.Attr;
                    DB.Size := FSearch.Size;
                    DB.Time := FSearch.Time;
                    DB.BlockSize := SizeOf(DB) + Length(FN) + 2 + DB.Size;
                    FHandler^.WriteRecord(DB, Sizeof(DB));
                    if FHandler^.Result = 0 then
                       FHandler^.WriteRecord(FN, Length(FN) + 1);
                    if FHandler^.Result = 0 then
                       FHandler^.WriteByte(Zero);
                    W := 0;
                    While (F.Result = 0) and (FHandler^.Result = 0)
                    and (W < DB.Size) do begin
                        F.BlockRead(FBuffer, Sizeof(FBuffer), C);
                        FHandler^.WriteRecord(FBuffer, C);
                        Inc(W, C);
                        P := (W * 100 div DB.Size); { Meh }
                        if (LP <> P) and (P < 100) then begin
                            if Assigned(AStatus) then
                                if not AStatus(FN, P) then
                                    Abort := True;
                            LP := P;
                        end;
                    end;

                    if (F.Result = 0) and (FHandler^.Result = 0) then
                        PutFooter;

                end;

                FResult := F.Result;
                F.Close;
                F.Destroy;
                if FResult = 0 then FResult := FHandler^.Result;

                if FResult <> 0 then begin
                    { if Assigned(AStatus) then AStatus(FN, 0) then }
                    E := FResult;
                    FHandler^.Close;
                    FResult := E;
                    Exit;
                end;
                if Assigned(AStatus) then
                    if not AStatus(FN, 100) then
                        Abort := True;
            end;
            QDOS.FindNext(FSearch);
        end;
        if DOSError <> 18 then begin
            FResult := DOSError;
            Exit;
        end else
            FResult := 0;
    end;
    Import := True;
end;

function TResource.ExportResource(ABasePath : String; ARec : TResourceRec;
AStatus : TResourceStatusProc) : boolean;
var
    F : TFile;
    FN, TFN : String;
    Abort : boolean;
    W : LongInt;
    C : Word;
    LP, P : Word;
begin
    Abort := False;
    ExportResource := False;
    if InvalidHandler then exit;
    if (FDataStart = -1) then begin
        FResult := 103;
        Exit;
    end;
    if InvalidBlock(ARec, Sizeof(ARec)) then exit;
    if ARec.Data.BlockType <> 1 then begin
        FResult := 1;
        Exit;
    end;
    FHandler^.Seek(ARec.Position + Sizeof(TResourceBlockData) +
        Length(ARec.Name) + 2 );
    if HandlerError then exit;
    F.Create;
    FN := ARec.Name;
    if FJunkPaths then System.Delete(FN, 1, Length(PathOf(FN)));
    if ABasePath <> '' then TFN := TailDelim(AbasePath) + FN else TFN := FN;
    if Assigned(AStatus) then
        if not AStatus(FN, 0) then Abort := True;
    if (PathOf(TFN) <> '' ) and (not DirExists(NoTailDelim(Pathof(TFN)))) then begin
        FResult := MakeDir(NoTailDelim(Pathof(TFN)));
    end;
    if FResult = 0 then begin
        F.Assign(TFN);
        F.Rewrite;
        FResult := F.Result;
        W := 0;
        LP := 0;
        while (FResult = 0) and (W < ARec.Data.Size) do begin
            if  Sizeof(FBuffer) < (ARec.Data.Size - W) then
                C := Sizeof(FBuffer)
            else
                C := ARec.Data.Size - W;
            FHandler^.ReadRecord(FBuffer, C);
            FResult := FHandler^.Result;
            if FResult = 0 then begin
                F.WriteRecord(FBuffer, C);
                FResult := F.Result;
                Inc(W, C);
            end;
            P := (W * 100 div ARec.Data.Size);
            if (LP <> P) and (P < 100) then begin
                if Assigned(AStatus) then
                    if not AStatus(FN, P) then
                        Abort := True;
                LP := P;
            end;
        end;
        if FResult = 0 then SetFTime(F.FRec, ARec.Data.Time);
        F.Close;
        if FResult = 0 then SetFAttr(F.FRec, ARec.Data.Attr);
    end;
    F.Destroy;
    if Assigned(AStatus) and (FResult = 0) then
        if not AStatus(FN, 100) then Abort := True;
    ExportResource := (Not Abort) and (FResult = 0);
end;

function TResource.Export(ABasePath, AFileSpec : String; AStatus : TResourceStatusProc ) : boolean;
var
    ARec : TResourceRec;
    S : String;
begin
    AFileSpec:=UCase(AFileSpec);
    FindFirst('', ARec);
    While FResult = 0 do begin
        if ARec.Data.BlockType = 1 then begin
            if MatchWildCard(AFileSpec, uCase(ARec.Name)) then begin
                if not ExportResource(ABasePath, ARec, AStatus) then
                    Break;
            end;
        end;
        FindNext(ARec);
    end;
    if FResult = 18 then begin
        FResult := 0;
    end;
    Export := FResult = 0;
end;

function TResource.ExportAll(ABasePath : String; AStatus : TResourceStatusProc ) : boolean;
var
    ARec : TResourceRec;
    S : String;
begin
    FindFirst('', ARec);
    While FResult = 0 do begin
        if ARec.Data.BlockType = 1 then begin
            if not ExportResource(ABasePath, ARec, AStatus) then
                Break;
        end;
        FindNext(ARec);
    end;
    if FResult = 18 then begin
        FResult := 0;
    end;
    ExportAll := FResult = 0;
end;

function TResource.Delete(AFileSpec : String; AStatus : TResourceStatusProc ) : boolean;
var
    E : integer;
begin
    Delete := False;
    if InvalidHandler then exit;
    if FDataStart = -1 then begin
        FResult := 103;
        Exit;
    end;
    if FReadOnly then begin
        FResult := 5;
        Exit;
    end;
{    FHandler^.Seek(FDataStart);
    if HandlerError then exit;
    FHandler^.Truncate;
    FHasFooter := False;
    if HandlerError then begin
        E := FResult;
        FHandler^.Close;
        FResult := E;
        Exit;
    end; }
    Delete := True;
end;

function TResource.DeleteAll : boolean;
var
    E : integer;
begin
    DeleteAll := False;
    if InvalidHandler then exit;
    if FDataStart = -1 then begin
        FResult := 103;
        Exit;
    end;
    if FReadOnly then begin
        FResult := 5;
        Exit;
    end;
    FHandler^.Seek(FDataStart);
    if HandlerError then exit;
    FHandler^.Truncate;
    FHasFooter := False;
    if HandlerError then begin
        E := FResult;
        FHandler^.Close;
        FResult := E;
        Exit;
    end;
    DeleteAll := True;
end;


procedure InitUnit;
begin
end;

begin
  InitUnit;
end.
