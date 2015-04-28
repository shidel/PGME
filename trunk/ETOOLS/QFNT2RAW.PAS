{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Font_Convert_To_Raw_Binary; { X Series }

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'QuickCrt Font To Raw Converter';
	AppID = 'QFNT2RAW';
		
procedure Execute;
const
	FileID : String = 'QCRT-FONT'#0;
var
	F : File;
	Success : boolean;
	S : String;
	W, H : byte;
	I : integer;
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
		WriteLn('Please Specify a QuickCrt font file to convert.')
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
			if S <> '' then WriteLn(S); 
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

		S := ParamStr(2);
		if S = '' then
			S := Copy(ParamStr(1), 1, Length(ParamStr(1)) - 3) + 'BIN';
		
		WriteLn;
		WriteLn('Creating raw binary font file "', S, '".');
		Assign(F, S);
		Rewrite(F, 1);
		Success := (IOResult = 0);
		if Not Success then begin
			WriteLn('Error creating raw binary font file.');
			Halt(1);
		end;
		BlockWrite(F, P^, Size);
		DoomsDay(IOResult, 'Writing to raw binary font file.');
		
		Close(F);
		FreeMem(P, Size);
	end;
	
end;
		 
begin
	Execute; 
end.
