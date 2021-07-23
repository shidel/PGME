{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Font_Convert_from_RAW_data; { X Series }

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'QuickCrt Raw to Font Converter';
	AppID = 'QRAW2FNT';
		
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
	FSize : LongInt;
	P : Pointer;
	
	procedure DoomsDay(Error : integer; Msg: String); 
	begin
		if Error = 0 then Exit;
		if Assigned(P) then FreeMem(P, Size);
		WriteLn('ERROR: ', Msg);
		Close(F);
		Halt(1);
	end;
	
	procedure WriteStr(S : String);
	begin
		BlockWrite(F, S, Length(S) + 1);
		DoomsDay(IOResult, 'Writing font file.');
	end;
	
begin
	P := nil;
	if ParamStr(1) = '' then 
		WriteLn('Please Specify a raw binary font file to convert.')
	else begin
		{$I-}
		WriteLn('Reading raw font file "', ParamStr(1), '".');
		Assign(F, ParamStr(1));
		Reset(F, 1);
		FSize := FileSize(F);
		Size := FSize;
		if FSize mod 256 <> 0 then 
			DoomsDay(5, 'Unable to determine raw font dimensions.');
		FSize := FSize div 256;
		if FSize < 1 then
			DoomsDay(5, 'Confused reguarding raw font dimensions. It seems to small.');
		if FSize > 32 then
			DoomsDay(5, 'Confused reguarding raw font dimensions. It seems to big.');

		W := 8;
		H := FSize;
		
		GetMem(P, Size);
		FillChar(P^, Size, 0);
		BlockRead(F, P^, Size);
		DoomsDay(IOResult, 'Reading raw font file.');
		Close(F);
		WriteLn;

		S := ParamStr(2);
		if S = '' then begin
			S := Copy(ParamStr(1), 1, Length(ParamStr(1)) - 3) + 'FNT';
			while Pos('\', S) > 0 do 
				Delete(S, 1, Pos('\', S));
		end;

		WriteLn('Creating QuickCrt font file "', S, '".');
		Assign(F, S);
		Rewrite(F, 1);
		Success := (IOResult = 0);
		if Not Success then begin
			WriteLn('Error creating font file.');
			FreeMem(P, Size);
			Halt(1);
		end;

		BlockWrite(F, FileID[1], Length(FileID));
		DoomsDay(IOResult, 'Writing Font Header.');
		WriteStr('TITLE=Converted Binary Font');
		WriteStr('LANGUAGE=EN_US');
		WriteStr('VERSION=1.00');
		WriteStr('COPYRIGHT=Copyright 2015');
		WriteStr('AUTHOR=Anonymous');
		WriteStr('URL=http://quickcrt.sourceforge.net'); 
		WriteStr('LICENSE=GNU GENERAL PUBLIC LICENSE, Version 2');
		WriteStr('LURL=http://www.gnu.org/licenses/gpl-2.0.txt');
		WriteStr('');
			
		BlockWrite(F, W, Sizeof(W));
		DoomsDay(IOResult, 'Writing Font Header');
		BlockWrite(F, H, Sizeof(H));
		DoomsDay(IOResult, 'Writing Font Header');
		
		WriteLn('Font dimensions: ', W, 'x', H);
		WriteLn('Font size: ', Size);
		BlockWrite(F, P^, Size);
		DoomsDay(IOResult, 'Writing Font Data');
		Close(F);

		FreeMem(P, Size);
	end;
	
end;

begin
	Execute; 
end.
