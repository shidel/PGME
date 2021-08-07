{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ..\QUICKCRT\TEMPLATE.DEF}
{$I ETERNITY.DEF}
unit PGM_Call;

interface

procedure CallBack(const Command, Params, RetStr : String; const ComeBack : boolean);

implementation

uses QStrings;

var
	CallBackPtr : Pointer;
	CallBackBuffer  : record
		S : boolean;
		C, P, R : String;
	end;

procedure DoCallBack(const Buffer); assembler;
asm
  PUSH ES
  PUSH BX
  LES BX, DWORD PTR Buffer
  CALL DWORD PTR CallBackPtr
  POP BX
  POP ES
end;

procedure CallBack(const Command, Params, RetStr : String; const ComeBack : boolean);
begin
	with CallBackBuffer do begin
		S := ComeBack;
		C := Command;
		P := Params;
		R := RetStr;
	end;
	DoCallBack(CallBackBuffer);
end;

procedure Init;
var
	Sig : ^String;
begin
	if Pos('/CALL=', ParamStr(1)) <> 1  then begin
		WriteLn('Please use PGM.EXE to start "Program Manager Eternity."');
		halt($F001);
	end;

	Sig := Ptr(StrInt(Copy(ParamStr(1), 7, 10)), 3);
	if Sig^ <> 'ETERNITY' then begin
		WriteLn('Invalid Callback address. Terminated.');
		halt($F002);
	end;

	CallBackPtr := Ptr(StrInt(Copy(ParamStr(1), 7, 10)), 0);
	CallBack('', '', '', False);
end;

begin
	Init;
end.
