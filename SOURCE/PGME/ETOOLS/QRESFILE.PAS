{ Copyright 2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ETOOLS.DEF}
program Eternity_Resource_File_Toolkit; { X Series }

uses QStrings, QDos, QFiles, QNLS, QResrc;

{$I ..\ETERNITY\LICENSE.INC}
{$I ..\VERSION.INC}

var
    CON : Text;

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

    db 'COUNT.FILES=$ file(s)',0
    db 'BYTES.FILES=$ bytes(s)',0

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
    db 'HELP.14=   $X              Export all resource data to files',0
    db 'HELP.15=   $I  filespec    Import resource from filename',0
    db 'HELP.16=   $J              Ignore path information for resource',0
    db 'HELP.17=   $G  filename    Read instructions from file',0
    db 'HELP.18=   $D  filespec    Delete resource filespec from file',0
    db 'HELP.19=   $K              Remove ALL resource data from file',0
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
    WriteLn(CON,Data);
    Halt(ExitCode);
end;

procedure DisplaySupport;
var
    S : String;
begin
    S:=NLSstr('SUPPORT', NLS('PROG.TITLE'));
    if S <> '' then begin
        WriteLn(CON,S);
        WriteLn(CON,'');
    end;
end;

procedure DisplayVersion;
begin
    WriteLn(CON,NLS('PROG.TITLE') + ', ' + NLSstr('PROG.VERSION',
        ZPad(IntStr(Version.Year), 4) + '-' +
        ZPad(IntStr(Version.Month), 2) + '-' +
        ZPad(IntStr(Version.Day), 2)
        ));
    WriteLn(CON,NLS('PROG.DESC'));
    WriteLn(CON,'');
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
        WriteLn(CON,S);
        Inc(I);
        W := GetNLS('HELP.' + IntStr(I), S);
        S := ReplaceStr(S, '$', SwitchChar, True);
    end;
    WriteLn(CON,'');
    Halt(0);
end;

function ImportMsg(AFileName : String; ACompleted, ABytes : word) : boolean; far;
const
    LW : byte = 0;
var
    S : String;
