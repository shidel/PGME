{ Copyright 2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ETOOLS.DEF}
program Eternity_Resource_File_Toolkit; { X Series }

uses QObjects, QStrings, QErrors, QDos, QFiles, QNLS, QResrc,
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
    TFileBlockData = record
        BlockType : word;        { = 1 }
        BlockSize : LongInt;     { Total size of block }
    	Attr      : byte;
    	Time      : LongInt;
    	Size      : LongInt;
    end;
    TFileBlockName = record
    	Name      : String;
    	Zero      : byte;        { = 0, Null terminator }
    end;
    TFileBlock = record          { Maximum Sized File Block Header }
        Data : TFileBlockData;
        Name : TFileBlockName;
    end;

    PResource = ^TResource;
    TResource = object (TObject)
	public { private }
	    FResID     : String;
	    FHandler   : PAbstractFile;
	    FResult    : Integer;
	    FResName   : String;
	    FReadOnly  : boolean;
	    FExistOnly : boolean; { Must already have resource data }
	    FDataStart : longint;
	    FHasFooter : boolean;
	    FModified  : boolean;
      procedure   SetHandler(AHandler : PAbstractFile); virtual;
      function    GetHandler : PAbstractFile; virtual;
      function    InvalidHandler : boolean; virtual;
      function    HandlerError : boolean; virtual;
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
    db 'ERROR.4=access to "$" denied',0

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
    db 'HELP.16=   $P              Ignore path information for resource',0
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
    S := NLSstr('ERROR.' + Err, Data);
    if S  = '' then
        R := ReplaceStr(R, '$', Data, false)
    else
        R := ReplaceStr(R, '$', S, false);
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

procedure Run;
var
    PRes : PResource;
    Err : integer;
    Opt : String;
    CanCreate,
    CanModify,
    CanAppend : boolean;
    I : integer;
begin
	if ParamCount = 0 then  DisplayHelp;

	CanCreate := False;
	CanModify := False;
	CanAppend := False;
	PRes := nil;
	Err := 0;

	for I := 0 to ParamCount do
	    if (ParamStr(I) = '/?')
	    or (UCase(ParamStr(I)) = '/H') then DisplayHelp;

    I := 1;
    While I <= ParamCount do begin
	    Opt := UCase(ParamStr(I));
	    if (Length(Opt) < 2) or (Opt[1] <> SwitchChar) then begin
        	WriteLn(ErrorMsg(1, Opt),1);
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
	        'L' : begin end; {List Resource}
	        'E' : begin end; {Export File}
	        'O' : begin end; {Export All Files}
	        'I' : begin end; {Import file}
	        'P' : begin end; {Ignore path data}
	        'D' : begin end; {Delete resource}
	        'X' : begin end; {Remove all Resource data}
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
               if CanAppend then PRes^.FExistOnly := False;
               if not PRes^.Open then begin
                	WriteLn(ErrorMsg(PRes^.GetResult, Opt),PRes^.GetResult);
                	Err := 1;
        	        Break;
        	    end;
	        end;
	    else
        	WriteLn(ErrorMsg(1, Opt),1);
        	Err := 1;
        	Break;
        end;
    end;
	if Assigned(PRes) then Dispose(PRes, Destroy);
	If Err <> 0 then Halt(Err);
end;

begin
	ErrorsOnExit := false;
	ErrorClrScr := false;
    SetBuiltInLanguage(@ProgramMessages);
    SetLanguage('');
	Run;
end.
