{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Font_To_Terminate_And_Stay_Resident; { X Series }

{
 This is based on my tsr to load a specific large custom font. It is a very quick and
dirty version. I'll probably make one later that will generate smaller COM files.
However, for now this works and the TSR won't use anymore memory than the better
version. It's just a bigger com file do to the padding. }

{$I ..\ETERNITY\LICENSE.INC}

{$DEFINE FONT24}

const
	AppTitle = 'QuickCrt Font TSR Creator';
	AppID = 'QFNT2TSR';
	
const
{$IFNDEF FONT32}
	{$IFDEF FONT24}
		MaxHeight = 24;
	{$ELSE}
		MaxHeight = 16;
	{$ENDIF}
{$ELSE}
	MaxHeight = 32;
	{$DEFINE FONT24}
{$ENDIF}
	
procedure AssemblyCode; assembler;
asm

@PSP:
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;

{ EGA Data Area }
@EGA_Video:       			{ DW $0000, $0000 }
	JMP @Install
	NOP
@EGA_Dynamic:     			DW $0000, $0000
@EGA_Text:        			DW $0000, $0000
@EGA_Graphics:    			DW $0000, $0000
@VGA_Table:       			DW $0000, $0000
@EGA_Reserved:    			DW $0000, $0000,  $0000, $0000

{ VGA Data Area }
@VGA_TableSize:   			DW  $0000       { Must be $001A Bytes Long }
@VGA_DCC:         			DW  $0000,$0000
@VGA_Text:        			DW  $0000,$0000
@VGA_Palette:     			DW  $0000,$0000
@VGA_Reserved:    			DW  $0000,$0000,  $0000,$0000,  $0000,$0000

@AuxiliaryFontTable:
{ Bytes per Character }		DB $10 	 { Re-Written at build time }
{ RAM Block to Load }		DB $00
{ Reserved Byte }			DB $00
@FontPointer:            	DW $0000, $0000
{ Modes }                	DB $03,$FF

@Reserved:					DB 0

@AuxiliaryFont: { 8K Max font size of space 32 bit high font, really? }
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
{8}
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
{16}
{$IFDEF FONT24}
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
{24}
{$ENDIF}
{$IFDEF FONT32}
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DD 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
{32}
{$ENDIF}

@Install:
	
@Dulicate:
	{ Duplicate SAVE_PTR Table }
	{ Load Address of EGA_SavePtr }
	MOV  AX, $0000
	MOV  ES, AX
	{ Load EGA Save pointer into DX:CX }
	MOV  CX, ES:[$04a8]
	MOV  DX, ES:[$04a8 + 2]
	{ Move Save Pointer to ES:SI }
	MOV  SI, CX
	MOV  ES, DX
	MOV  CX, 12
	MOV  DI, OFFSET @EGA_Video
	{ Copy EGA Save Pointer Table }
@Duplicate2:
	MOV  AX, ES:[SI]
	MOV  CS:[DI], AX
	ADD  SI, 2
	ADD  DI, 2
	LOOP @Duplicate2
{ Test for VGA }
@Test1:
	MOV  CX, CS:[OFFSET @VGA_Table]
	MOV  DX, CS:[OFFSET @VGA_Table + 2]
	CMP  CX, $0000
	JNE  @Test2
	CMP  DX, $0000
	JNE  @Test2
	MOV  DI, OFFSET @NoVga
	CALL @WriteString
	JMP @Exit
{ Test Table Size }
@Test2:
	MOV  ES, DX
	MOV  SI, CX
	MOV  CX, ES:[SI]
	CMP  CX, $001A
	JE   @Duplicate3
	MOV  DI, OFFSET @InvalidVGA
	CALL @WriteString
	JMP  @Exit
{ Duplicate VGA Table }
@Duplicate3:
	MOV  DI, OFFSET @VGA_TableSize
@Duplicate4:
	MOV  AL, ES:[SI]
	MOV  CS:[DI], AL
	INC  SI
	INC  DI
	LOOP @Duplicate4
@UpdatePointers:
	{ Load CS into DX }
	PUSH CS
	POP  DX
	{ Set Aux font pointer }
	MOV  DI, OFFSET @FontPointer
	MOV  CX, OFFSET @AuxiliaryFont
	MOV  CS:[DI], CX     { OFFSET }
	MOV  CS:[DI + 2], DX { Segment }
	{ Set Aux font table pointer }
	MOV  DI, OFFSET @VGA_Text
	MOV  CX, OFFSET @AuxiliaryFontTable
	MOV  CS:[DI], CX     { OFFSET }
	MOV  CS:[DI + 2], DX { Segment }
	{ Set VGA Table Pointer }
	MOV  DI, OFFSET @VGA_Table
	MOV  CX, OFFSET @VGA_TableSize
	MOV  CS:[DI], CX     { OFFSET }
	MOV  CS:[DI + 2], DX { Segment }
	{ Set EGA Save Pointer }
	MOV  AX, $0000
	MOV  ES, AX
	MOV  DI, $04A8
	MOV  CX, OFFSET @EGA_Video
	MOV  ES:[DI], CX     { OFFSET }
	MOV  ES:[DI + 2], DX { Segment }

@ResetMode:
	MOV  AX, $0000
	MOV  ES, AX
	MOV  AL, ES:[$0449]
	MOV  AH, $0
	INT  $10
	MOV  DI, OFFSET @Installed
	CALL @WriteString
	{ Dislpay Additional text }
	MOV  AX, $0100
	XOR  CH, CH
	MOV  CL, [OFFSET @AuxiliaryFontTable]
	MUL  CX
	ADD  AX, OFFSET @AuxiliaryFont
	PUSH AX { Save the offset for keep function }
	MOV  DI, AX
	CALL @WriteString
	POP  DX { set the offset for keep function }
	JMP  @Keep  

@Exit:
	MOV AX, $4C00
	INT $21

{ DS:DI := points to Null Terminated String }
@WriteString:
	PUSH AX
	PUSH DX
	PUSH DI
@WriteString2:
	CMP  DI, OFFSET @Install
	JNE  @WriteString3
	MOV  DI, OFFSET @MoreText
@WriteString3:
	MOV  DL, [DI]
	INC  DI
	CMP  DL, 0
	JE   @WriteString4
	MOV  AH, $02
	INT  $21
	JMP  @WriteString2
@WriteString4:
	POP  DI
	POP  DX
	POP  AX
	RET

{ Writes the decimal equivilant of a word AX }
@WriteDec:
	PUSHF
	PUSH AX
	PUSH DX
	PUSH CX
	MOV  DX, $00FF { Terminator }
	MOV  CX, $000A { Divisor }
	PUSH DX
@WriteDec2:
	CMP  AX, CX
	JNB  @WriteDec4
	CMP  DX, $00FF
	JE   @WriteDec3
	CMP  AX, 0
	JE   @WriteDec5
@WriteDec3:
	PUSH AX
	JMP  @WriteDec5
@WriteDec4:
	XOR  DX, DX
	DIV  CX
	PUSH DX
	JMP  @WriteDec2
@WriteDec5:
	POP  DX
	CMP  DX, $00FF
	JE   @WriteDec6
	MOV  AH, $02
	ADD  DL, $30
	INT  $21
	JMP  @WriteDec5
@WriteDec6:
	POP  CX
	POP  DX
	POP  DX
	POPF
	RET

@FreeENVSeg:
	PUSH ES
	PUSH AX
	MOV  ES, [$002C]
	MOV  AX, $4900
	INT  $21
	POP  AX
	POP  ES
	RET
	
@Keep:
	PUSH DX
	CALL @FreeENVSeg
	POP  DX
{	MOV DX, Address of end of table }
	MOV CX, DX
	AND CX, $0F
{$IFOPT	G+}
	SHR DX, 4
{$ELSE}
	SHR DX, 1
	SHR DX, 1
	SHR DX, 1
	SHR DX, 1 
{$ENDIF}
	CMP CX, 0
	JE  @Keep2
	INC DX
@Keep2:
	MOV AX, $3100
	INT $21

@CRLF:
	DB 13,10,0
@NOVGA:
	DB 'Error: VGA card not found.',13,10,0
@InvalidVGA:
	DB 'Error: Unknown VGA table format.',13,10,0
@Installed:
	DB 'Custom QuickCRT text font is now installed.',13,10,0
@OppsSize:
	DB 'Opps, my internal font storage area is to small.',13,10
	DB 'Sorry, I must abort.',13,10,0

@MoreText: { Added at Creation if Needed }

end;

procedure FontData; assembler;
asm
end;

procedure AssemblyCodeEnd; assembler;
asm
end;


procedure WriteCOM(var F : File);
var
	PAsm, PEnd : Pointer;
	Size : Word;
begin
	PAsm := @AssemblyCode;	  { get the start of the asm code }
	PEnd := @AssemblyCodeEnd; { Get then end of the asm code }

	if Ofs(PAsm^) <> 0 then begin
		WriteLn ( 'PSP segment is not alligned on a segment, cannot create QFNT TSR.');
		RunError ( 204 );
	end;

	PAsm := Ptr(Seg(PAsm^), Ofs(PAsm^) + $0100); { Discard program prefix segment }
	PEnd := Ptr(Seg(PEnd^), Ofs(PEnd^) - 1); { Discard procedures built-in return }

	Size := Ofs(PEnd^) - Ofs(PAsm^);
	BlockWrite(F, PAsm^, Size );
end;

type
	PStrings = ^TStrings;
	TStrings = record
		Next : PStrings;
		Str  : String;
	end;

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
	M, D, T : PStrings;
	SkipAt : LongInt;
	
	procedure DoomsDay(Error : integer; Msg: String); 
	var
		T : PStrings;
	begin
		if Error = 0 then Exit;
		if Assigned(P) then FreeMem(P, Size);
		While Assigned(M) do begin
			T := M^.Next;
			Dispose(M);
			M := T;
		end;
		WriteLn('ERROR: ', Msg);
		Close(F);
		Halt(1);
	end;
	
	function IntStr( I : integer ) : String;
	var
		S : String;
	begin
		Str(I, S);
		IntStr := S;
	end;
	
	procedure WriteStr(S : String);
	var
		I : integer;
	begin
		I := 0;
		while I < Length(S) do begin
			Inc(I);
			if FilePos(F) = SkipAt then
				Seek(F, FileSize(F));
			DoomsDay(IOResult, 'Writing to TSR file.');
			BlockWrite(F, S[I], 1);
			DoomsDay(IOResult, 'Writing to TSR file.');			
		end;
	end;
	
begin
	P := nil;
	M := nil;
	D := nil;
	if ParamStr(1) = '' then 
		WriteLn('Please Specify a QuickCrt font file use for making a TSR.')
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
			T := New(PStrings);
			T^.Next := nil;
			T^.Str := S;
			if Assigned(M) then
				D^.Next := T
			else 
				M := T;
			D := T;			
		end;
		BlockRead(F, W, Sizeof(W));
		DoomsDay(IOResult, 'Reading Font Header');
		BlockRead(F, H, Sizeof(H));
		DoomsDay(IOResult, 'Reading Font Header');
		if W <> 8 then DoomsDay(5, 'Invalid font width');
		if (H < 1) or (H > MaxHeight) then 
			DoomsDay(5, 'Invalid font height of ' + IntStr(H) + 
			'. Only up to ' + IntStr(MaxHeight) + ' is supported.');
		WriteLn('Font dimensions: ', W, 'x', H);
		Size := Word(H) * 256;
		WriteLn('Font size: ', Size);
		GetMem(P, Size);
		BlockRead(F, P^, Size);
		DoomsDay(IOResult, 'Reading Font Data');
		Close(F);

		S := ParamStr(2);
		if S = '' then begin
			S := Copy(ParamStr(1), 1, Length(ParamStr(1)) - 3) + 'COM';
			while Pos('\', S) > 0 do 
				Delete(S, 1, Pos('\', S));
		end;
		
		WriteLn;
		WriteLn('Creating TSR for QCRT font file "', S, '".');
		Assign(F, S);
		Rewrite(F, 1);
		WriteCOM(F);
		Success := (IOResult = 0);
		if Not Success then begin
			WriteLn('Error creating TSR file.');
			Halt(1);
		end;
		
		{ Write Height Byte into Font Table Data }
		Seek(F, 54);
		DoomsDay(IOResult, 'Writing to TSR file.');
		BlockWrite(F, H, Sizeof(H));
		DoomsDay(IOResult, 'Writing to TSR file.');
		
		{ Insert Font into Aux Font Area }
		Seek(F, 64);
		DoomsDay(IOResult, 'Writing to TSR file.');
		BlockWrite(F, P^, Size);
		DoomsDay(IOResult, 'Writing to TSR file.');
		
		{ Write Strings }
		SkipAt := 64 + MaxHeight * 256;
		T := M;
		WriteStr(#13#10);
		while Assigned(T) do begin
			if T^.Str <> '' then 
				WriteStr(T^.Str + #13#10);
			T := T^.Next;	
		end;
		WriteStr(#0);
		
		Close(F);
		
		WriteLn('Please note that TSR font will not function when using DOSBox.');
		if H <> 16 then begin
			WriteLn('');
			WriteLn('Also, this is a 8x', H, ' font. Nearly all VGA cards will only support a 8x16');
			WriteLn('font as a system replacement for the default VGA text mode font.');
			WriteLn('');
		end;

		While Assigned(M) do begin
			T := M^.Next;
			Dispose(M);
			M := T;
		end;

		FreeMem(P, Size);
	end;
	
end;
		 
var
	P : Pointer;
begin
	Execute; 
end.
