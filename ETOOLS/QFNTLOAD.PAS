{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Font_Loader; { X Series }

uses QBIOS, QStrings;

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'QuickCrt Font Loader';
	AppID = 'QLOADFNT';
	
procedure FixCursor; assembler;
asm
  MOV AH, $03
  MOV BH, 0
  INT $10
  PUSH CX 
  MOV  CX, $2000
  MOV  AH, $01
  INT  $10
  POP  CX
  INT  $10
end;

procedure ResetTextMode; assembler;
asm
  MOV AX, 0003h
  INT $10
end;		

procedure Execute;
const
	FileID : String = 'QCRT-FONT'#0;
var
	F : File;
	Success : boolean;
	S : String;
	W, H : byte;
	Size : word;
	P : Pointer;
	
	procedure DoomsDay(Error : integer; Msg: String); 
	begin
		if Error = 0 then Exit;
		if Assigned(P) then FreeMem(P, Size);
		WriteLn('ERROR: ', Msg);
		Close(F);
		Halt(1);
	end;
	
begin
	P := nil;
	if ParamStr(1) = '' then 
		WriteLn('Please Specify a QuickCrt font file to load or the reset command.')
	else
	if UCase(ParamStr(1)) = 'RESET' then 
		ResetTextMode
	else begin
		{$I-}
		Assign(F, ParamStr(1));
		Reset(F, 1);
		Success := (IOResult = 0);
		if Not Success then begin
			WriteLn('Error opening QCRT font file.');
			Halt(1);
		end;
		FillChar(S[1], Length(FileID), 0);
		S[0] := Char(Length(FileID));
		BlockRead(F, S[1], Length(S));
		DoomsDay(IOResult, 'Reading Font Header');
		if S <> FileID then DoomsDay(5, 'Invalid file format.');
		while S <> '' do begin
			BlockRead(F, S[0], 1);
			DoomsDay(IOResult, 'Reading font file.');
			BlockRead(F, S[1], Length(S));
			DoomsDay(IOResult, 'Reading font file.');
			{if S <> '' then WriteLn(S); }
		end;
		BlockRead(F, W, Sizeof(W));
		DoomsDay(IOResult, 'Reading Font Header');
		BlockRead(F, H, Sizeof(H));
		DoomsDay(IOResult, 'Reading Font Header');
		if W <> 8 then DoomsDay(5, 'Invalid font width');
		if (H < 1) or (H > 32) then DoomsDay(5, 'Invalid font height');
		WriteLn('Font dimensions: ', W, 'x', H);
		Size := Word(H) * 256;
		WriteLn('Font size: ', Size);
		GetMem(P, Size);
		BlockRead(F, P^, Size);
		DoomsDay(IOResult, 'Reading Font Data');
		Close(F);
		
		SetUserFont(H, P); 
		FixCursor;

		FreeMem(P, Size);
	end;
	
end;
		 
begin
	Execute; 
end.
