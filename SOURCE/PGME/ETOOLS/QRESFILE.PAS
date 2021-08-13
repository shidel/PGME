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
