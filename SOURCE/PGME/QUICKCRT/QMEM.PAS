{ Copyright 2024 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QMem; { QuickCRT Memory Unit }
{$I QCRT.DEF}

interface

{$DEFINE INTERFACE}	
	
const
    XMS_Present     : Boolean = false;
	MEM_Result      : Word = 0;

    function  XMS_Avail : LongInt;                      { returns largest free block }
    function  XMS_GetMem(Size : LongInt) : Word;        { returns 0xffff if failed }
    procedure XMS_FreeMem(Handle : Word);               { releases XMS memory}
    function  XMS_MemSize(Handle : Word) : LongInt;     { returns size of block }
    
    function  UMB_Avail : LongInt;                      { returns largest free UMB }
    function  UMB_GetMem(Size :LongInt) : word;         { returns segment, or 0xffff }
    procedure UMB_FreeMem(Segment : word);              { releases UMB block }
    function  UMB_MemSize(Segment : Word) : LongInt;    { returns size of block }

    { Generally, DOS memory is not much use. You could probably use the heap instead. }
    function  DOS_Avail : LongInt;                      { returns largest free lower }
    function  DOS_GetMem(Size :LongInt) : word;         { returns segment, or 0xffff }
    procedure DOS_FreeMem(Segment : word);              { releases lower block }
    function  DOS_MemSize(Segment : Word) : LongInt;    { returns size of block }
  
{$UNDEF INTERFACE}

implementation

var
	OldExitProc : Pointer;
		
const 
	XMS_Driver      : Pointer = nil;

{$DEFINE IMPLEMENTATION}

{ 

Excluded XMS functions of limited general program usage:

    00, Get XMS/XMM Version and HMA Exists
    01, HMA Alloc
    02, HMA Free
    03, Enable Global A20
    04, Disable Global A20
    05, Enable Local A20
    06, Disable Local A20
    07, Query A20
    08, (only total XMS available)
    0b, Move XMS Mlock
    0c, Lock XMS Handle
    0d, Unlock XMS Handle
    0e, (only Get XMS Lock state, Available Handles)
    0f, Resize XMS Handle
    12, Resize UMB Segment

Note:
    XMS memory is in 1 Kilobyte (1024 byte) blocks.     Request Maximum of 63   Mb
    UMB memory is in 1 Paragraph (16 byte) blocks.      Request Maximum of 1023 Mb
    EMS memory is in 1 Page (16384 byte, 16 Kb) blocks. Request Maximum of 1023 Mb
    
}

procedure XMS_Check; assembler;
asm
    mov         ax, $4300       { XMS install check }
    int         $2f
    cmp         al, $80
    jne         @@Done          { al <> 0x80 then no XMS driver is loaded }
    push        es
    mov         ax, $4310       { XMS Get Driver Entry Point as ES:BX }
    int         $2f
    mov         word [XMS_Driver], bx 
    mov         word [XMS_Driver + 2], es
    pop         es 
    mov         [XMS_Present], True
@@Done:    
end;

function XMS_Avail : LongInt; assembler;
asm
    xor         bl, bl         { not present, no error }
    cmp         [XMS_Present], False
    je          @@Error
    mov         ah, $08         { XMS Query Free Memory }
    { xor         bl, bl }      { Some drivers leave BL unchanged on success }
    call        [XMS_Driver]    
    { Up to 64MB, DX = Total Free, AX = Largest Block }
    test        bl, bl          { check for error }
    jz          @@Done
@@Error:
    xor         ax, ax
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx
    mov         dx, 1024
    mul         dx
end;

function XMS_GetMem(Size : LongInt) : Word; assembler;
asm
    cmp         [XMS_Present], False
    je          @@NoXMS
    les         di, Size
    mov         dx, es
    mov         ax, di
    cmp         dx, $03ff          
    jb          @@ValueConvert  { <= 1023 is ok }
    ja          @@ValueTooBig   { > 1023 is no good }
    cmp         ax, $fc00           
    jbe         @@ValueConvert  { = 1023 and <= 64512 is fine }
@@ValueTooBig:
    mov         bl, $08         { out of memory }
    jmp         @@Error
@@ValueConvert:
    mov         cx, $0400       
    div         cx              { divide dx:ax / 1024 }
    test        dx, dx
    jz          @@ValueReady
    inc         ax
@@ValueReady:

    mov         dx, ax          { Size in KB }
    mov         ah, $09         { Request XMS Memory }
    call        [XMS_Driver]
    test        ax, ax          { 0=Failure, 1=Success}
    jz          @@Error
    xor         bl, bl
    mov         ax, dx          { save handle }
    jmp         @@Done

@@NoXMS:
    mov         bl, $01
@@Error:
    mov         ax, $ffff       { -1 }   
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx
end;

procedure XMS_FreeMem(Handle : Word); assembler;
asm
    cmp         [XMS_Present], False
    je          @@NoXMS

    mov         dx, Handle
    mov         ah, $0a         { Free Memory }
    call        [XMS_Driver]
    test        ax, ax          { 0=Failure, 1=Success}
    jz          @@Error
    xor         bl, bl
    jmp         @@Done

@@NoXMS:
    mov         bl, $01
@@Error:
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx

end;

function XMS_MemSize(Handle : Word) : LongInt; assembler;
asm
    cmp         [XMS_Present], False
    je          @@NoXMS

    mov         dx, Handle
    mov         ah, $0e         { Get Size and Lock state }
    call        [XMS_Driver]
    test        ax, ax          { 0=Failure, 1=Success}
    jz          @@Error
    mov         ax, 1024
    mul         dx
    xor         bl, bl
    jmp         @@Done

@@NoXMS:
    mov         bl, $01
@@Error:
    xor         ax, ax
    xor         dx, dx
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx

end;

{ -------------------------------------------------------------------------- }

function  UMB_Avail : LongInt; assembler;
asm
    xor         bl, bl         { not present, no error }
    cmp         [XMS_Present], False
    je          @@Error
    mov         ah, $10         { Allocate UMB }
    mov         dx, $ffff       { In paragraphs, so 1023 kb (way to big) }
    call        [XMS_Driver]    
    mov         ax, dx
    cmp         bl, $b0         { check for too big error }
    jne         @@Error
    xor         bl, bl          { clear error }
    jmp         @@Done
@@Error:
    xor         ax, ax
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx
    mov         dx, 16
    mul         dx
end;

function  UMB_GetMem(Size : LongInt) : word; assembler;
asm
    cmp         [XMS_Present], False
    je          @@NoXMS
    les         di, Size
    mov         dx, es
    mov         ax, di
    cmp         dx, $000f          
    jb          @@ValueConvert  { <= 15 is ok }
    ja          @@ValueTooBig   { > 15 is no good }
    cmp         ax, $fff0           
    jbe         @@ValueConvert  { = 15 and <= 65520 is fine }
@@ValueTooBig:
    mov         bl, $08         { out of memory }
    jmp         @@Error
@@ValueConvert:
    mov         cx, $0010      
    div         cx              { divide dx:ax / 16 }
    test        dx, dx
    jz          @@ValueReady
    inc         ax
@@ValueReady:

    mov         dx, ax          { Size in KB }
    mov         ah, $10         { Request UMB Memory }
    call        [XMS_Driver]
    test        ax, ax          { 0=Failure, 1=Success}
    jz          @@Error
    mov         ax, bx          { save segment }
    xor         bl, bl
    jmp         @@Done

@@NoXMS:
    mov         bl, $01
@@Error:
    mov         ax, $ffff       { -1 }   
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx
end;

procedure UMB_FreeMem(Segment : word); assembler;
asm
    cmp         [XMS_Present], False
    je          @@NoXMS

    mov         dx, Segment
    mov         ah, $11         { Free Memory }
    call        [XMS_Driver]
    test        ax, ax          { 0=Failure, 1=Success}
    jz          @@Error
    xor         bl, bl
    jmp         @@Done

@@NoXMS:
    mov         bl, $01
@@Error:
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx

end;

function  UMB_MemSize(Segment : Word) : LongInt;    
begin
    UMB_MemSize:=DOS_MemSize(Segment);
end;

{ -------------------------------------------------------------------------- }

function  DOS_Avail : LongInt; assembler;
asm
    mov         ah, $48         { Allocate DOS }
    mov         bx, $ffff       { In paragraphs, so 1023 kb (way to big) }
    int         $21   
    xchg        bx, ax
    cmp         bl, $08         { check for too big error }
    jne         @@Error
    xor         bl, bl          { clear error }
    jmp         @@Done
@@Error:
    xor         ax, ax
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx
    mov         dx, 16
    mul         dx
end;

function  DOS_GetMem(Size : LongInt) : word; assembler;
asm
    les         di, Size
    mov         dx, es
    mov         ax, di
    cmp         dx, $000f          
    jb          @@ValueConvert  { <= 15 is ok }
    ja          @@ValueTooBig   { > 15 is no good }
    cmp         ax, $fff0           
    jbe         @@ValueConvert  { = 15 and <= 65520 is fine }
@@ValueTooBig:
    mov         bl, $08         { out of memory }
    jmp         @@Error
@@ValueConvert:
    mov         cx, $0010      
    div         cx              { divide dx:ax / 16 }
    test        dx, dx
    jz          @@ValueReady
    inc         ax
@@ValueReady:

    mov         bx, ax          { Size in KB }
    mov         ah, $48         { Request DOS Memory }
    int         $21
    jc          @@Error
    xor         bl, bl
    jmp         @@Done

@@Error:
    mov         bx, ax
    mov         ax, $ffff       { -1 }   
@@Done:
    xor         bh, bh
    mov         [MEM_Result], bx
end;

procedure DOS_FreeMem(Segment : word); assembler;
asm
    push        es
    mov         es, Segment
    mov         ah, $49         { Free Memory }
    int         $21
    jc          @@Error
    xor         bl, bl
    jmp         @@Done

@@Error:
    mov         bx, ax
@@Done:
    pop         es
    xor         bh, bh
    mov         [MEM_Result], bx
end;

function DOS_MemSize(Segment : Word) : LongInt; assembler;
asm
    push        es
    mov         ax, Segment
    dec         ax
    mov         es, ax
    mov         al, $4d
    cmp         [es:$0000], al
    je          @@MCBChain
    mov         al, $5d
    cmp         [es:$0000], al
    jne         @@Error
@@MCBChain:
    
    mov         ax, [es:$0003]
    mov         dx, $10
    mul         dx
    xor         bl, bl
    jmp         @@Done

@@Error:
    mov         bl, $09         { invalid MCB }
    xor         ax, ax
    xor         dx, dx
@@Done:
    pop         es
    xor         bh, bh
    mov         [MEM_Result], bx

end;

{ -------------------------------------------------------------------------- }

procedure ExitMemoryUnit; far;
begin
	ExitProc := OldExitProc;	
end;

procedure InitMemoryUnit;
begin
	OldExitProc := ExitProc;
	ExitProc := @ExitMemoryUnit;
	XMS_Check;
end;

{$UNDEF IMPLEMENTATION}
begin
	InitMemoryUnit;
end.