begin
    ImportMsg:=True;
    S := NLSStr('IMPORT', AFileName) + ' ' + NLSStr('PERCENT', IntStr(ACompleted));
    if ACompleted <> 0 then
        Write(CON,ChrStr(#8, LW), Space(LW), ChrStr(#8, LW));
    Write(CON,S);
    if ACompleted = 100 then begin
        WriteLN;
        LW := 0
    end else
        LW := Length(S);
end;

function ExportMsg(AFileName : String; ACompleted, ABytes : word) : boolean; far;
const
    LW : byte = 0;
var
    S : String;

begin
    ExportMsg:=True;
    S := NLSStr('EXPORT', AFileName) + ' ' + NLSStr('PERCENT', IntStr(ACompleted));
    if ACompleted <> 0 then
        Write(CON,ChrStr(#8, LW), Space(LW), ChrStr(#8, LW));
    Write(CON,S);
    if ACompleted = 100 then begin
        WriteLN;
        LW := 0
    end else
        LW := Length(S);
end;

type
    TParamStrFunc = function (I : integer) : String;
    TParamCountFunc = function : integer;

procedure Run(FParamCount : TParamCountFunc; FParamStr : TParamStrFunc); forward;

var
    PRes : PResource;
    CanCreate,
    CanModify,
    CanAppend,
    NoPaths : boolean;
    Opts : String;

function GetParamStr(I : integer) : String; far;
begin
    GetParamStr := ParamStr(I);
end;

function GetParamCount : integer; far;
begin
    GetParamCount := ParamCount;
end;

function GetOptStr(I : integer) : String; far;
var
    N, S : String;
    X : integer;
begin
    if I  = 0 then begin
        GetOptStr := ParamStr(0);
        Exit;
    end;
    S := Opts;
    for X := 1 to I do
        GetOptStr := PullStr(' ', S);
end;

function GetOptCount : integer; far;
var
    S : String;
    I : Integer;
begin
    S := Opts;
    I := 0;
    While (S <> '') do begin
        Inc(I);
        PullStr(' ', S);
    end;
    GetOptCount := I;
end;

procedure RunFile(AFileName : String);
var
    FM : integer;
    F : TFile;
begin
    FM := FileMode;
    FileMode := FM;
    F.Create;
    F.Assign(AFilename);
    F.Reset;
    FileMode := FM;
    while F.Result = 0 do begin
        F.ReadLn(Opts, Sizeof(Opts) - 1);
        Opts := Trim(Opts);
        if (F.Result = 0) and (Opts <> '') then
        	Run(GetOptCount, GetOptStr);
    end;
    F.Destroy;
end;

procedure Run(FParamCount : TParamCountFunc; FParamStr : TParamStrFunc);
var
    RSearch : TResourceRec;
    Err : integer;
    Opt: String;
    I : integer;
    OPath : String;
    FCount, FSize : longint;
begin
	if FParamCount = 0 then  DisplayHelp;
    OPath     := '';
	Err := 0;

	for I := 0 to FParamCount do
	    if (FParamStr(I) = '/?')
	    or (UCase(FParamStr(I)) = '/H') then DisplayHelp;

    I := 1;
    While I <= FParamCount do begin
	    Opt := UCase(FParamStr(I));
	    if (Length(Opt) < 2) or (Opt[1] <> SwitchChar) then begin
        	WriteLn(CON,ErrorMsg(1, Opt));
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
	            FCount := 0;
	            FSize := 0;
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
                PRes^.FindFirst('', RSearch);
                While PRes^.GetResult = 0 do begin
                    if RSearch.Data.BlockType <> 1 then begin
                        WriteLn(CON,NLSStr('TYPE.UNKOWN', IntStr(RSearch.Data.BlockType)), ' ',
                            NLSStr('SIZE.DATA', IntStr(RSearch.Data.BlockSize - 6)));
                    end else begin
                        WriteLn(CON,NLSStr('TYPE.FILE', RSearch.Name), ' ',
                            NLSStr('SIZE.FILE', IntStr(RSearch.Data.Size)));
                            Inc(FCount);
                            Inc(FSize, RSearch.Data.Size );
                    end;
                    PRes^.FindNext(RSearch);
                end;
                if PRes^.GetResult <> 18 then begin
                  	WriteLn(CON,ErrorMsg(PRes^.GetResult, Opt));
                    Err := PRes^.GetResult;
                    Break;
                end;
                WriteLn(CON,NLSStr('COUNT.FILES', IntStr(FCount)), ' ',
                            NLSStr('BYTES.FILES', IntStr(FSize)));
	        end;
	        'E' : begin {Export File}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
                Opt := FParamStr(I);
	            Inc(I);
                if not PRes^.Export('', Opt, ExportMsg) then begin
                	WriteLn(CON,ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
                end;
	        end;
	        'X' : begin {Export All Files}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
                if not PRes^.ExportAll(OPATH, ExportMsg) then begin
                	WriteLn(CON,ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
                end;
	        end;
	        'I' : begin {Import file}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
                Opt := FParamStr(I);
	            Inc(I);
                if not PRes^.Import('', Opt, ImportMsg) then begin
                	WriteLn(CON,ErrorMsg(PRes^.GetResult, Opt));
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
	            WriteLn(CON,NLSstr('NOFUNC', SwitchChar + Opt));
	            Err := 1;
	            Break;
	        end;
	        'G' : begin  {Delete resource}
	            Opt := FParamStr(I);
	            Inc(I);
	            if Opt = '' then WriteHalt(ErrorMsg(5, 'CON:'),5);
	            RunFile(Opt);
	        end;
	        'K' : begin {Remove all Resource data}
	            if Not Assigned(PRes) then WriteHalt(ErrorMsg(5, 'CON:'),5);
	            if not PRes^.DeleteAll then begin
                	WriteLn(CON,ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
        	    end;
	        end;
	        'F' : begin { Set filename }
                if Assigned(PRes) then Dispose(PRes, Destroy);
                PRes := nil;
	            Opt := FParamStr(I);
	            Inc(I);
	            if (Opt = '') or ((Not CanCreate) and (Not FileExists(Opt))) then begin
  	                WriteHalt(ErrorMsg(2, Opt),2);
	            end;

               PRes := New(PResource, Create(Opt, New(PFile, Create)));
               PRes^.FReadOnly := not (CanCreate or CanModify or CanAppend);
               PRes^.FJunkPaths := NoPaths;
               if CanAppend then PRes^.FExistOnly := False;
               if not PRes^.Open then begin
                	WriteLn(CON,ErrorMsg(PRes^.GetResult, Opt));
                	Err := PRes^.GetResult;
        	        Break;
        	    end;
	        end;
	    else
        	WriteLn(CON,ErrorMsg(1, SwitchChar + Opt));
        	Err := 1;
        	Break;
        end;
    end;
	if Err <> 0 then begin
    	if Assigned(PRes) then Dispose(PRes, Destroy);
	    Halt(Err);
	end;
end;

begin
	CanCreate := False;
	CanModify := False;
	CanAppend := False;
	NoPaths   := False;
	PRes := nil;
    Assign(CON, '');
    Append(CON);
    SetBuiltInLanguage(@ProgramMessages);
    SetLanguage('');
	Run(GetParamCount, GetParamStr);
	Close(CON);
	if Assigned(PRes) then Dispose(PRes, Destroy);
end.
