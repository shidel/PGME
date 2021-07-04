{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QBIOS; { QuickCrt BIOS Tools }
{$I QCRT.DEF}

interface

	const
	{ Change-Line Support Constants }
		clNotPresent	 = 0; { No Drive Present }
		clNotSupported   = 1; { Floppy without Change-Line Support }
		clSupported		 = 2; { Floppy with Change-Line Support }
		clFixed			 = 3; { Fixed Disk }
		
	type
	{ Get ROM Bios Font Pointer Constants as a set }
		TROMFont = ( rf8x14, rf8x8, rfHigh8x8, rfAlt9x14, rf8x16, rfAlt8x16 );
		{
			0 = EGA 8x14
			1 = 8x8 (Part 1), 2 = 8x8 (Part 2)
			3 = 9x14 alternate
			4 = VGA 8x16
			5 = VGA 8x16 alternate
			Pointer to Specified ROM Font
		}
		
	type
		TFloppyDescriptor = record
			Cylinder, Head, Sector, Size : byte;
		end;
		TFloppyFormatBuf = array[1..128] of TFloppyDescriptor;

	const
		BIOSError : integer = 0;

	function GetBiosDriveID(Drive:Byte) : byte;
	function Diskettes : byte;																			{ ROM-BIOS	}
	function ResetDrive(Drive : Byte) : boolean;
	function ReadSectors(Drive : Byte; Head, Track, Sector, Count : word; var Buf) : boolean;
	function WriteSectors(Drive : Byte; Head, Track, Sector, Count : word; var Buf) : boolean;
	function VerifySectors(Drive : Byte; Head, Track, Sector, Count : word; var Buf) : boolean;
	function FormatTrack(Drive : Byte; Head, Track, Interleave : word; var Buf) : boolean;
	procedure FillFloppyFormatBuf(Head, Track, Size : word; var Buf : TFloppyFormatBuf);
	procedure GetDriveParams(Drive : Byte; var Heads, Tracks, Sectors : word);
	function ChangeLineSupport(Drive : Byte) : byte;
	procedure SetFloppyFormatDASD(Drive : byte; DiskType : byte);
	function SetFloppyFormatMedia(Drive : byte; Tracks, Sectors : byte) : Pointer;
	
	procedure SetUserFont(Height : byte; FontPtr : Pointer); 
	function GetROMFontPtr(Mode : TROMFont) : Pointer; 

type
	TRGB = record
		R, G, B : byte;
	end;
	TDAC = array[0..255] of TRGB;
	TPalette = record
		Palette : array[0..15] of byte;
		Overscan : Byte;
	end;
	
	procedure ReadDAC( First, Count : word; var Buffer );
	procedure WriteDAC( First, Count : word; var Buffer );
	procedure FadeOutDAC( First, Count : word; var Buffer ); 
	procedure FadeInDAC( First, Count : word; var Buffer ); 
	procedure GetPalette(var Buffer);
	procedure SetPalette(var Buffer);
	
implementation


function GetBiosDriveID(Drive:Byte) : byte; assembler;
asm
	MOV	 AL, Drive
	CMP	 AL, 0
	JNE	 @ComputeIt
	MOV	 AH, 19h
	INT	 21h
	INC	 AL
@ComputeIt:
	DEC	 AL
	CMP	 AL, 1
	JG	 @NotFloppy
	JMP	 @Done
@NotFloppy:
	ADD	 AL, $7E
@Done:
	MOV	 BIOSError, 0
end;

function Diskettes : byte; assembler;
asm
	INT	 11h
	TEST AL, 01h
	JZ	 @@NoDrives
	XOR	 AH, AH
	AND	 AL, 0C0h
	MOV	 CL, 06h
	SHR	 AX, CL
	INC	 AX
	JMP	 @@Done
@@NoDrives:
	XOR	 AX, AX
@@Done:
	MOV	 BIOSError, 0
end;

function SetBIOSError(Drive : byte) : boolean; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBIOSDriveID
	MOV	 DL, AL
	MOV	 AH, 01h
	INT	 13h
	XOR	 AH, AH
	MOV	 BIOSError, AX
	CMP	 AX, 0
	JE	 @NoError
	MOV	 AL, FALSE
	JMP	 @Done
@NoError:
	MOV	 AL, True
@Done:
end;

function ResetDrive(Drive : Byte) : boolean; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBIOSDriveID
	MOV	 DL, AL
	MOV	 AH, 00h
	INT	 13h
	MOV	 AL, Drive
	PUSH AX
	CALL SetBIOSError
end;

function ReadSectors(Drive : Byte; Head, Track, Sector, Count : word; var Buf) : boolean; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBIOSDriveID
	MOV	 DX, Head
	MOV	 DH, DL
	MOV	 DL, AL
	MOV	 CX, Sector
	MOV	 AX, Track
	MOV	 CH, AL
	MOV	 AX, Count
	MOV	 AH, 02h
	LES	 BX, Buf
	INT	 13h
	JNC	 @NoError
	MOV	 AL, AH
	XOR	 AH, AH
	MOV	 BIOSError, AX
	MOV	 AL, False
	JMP	 @Done
@NoError:
	XOR	 AL, True
	MOV	 BIOSError, 0
@Done:
end;

function WriteSectors(Drive : Byte; Head, Track, Sector, Count : word; var Buf) : boolean; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBIOSDriveID
	MOV	 DX, Head
	MOV	 DH, DL
	MOV	 DL, AL
	MOV	 CX, Sector
	MOV	 AX, Track
	MOV	 CH, AL
	MOV	 AX, Count
	MOV	 AH, 03h
	LES	 BX, Buf
	INT	 13h
	JNC	 @NoError
	MOV	 AL, AH
	XOR	 AH, AH
	MOV	 BIOSError, AX
	MOV	 AL, False
	JMP	 @Done
@NoError:
	XOR	 AL, True
	MOV	 BIOSError, 0
@Done:
end;

function VerifySectors(Drive : Byte; Head, Track, Sector, Count : word; var Buf) : boolean; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBIOSDriveID
	MOV	 DX, Head
	MOV	 DH, DL
	MOV	 DL, AL
	MOV	 CX, Sector
	MOV	 AX, Track
	MOV	 CH, AL
	MOV	 AX, Count
	MOV	 AH, 04h
	LES	 BX, Buf
	INT	 13h
	JNC	 @NoError
	MOV	 AL, AH
	XOR	 AH, AH
	MOV	 BIOSError, AX
	MOV	 AL, False
	JMP	 @Done
@NoError:
	XOR	 AL, True
	MOV	 BIOSError, 0
@Done:
end;

function FormatTrack(Drive : Byte; Head, Track, Interleave : word; var Buf) : boolean; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBIOSDriveID
	MOV	 DX, Head
	MOV	 DH, DL
	MOV	 DL, AL
	MOV	 CX, 0 { Sector }
	MOV	 AX, Track
	MOV	 CH, AL
	MOV	 AX, Interleave
	MOV	 AH, 05h
	LES	 BX, Buf
	INT	 13h
	JNC	 @NoError
	MOV	 AL, AH
	XOR	 AH, AH
	MOV	 BIOSError, AX
	MOV	 AL, False
	JMP	 @Done
@NoError:
	XOR	 AL, True
	MOV	 BIOSError, 0
@Done:
end;

procedure FillFloppyFormatBuf(Head, Track, Size : word; var Buf : TFloppyFormatBuf);
var
	I : word;
begin
	for I := 1 to 128 do
	begin
		Buf[I].Head := Head;
		Buf[I].Cylinder := Track;
		Buf[I].Sector := I;
		case Size of
		128	 : Buf[I].Size := 0;
		256	 : Buf[I].Size := 1;
		512	 : Buf[I].Size := 2;
		1024 : Buf[I].Size := 3;
		else
		Buf[I].Size := 2;
		end;
	end;
end;

procedure GetDriveParams(Drive : Byte; var Heads, Tracks, Sectors : word); assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBIOSDriveID
	MOV	 DL, AL
	MOV	 AH, 08h
	INT	 13h
	XOR	 AX, AX
	MOV	 AL, DH
	LES	 DI, Heads
	MOV	 ES:[DI], AX
	MOV	 AL, CH
	LES	 DI, Tracks
	MOV	 ES:[DI], AX
	MOV	 AL, CL
	LES	 DI, Sectors
	MOV	 ES:[DI], AX
	MOV	 BIOSError, 0
end;

function ChangeLineSupport(Drive : Byte) : byte; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBIOSDriveID
	MOV	 DL, AL
	MOV	 AH, 15h
	INT	 13h
	JNC	 @NoError
@Error:
	MOV	 AL, AH
	XOR	 AH, AH
	MOV	 BIOSError, AX
	MOV	 AL, 0
	JMP	 @Done
@NoError:
	CMP	 AH, 3
	JG	 @Error
	MOV	 BIOSError, 0
	MOV	 AL, AH
@Done:
end;

function ChangeLine(Drive : Byte) : byte; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBiosDriveID
	MOV	 DL, AL
	MOV	 AH, 16h
	INT	 13h
	MOV	 AL, False
	JC	 @Done
@NoError:
	CMP	 AH, 6
	JNE	 @Done
	CMP	 DL, Drive
	JNE	 @Done
	MOV	 AL, True
@Done:
	MOV	 BIOSError, 0
end;

procedure SetFloppyFormatDASD(Drive : byte; DiskType : byte); assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBiosDriveID
	MOV	 DL, AL
	MOV	 AH, 17h
	MOV	 AL, DiskType
	INT	 13h
	JC	 @Error
	MOV	 AH, 0
@Error:
	MOV	 AL, AH
	XOR	 AH, AH
	MOV	 BIOSError, AX
end;

function SetFloppyFormatMedia(Drive : byte; Tracks, Sectors : byte) : pointer; assembler;
asm
	MOV	 AL, Drive
	PUSH AX
	CALL GetBiosDriveID
	MOV	 DL, AL
	MOV	 AH, 17h
	MOV	 CH, Tracks
	MOV	 CL, Sectors
	INT	 13h
	JC	 @Error
	MOV	 AH, 0
	JMP	 @Done
@Error:
	XOR	 DI, DI
	MOV	 ES, DI
@Done:
	MOV	 AL, AH
	XOR	 AH, AH
	MOV	 BIOSError, AX
	MOV	 AX, DI
	MOV	 DX, ES
end;


procedure SetUserFont(Height : byte; FontPtr : Pointer); assembler;
asm
	PUSH BP
	MOV AX, $0003
	INT $10
	MOV AX, $1110
	MOV CX, $0100
	MOV DX, $0000
	MOV BL, $00
	MOV BH, Height
	LES BP, DWORD PTR FontPtr
	INT $10
	POP BP
end;

procedure ReadDAC( First, Count : word; var Buffer ); assembler;
asm
	MOV AX, 1017h
	MOV BX, First
	MOV CX, Count
	LES DX, Buffer
	INT 10h
end;

procedure WriteDAC( First, Count : word; var Buffer ); assembler;
asm
	MOV AX, 1012h
	MOV BX, First
	MOV CX, Count
	LES DX, Buffer
	INT 10h
end;
	
procedure FadeOutDAC( First, Count : word; var Buffer ); 
var
	Temp : TDAC;
	I, J : integer;
	LastTimer : LongInt;
	TimerTick : LongInt absolute $0040:$006c;
begin
	LastTimer := TimerTick;
	While LastTimer = TimerTick do;
	for I := 1 to 8 do begin
		for J := First to First + Count - 1 do begin
			Temp[J].R := TDAC(Buffer)[J].R shr I;
			Temp[J].G := TDAC(Buffer)[J].G shr I;
			Temp[J].B := TDAC(Buffer)[J].B shr I;
		end;
		WriteDAC(First, Count, Temp);
		LastTimer := TimerTick;
		While LastTimer = TimerTick do;
	end;
end;

procedure FadeInDAC( First, Count : word; var Buffer ); 
var
	Temp : TDAC;
	I, J : integer;
	LastTimer : LongInt;
	TimerTick : LongInt absolute $0040:$006c;
begin
	LastTimer := TimerTick;
	While LastTimer = TimerTick do;
	for I := 1 to 8 do begin
		for J := First to First + Count - 1 do begin
			Temp[J].R := TDAC(Buffer)[J].R shr (8 - I);
			Temp[J].G := TDAC(Buffer)[J].G shr (8 - I);
			Temp[J].B := TDAC(Buffer)[J].B shr (8 - I);
		end;
		WriteDAC(First, Count, Temp);
		LastTimer := TimerTick;
		While LastTimer = TimerTick do;
	end;
end;

function GetROMFontPtr(Mode : TROMFont) : Pointer; assembler;
asm
	MOV  BH, Mode
	ADD  BH, 02h
	MOV  AX, 1130h
	PUSH ES
	PUSH BP
	INT  10h
	{ CX Current Font Height }
	{ DL Screen Rows }
	MOV  AX, BP
	MOV	 DX, ES
	POP  BP
	POP  ES
{	LES  BX, DWORD PTR A
	MOV  ES:[BX], CX
	MOV  ES:[BX + 2], DX }
end;

procedure SetPalette(var Buffer); assembler;
asm
	XOR BX, BX
	MOV AX, 1002h
	LES DX, Buffer
	INT 10h
end;

procedure GetPalette(var Buffer); assembler;
asm
	LES  DX, Buffer
	MOV	 CX, 17
	MOV  BX, DX
	MOV  AL, 0FFh
@@Clear:
	MOV	  ES:[BX], AL
	INC   BX
	LOOP  @@Clear
	MOV  AX, 1009h
	INT 10h
end;

end.
