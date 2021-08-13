{ Copyright 2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ETOOLS.DEF}
program Eternity_Resource_File_Toolkit; { X Series }

uses QStrings, QDos, QFiles, QNLS, QResrc, QCrt,
    QClass;

{$I ..\ETERNITY\LICENSE.INC}
{$I ..\VERSION.INC}

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

{ --------------------- }

procedure ProgramMessages; assembler;
asm
    db 'LANGUAGE=Built-in',0
    db 'PROG.NAME=QResFile',0
    db 'PROG.VERSION=Version $',0
    db 'PROG.TITLE=QResFile',0
    db 'PROG.DESC=Eternity Resource File Management Utility',0

    db 'ERROR=Error (#$), $',0
    db 'ERROR.1=invalid function "$"',0
    db 'ERROR.2=file "$" not found',0
    db 'ERROR.3=path "$" not found',0
    db 'ERROR.5=access to "$" denied',0

    db 'IMPORT=import: $',0
    db 'EXPORT=export: $',0
    db 'PERCENT=$%',0

    db 'NOFUNC=Sorry, the "$" option has not been implemented (yet).',0

    db 'TYPE.UNKNOWN=UNKNOWN: Type $,',0
    db 'TYPE.FILE=FILE: $,',0

    db 'SIZE.DATA=$ byte(s)',0
    db 'SIZE.FILE=$ byte(s)',0

    db 'HELP.0=Usage $: [options] [package]',0
    db 'HELP.1=',0
    db 'HELP.2=   $F filename     Specify filename with resource data',0
    db 'HELP.3=',0
    db 'HELP.4=   $R              Read Only (this is the default!)',0
    db 'HELP.5=',0
    db 'HELP.6=These options disable read only mode (must be before $F):',0
    db 'HELP.7=   $C              Allow file to be created',0
    db 'HELP.8=   $M              Allow file with resource data to be modified',0
    db 'HELP.9=   $A              Allow file without resource data to have it appended',0
    db 'HELP.10=',0
    db 'HELP.11=General functions:',0
    db 'HELP.12=   $L              List resources',0
    db 'HELP.13=   $E  filespec    Export resource to file',0
    db 'HELP.14=   $O              Export all resource data to files',0
    db 'HELP.15=   $I  filespec    Import resource from filename',0
    db 'HELP.16=   $J              Ignore path information for resource',0
    db 'HELP.17=',0
    db 'HELP.18=   $D  filespec    Delete resource filespec from file',0
    db 'HELP.19=   $X              Remove ALL resource data from file',0
    db 'HELP.21=',0
    db 'HELP.22=   $?              Display help and exit',0

    db 0
end;

function ErrorTxt(Err : String; Data : String) : string;
var
    R : String;
    S : String;
begin
    R := Trim(ReplaceStr(NLS('ERROR'), '$', Err, false));
    S := NLS('ERROR.' + Err);
    if S  = '' then
        R := ReplaceStr(R, '$', Data, false)
    else
        R := ReplaceStr(R, '$', ReplaceStr(S, '$', Data, false), false);
    ErrorTxt := Trim(R);
end;

function ErrorMsg(Err : integer; Data : String) : string;
begin
    ErrorMsg := ErrorTxt(IntStr(Err), Data);
end;

procedure WriteHalt (Data : String; ExitCode : integer);
begin
    WriteLn(Data);
    Halt(ExitCode);
end;

procedure DisplaySupport;
var
    S : String;
begin
    S:=NLSstr('SUPPORT', NLS('PROG.TITLE'));
    if S <> '' then begin
        WriteLn(S);
        WriteLn('');
    end;
end;

procedure DisplayVersion;
begin
    WriteLn(NLS('PROG.TITLE') + ', ' + NLSstr('PROG.VERSION',
        ZPad(IntStr(Version.Year), 4) + '-' +
        ZPad(IntStr(Version.Month), 2) + '-' +
        ZPad(IntStr(Version.Day), 2)
        ));
    WriteLn(NLS('PROG.DESC'));
    WriteLn('');
end;

procedure DisplayHelp;
var
    I, W, H : integer;
    S : String;
begin
    DisplayVersion;
    I := 0;
    W := GetNLS('HELP.' + IntStr(I), S);
    S := ReplaceStr(S, '$', GetEXEBase, True);
    if W < 0 then Halt(1);
    H := W;
    while (H = W) do begin
        WriteLn(S);
        Inc(I);
        W := GetNLS('HELP.' + IntStr(I), S);
        S := ReplaceStr(S, '$', SwitchChar, True);
    end;
    WriteLn('');
    Halt(0);
end;

function ImportMsg(AFileName : String; ACompleted : word) : boolean; far;
const
    LW : byte = 0;
var
    S : String;

