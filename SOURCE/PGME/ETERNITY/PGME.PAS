{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$M 2048,0,0}
program Program_Manager_Eternity; { X Series }
{ 
	FYI, Just a little information on why it is "X Series" and not PGM 7.3 or 8.0. 
	
	Version 7.2 was the last fully functional version of the PGM series. Completed
	on 4/15/1992 (According to timestamps on original source files). I decided to release
	it under the MPL v2.0 on GitHub (https://github.com/shidel/DustyTP7/PGME). Even though
	a couple bug crept in when porting it to my QCrt unit from the unusable Borland Crt.
	That is a final version and will never be updated. 
	
	Version 8.0. Was a ground-up rewrite of 7.2. It was never completed. Because of...
	
	Version 8.0a. This should have been version 9. But it wasn't. It was another complete
	rewrite in graphics mode. It was only partially completed and abandoned shortly after 
	starting it. Because...
	
	Version 8.0b. Targeted Windows 3.11 which was also horrible at launching programs. I
	had very limited time at that time. Out came windows 95. Version 8.0b was dropped.
	
	Version 9.0. If Microsoft can just decide to skip a version number on Windows, I 
	figure I've earned that right too.
	
	X Series. Everybody Loves "X". "X Marks the Spot", "X Games", "Mac OS X"...
	
}

type
	TCallBackBuffer  = record
		S : boolean;
		C, P, R : String;
	end;

    PParamBlock = ^TParamBlock;
    TParamBlock = record
    	EnvSeg : word;
    	CmdLn : pointer;
    	FCB1, FCB2 : pointer;
    	Params : String;
    end;


var
	Index : integer;
	EXE : String;
	CALLS, PARMS : String;
	Buffer : TCallBackBuffer;
    ParamBlock : TParamBlock;

procedure CallBack; far; assembler;
asm		
		JMP  @@GO
	@@SIG:
		DB 8,'ETERNITY'
	@@DATA:
		DB 0
		DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
		DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
		DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
		DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
		DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
		DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
		DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
		DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
	@@DATA_END:
		DB 0
	@@GO:
		PUSH DI
		MOV  DI, OFFSET @@DATA
	@@LOOP:
		CMP  DI, OFFSET @@DATA_END
		JE   @@DONE
		MOV  AX, ES:[BX]
		MOV  CS:[DI], AX
		INC  BX
		INC  DI
		JMP  @@LOOP
	@@DONE:
		POP DI
end;

{ To Minimize PGM Launcher Size, Imported stuff from QStrings }
function IntStr ( Value : Integer ) : String;
var
	Temp : String;
begin
	Str(Value, Temp);
	IntStr := Temp;
end;

{ To Minimize PGM Launcher Size, Imported stuff from QDos }
const
	DosError : integer = 0;

procedure DoExec(AFunc : Byte; const Command, ParamBlock); assembler;
asm
  	{ Save Registers }
	PUSH SI
	PUSH DI
	PUSH ES
	PUSH DS

	{ Save SS:SP }
	MOV DI, OFFSET @@SaveRegs
	MOV CS:[DI], SP
	MOV CS:[DI+2], SS
	
	MOV AL, AFunc
	LES BX, DWORD PTR ParamBlock
	LDS DX, DWORD PTR Command
	INC DX

	MOV AH, $4B
	INT 21h
	JC @@Returned	
	MOV AX, BX
	JMP @@NoError		
@@SaveRegs:
	DD 0h
@@NoError:
    XOR  AX, AX
@@Returned:
	{ Restore SS:SP }
	MOV DI, OFFSET @@SaveRegs
	MOV SP, CS:[DI]
	MOV SS, CS:[DI+2]
	
	{ Restore Registers }
	POP  DS
	POP  ES
	POP  DI
	POP  SI
    MOV  DosError, AX
    XOR  AX, AX	
end;

procedure Exec(Command, Params : String);
begin
  	if Length(Command) > 254 then 
  		Command[Length(Command)] := #0
  	else
  		Command := Command + #0;  	
  	ParamBlock.EnvSeg := 0;
  	ParamBlock.CmdLn := @ParamBlock.Params;  	
  	ParamBlock.Params := Params + #$0d; 
  	DoExec ($00, Command, ParamBlock); { Load and Exec }
end;

{ Testing Constant to prevent infinite loop }
const
	BAT : boolean = false;

begin
	if ofs(CallBack) <> 0 then begin
		Writeln('Internal Error');
		Halt(1);
	end;
	{ Begin Get Start Params }
	CALLS := ' /CALL=' + IntStr(Seg(CallBack));
	PARMS := ' /INIT';
	for Index := 1 to ParamCount do begin
		if ParamStr(Index) = '/BAT' then BAT := True;
		PARMS := PARMS + ' ' + ParamStr(Index);
	end;
	EXE := ParamStr(0);
	INDEX := Length(EXE);
	while EXE[INDEX] <> '\' do Dec(Index);
	EXE := Copy(EXE, 1, INDEX) + 'ETERNITY.BIN';
	{ END Get Start Params }
	Write('Starting Program Manager Eternity...');
	repeat
		Buffer.S := False;
		Buffer.C := '';
		Buffer.R := '';
		Exec(EXE, CALLS + PARMS);
		Buffer := TCallBackBuffer(Ptr(Seg(CallBack), OFS(CallBack) + $0003 + $0009)^);	
		if (DosError = 0) then begin
			if (Buffer.C <> '') then
				Exec(Buffer.C, Buffer.P);
			PARMS := ' ' + Buffer.R;
			if BAT then
				PARMS := ' /BAT' + PARMS;
			if DosError <> 0 then
				PARMS := PARMS + ' /ERROR=' + IntStr(DosError);
		end else begin
			Halt(DosError);
		end;
	until (not Buffer.S);
end.
