{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QLog; { QuickCrt Simple Log File }
{$I QCRT.DEF}

interface

uses QCrt;

	procedure LogInc;
	procedure LogDec;
	procedure WriteLog(S : String);

implementation

var
	TextFile : Text;
	IsOpen : boolean;
	Indent : integer;
	OldExitProc : Pointer;
	
procedure QLogExitProc; far; 
begin
	ExitProc := OldExitProc;
	if IsOpen then begin
		LogDec;
		Append(TextFile);
		WriteLn(TextFile, 'CLOSED');
		Close(TextFile);
	end;
end;

procedure InitQLog; 
begin
	IsOpen := False;
	Indent := 0;
	OldExitProc := ExitProc;
	ExitProc := @QLogExitProc;
end;

procedure OpenLog;
var
	INDEX : integer;
	LOG : String;
begin
	LOG := ParamStr(0);
	INDEX := Length(LOG);
	while LOG[INDEX] <> '.' do Dec(Index);
	LOG := Copy(LOG, 1, INDEX) + 'LOG';
	{$F-}
	Assign(TextFile, LOG);
	{$IFDEF DEVLOG_CLEAN}
		Rewrite(TextFile);
	{$ELSE}
		Append(TextFile);
		if IOResult <> 0 then
			Rewrite(TextFile);
	{$ENDIF}
	Close(TextFile);
	IsOpen := IOResult = 0;
	if IsOpen then WriteLog('OPEN LOG');
	LogInc;
	IsOpen := IOResult = 0;	
	{$F+}
end;

function Indentation : String;
var
	Index : integer;
	S : String;
begin
	S := '';
	for INDEX := 1 to Indent do
		S := S + #09 {#32};
	Indentation := S;
end;

procedure LogInc; 
begin
	Inc(Indent);
end;

procedure LogDec; 
begin
	Dec(Indent);
end;

procedure WriteLog(S : String);
begin
	if Not IsOpen then OpenLog;
	if IsOpen then begin
		Append(TextFile);
		WriteLn(TextFile, Indentation + S);
		CLose(TextFile);
	end;
end;

begin
	InitQLog;
end.

