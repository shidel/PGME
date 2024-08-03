{ Copyright 2024 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I QCRT.DEF}
{$M 50000,0,1024} (* Memory: Largest Stack, No Minimum Free, Use All Ram *)
program QMemory_Memory_Test_Program; { X Series }

uses QMem;

const
    Alloc_Size : longint = 16384 + 1;

procedure DOS_Memory_Test;
var     
    Segment : word;
begin
    WriteLn('DOS Memory Tests');    
    { DOS Memory is always present. However, it may be full. }
	WriteLn('   Available ', DOS_Avail, ' (', MEM_Result, ')');   
    Segment := DOS_GetMem(Alloc_Size);
	WriteLn('   Allocate  ', Alloc_Size, ' (', MEM_Result, ')');   
	WriteLn('   Segment   ', Segment );
    if Segment <> $ffff then begin
        WriteLn('   Allocated ', DOS_MemSize(Segment), ' (', MEM_Result, ')');   
        WriteLn('   Available ', DOS_Avail, ' (', MEM_Result, ')');   	
        DOS_FreeMem(Segment);
        WriteLn('   Release   ', '(', MEM_Result, ')');   
        WriteLn('   Available ', DOS_Avail, ' (', MEM_Result, ')');   	
	end;
end;

procedure UMB_Memory_Test;
var     
    Segment : word;
begin
    WriteLn('UMB Memory Tests');
    if not UMB_Present then begin
        WriteLn('   not present');
        Exit;
    end;
	WriteLn('   Available ', UMB_Avail, ' (', MEM_Result, ')');   
    Segment := UMB_GetMem(Alloc_Size);
	WriteLn('   Allocate  ', Alloc_Size, ' (', MEM_Result, ')');   
	WriteLn('   Segment   ', Segment );
    if Segment <> $ffff then begin
        WriteLn('   Allocated ', UMB_MemSize(Segment), ' (', MEM_Result, ')');   
        WriteLn('   Available ', UMB_Avail, ' (', MEM_Result, ')');   	
        UMB_FreeMem(Segment);
        WriteLn('   Release   ', '(', MEM_Result, ')');   
        WriteLn('   Available ', UMB_Avail, ' (', MEM_Result, ')');   	
	end;
end;

procedure XMS_Memory_Test;
var     
    Handle  : word;
    Params  : TXMS_MoveParams;
    Buffer  : array[0..1024] of word;
    I       : word;
    Verify  : boolean;
begin
    WriteLn('XMS Memory Tests');
    if not XMS_Present then begin
        WriteLn('   not present');
        Exit;
    end;
	WriteLn('   Available ', XMS_Avail, ' (', MEM_Result, ')');   
    Handle := XMS_GetMem(Alloc_Size);
	WriteLn('   Allocate  ', Alloc_Size, ' (', MEM_Result, ')');   
	WriteLn('   Handle    ', Handle );
    if Handle <> $ffff then begin
        WriteLn('   Allocated ', XMS_MemSize(Handle), ' (', MEM_Result, ')');   
        WriteLn('   Available ', XMS_Avail, ' (', MEM_Result, ')'); 
        { Test XMS Move }
        for I := Low(Buffer) to High(Buffer) do Buffer[I] := I;
        Params.Count := Sizeof(Buffer);
        Params.SrcHandle := 0;
        Params.DstHandle := Handle;
        Params.SrcPtr    := @Buffer;
        Params.DstPtr    := Pointer(0);
        XMS_Move(Params);
        WriteLn('   Send      ', '(', MEM_Result, ')');   
        FillChar(Buffer, Sizeof(Buffer), 0);
        Params.SrcHandle := Handle;
        Params.DstHandle := 0;
        Params.SrcPtr    := Pointer(0);
        Params.DstPtr    := @Buffer;
        XMS_Move(Params);
        WriteLn('   Receive   ', '(', MEM_Result, ')'); 
        Verify := True;  
        for I := Low(Buffer) to High(Buffer) do 
            if Buffer[I] <> I then Verify := False;
        WriteLn('   Verify    ', Verify); 
        { Free Memory }  	
        XMS_FreeMem(Handle);
        WriteLn('   Release   ', '(', MEM_Result, ')');   
        WriteLn('   Available ', XMS_Avail, ' (', MEM_Result, ')');   	
	end;
end;

function Frame : word; assembler;
asm
    mov         ah, $41
    int         $67
    mov         ax, bx
end;

procedure EMS_Memory_Test;
const
    E_Size = 16384 * 8; { 128k, 8 pages }
type
    TWords  = array[0..8191] of word;
var     
    Handle  : word;
    I, J    : word;
    K       : Byte;
    Verify  : boolean;
    Success : boolean;
begin
    WriteLn('EMS Memory Tests');
    if not EMS_Present then begin
        WriteLn('   not present');
        Exit;
    end;
    
	WriteLn('   Available ', EMS_Avail, ' (', MEM_Result, ')');   
    Handle := EMS_GetMem(E_Size);
	WriteLn('   Allocate  ', E_Size, ' (', MEM_Result, ')');   
	WriteLn('   Handle    ', Handle );
    if Handle <> $ffff then begin
        WriteLn('   Allocated ', EMS_MemSize(Handle), ' (', MEM_Result, ')');   
        WriteLn('   Available ', EMS_Avail, ' (', MEM_Result, ')'); 
        { Test EMS Pages }
        for K := 0 to 0 do begin            
            Write( '        Physical Page ', K, ' as Logical ');
            for J := 0 to 7 do begin
                EMS_MapPage(Handle, K, J);
                Write(J, '(', MEM_Result, ') ');
                for I := Low(TWords) to High(TWords) do 
                    TWords(EMS_Page[K]^)[I] := I + J;   
            end;
            WriteLn;
        end;
        Success := True;
        for K := 0 to 3  do begin
           Write( '        Verify   Page ', K, ' is Logical ');
           for J := 0 to 7 do begin
                EMS_MapPage(Handle, K, J);            
                Verify := True;  
                for I := Low(TWords) to High(TWords) do 
                    if TWords(EMS_Page[K]^)[I] <> I + J then 
                        Verify := False;
                if Verify then 
                    Write(J, ' Y  ')
                else
                    Write(J, ' n  ');
                Success := Success and Verify;
            end;
            WriteLn;
        end;
        WriteLn('   Success   ', Success);   
        { Free Memory }  
        	
        EMS_FreeMem(Handle);
        WriteLn('   Release   ', '(', MEM_Result, ')');   
        WriteLn('   Available ', EMS_Avail, ' (', MEM_Result, ')');   	
	end;
end;


procedure All_Memory_Tests;
begin
    DOS_Memory_Test;
    UMB_Memory_Test;
    XMS_Memory_Test;	
    EMS_Memory_Test;
end;

begin	
	All_Memory_Tests;  
end.