begin
    ImportMsg:=True;
    S := NLSStr('IMPORT', AFileName) + ' ' + NLSStr('PERCENT', IntStr(ACompleted));
    if ACompleted <> 0 then
        Write(ChrStr(#8, LW), Space(LW), ChrStr(#8, LW));
    Write(S);
    if ACompleted = 100 then begin
        WriteLN;
        LW := 0
    end else
        LW := Length(S);
end;

function ExportMsg(AFileName : String; ACompleted : word) : boolean; far;
const
    LW : byte = 0;
var
    S : String;

begin
    ExportMsg:=True;
    S := NLSStr('EXPORT', AFileName) + ' ' + NLSStr('PERCENT', IntStr(ACompleted));
    if ACompleted <> 0 then
        Write(ChrStr(#8, LW), Space(LW), ChrStr(#8, LW));
    Write(S);
    if ACompleted = 100 then begin
        WriteLN;
        LW := 0
    end else
        LW := Length(S);
end;

procedure Run;
var
    PRes : PResource;
    RSearch : TResourceRec;
    Err : integer;
    Opt: String;
    CanCreate,
    CanModify,
    CanAppend,
    NoPaths : boolean;
    I : integer;
begin
	if ParamCount = 0 then  DisplayHelp;

	CanCreate := False;
	CanModify := False;
	CanAppend := False;
	NoPaths   := False;
	PRes := nil;
	Err := 0;

	for I := 0 to ParamCount do
	    if (ParamStr(I) = '/?')
	    or (UCase(ParamStr(I)) = '/H') then DisplayHelp;

    I := 1;
    While I <= ParamCount do begin
	    Opt := UCase(ParamStr(I));
	    if (Length(Opt) < 2) or (Opt[1] <> SwitchChar) then begin
        	WriteLn(ErrorMsg(1, Opt));
        	Err := 1;
        	Break;
	    end;
        Inc(I);
	    Delete(Opt, 1,1);
	    case Opt[1] of
	        'R' : begin
                CanCreate := False;
                CanModify := False;
                CanAppend := False;
	        end;
	        'C' : CanCreate := True;
	        'M' : CanModify := True;
	        'A' : begin
	            CanAppend := True;
                CanModify := True;
	        end;
	        'L' : begin {List Resource}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
                PRes^.FindFirst('', RSearch);
                While PRes^.GetResult = 0 do begin
                    if RSearch.Data.BlockType <> 1 then begin
                        WriteLn(NLSStr('TYPE.UNKOWN', IntStr(RSearch.Data.BlockType)), ' ',
                            NLSStr('SIZE.DATA', IntStr(RSearch.Data.BlockSize - 6)));
                    end else begin
                        WriteLn(NLSStr('TYPE.FILE', RSearch.Name), ' ',
                            NLSStr('SIZE.FILE', IntStr(RSearch.Data.Size)));
                    end;
                    PRes^.FindNext(RSearch);
                end;
                if PRes^.GetResult <> 18 then begin
                  	WriteLn(ErrorMsg(PRes^.GetResult, Opt));
                    Err := PRes^.GetResult;
                    Break;
                end;
	        end;
	        'E' : begin {Export File}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
                Opt := ParamStr(I);
	            Inc(I);
                if not PRes^.Export('', Opt, ExportMsg) then begin
                	WriteLn(ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
                end;
	        end;
	        'O' : begin {Export All Files}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
                if not PRes^.ExportAll('OUT', ExportMsg) then begin
                	WriteLn(ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
                end;
	        end;
	        'I' : begin {Import file}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
                Opt := ParamStr(I);
	            Inc(I);
                if not PRes^.Import('', Opt, ImportMsg) then begin
                	WriteLn(ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
                end;

	        end;
	        'J' : begin {Ignore path data}
	            NoPaths := True;
                if Assigned(PRes) then PRes^.FJunkPaths := NoPaths;
	        end;
	        'D' : begin  {Delete resource}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
	            WriteLn(NLSstr('NOFUNC', SwitchChar + Opt));
	            Err := 1;
	            Break;
	        end;
	        'X' : begin {Remove all Resource data}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
	            if not PRes^.DeleteAll then begin
                	WriteLn(ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
        	    end;
	        end;
	        'F' : begin { Set filename }
                if Assigned(PRes) then Dispose(PRes, Destroy);
                PRes := nil;
	            Opt := ParamStr(I);
	            Inc(I);
	            if (Opt = '') or ((Not CanCreate) and (Not FileExists(Opt))) then begin
  	                WriteHalt(ErrorMsg(2, Opt),2);
	            end;

               PRes := New(PResource, Create(Opt, New(PFile, Create)));
               PRes^.FReadOnly := not (CanCreate or CanModify or CanAppend);
               PRes^.FJunkPaths := NoPaths;
               if CanAppend then PRes^.FExistOnly := False;
               if not PRes^.Open then begin
                	WriteLn(ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
        	    end;
	        end;
	    else
        	WriteLn(ErrorMsg(1, SwitchChar + Opt));
        	Err := 1;
        	Break;
        end;
    end;
	if Assigned(PRes) then Dispose(PRes, Destroy);
	If Err <> 0 then Halt(Err);
end;

begin
	{ ErrorsOnExit := false;
	ErrorClrScr := false; }
    SetBuiltInLanguage(@ProgramMessages);
    SetLanguage('');
	Run;
end.
