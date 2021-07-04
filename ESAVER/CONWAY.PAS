{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ESAVER.DEF}
program Conways_Game_of_Life;

  uses QCrt, QScrnSvr;

{$I ..\ETERNITY\LICENSE.INC}

(*
    Rules:
    Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
    Any live cell with two or three live neighbours lives on to the next generation.
    Any live cell with more than three live neighbours dies, as if by overpopulation.
    Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
*)
  const
    Speed = 1;
    CellUnder = 2;
    CellBirth = 3;
    CellOver  = 3;

  var
    Ticks : longint absolute $0040:$006c;

  const
    Last  : LongInt = 0;
    Count : Word = 0;

  type
    ScreenArray = array[0..199, 0..319] of byte;

  var
    Colors : array[0..255,0..2] of byte;
    WriteScreen, EditScreen : ^ScreenArray;
    ForeColor,
    BackColor   : byte;


  procedure Generation;
    var
        WSeg, WOfs, ESeg, EOfs : word;
  begin
    WSeg := Seg(WriteScreen^);
    WOfs := Ofs(WriteScreen^);
    ESeg := Seg(EditScreen^);
    EOfs := Ofs(EditScreen^);
    asm
        push es
        push ds
        mov  bh, CellBirth
        mov  dl, CellUnder
        mov  dh, CellOver
        mov  ax, WOfs
        sub  ax, 201
        push ax
        mov  ax, WSeg
        push ax
        mov  ax, EOfs
        push ax
        mov  ax, ESeg
        push ax
        pop  es
        pop  di
        pop  ds
        pop  si
        mov  cx, 200
    @@Lines:
        push cx
        mov  cx, 320
    @@Line:
        xor  ah, ah
        mov  al, [ds:si]
        cmp  al, 0
        je   @@EmptyA
        inc  ah
    @@EmptyA:
        mov  al, [ds:si + 1]
        cmp  al, 0
        je   @@EmptyB
        inc  ah
    @@EmptyB:
        mov  al, [ds:si + 2]
        cmp  al, 0
        je   @@EmptyC
        inc  ah
    @@EmptyC:
        mov  al, [ds:si + 200]
        cmp  al, 0
        je   @@EmptyD
        inc  ah
    @@EmptyD:
        mov  al, [ds:si + 202]
        cmp  al, 0
        je   @@EmptyE
        inc  ah
    @@EmptyE:
        mov  al, [ds:si + 400]
        cmp  al, 0
        je   @@EmptyF
        inc  ah
    @@EmptyF:
        mov  al, [ds:si + 401]
        cmp  al, 0
        je   @@EmptyG
        inc  ah
    @@EmptyG:
        mov  al, [ds:si + 402]
        cmp  al, 0
        je   @@EmptyH
        inc  ah
    @@EmptyH:
        mov  al, [ds:si + 201]
        cmp  al, 0
        je   @@IsDead
        cmp  ah, dl
        jb   @@Die
        cmp  ah, bh
        ja   @@Die
        jmp  @@Live
    @@IsDead:
        cmp  ah, bh
        je   @@Live
        jmp  @@Die
    @@Live:
        mov  bl, 255
        jmp  @@Update
    @@Die:
        xor  bl, bl
    @@Update:
        mov  [es:di], bl
        inc  di
        inc  si
        loop @@Line
        pop  cx
        loop @@Lines
        pop  ds
        pop  es
    end;

  end;

  procedure SetColors;
    var
      I      : byte;
    begin
      FillChar ( Colors, Sizeof(Colors), 0);
      for I := 0 to 63 do Colors[      I, 0] := I;
      for I := 0 to 63 do Colors[ 64 + I, 1] := I;
      for I := 0 to 63 do Colors[128 + I, 2] := I;
      for I := 0 to 63 do FillChar(Colors[192 + I, 0], 3, I);
      asm
      	MOV DX, OFFSET Colors
      	push ds
      	pop  es
      	mov ax, $1012
        xor bx, bx
        mov cx, $0100
        int $10
      end;
    end;

  procedure OpenVideo; assembler;
    asm
      MOV AX, $0013
      INT $10
      MOV ForeColor, 15
      MOV BAckColor, 0
    end;

  procedure CloseVideo; assembler;
    asm
      MOV AX, $0003
      INT $10
    end;

  procedure Clear ( X1, Y1, X2, Y2 : word );
    var
      Wide : word;
      J    : Word;
    begin
      Wide := X2 - X1 + 1;
      For J := Y1 to Y2 do
        FillChar(WriteScreen^[J, X1], Wide, ForeColor );
    end;

  procedure Draw;
    begin
        WriteScreen^ := EditScreen^;
    end;

type
	TESaver = object (TScreenSaver)
		constructor Create;
		destructor Destroy; virtual;
		procedure Refresh; virtual;
		procedure Show; virtual;
		procedure Hide; virtual;
	end;

constructor TESaver.Create;
begin
	inherited Create;
	New(EditScreen);
end;

destructor TESaver.Destroy;
begin
    Dispose(EditScreen);
	inherited Destroy;
end;

procedure TESaver.Refresh;
var
	I : Word;
	J : word;

begin
	inherited Refresh;
	Generation;
	Draw;
	While (Ticks - Last < Speed) and (Ticks - Last >= 0) do;
	Last := Ticks;
end;

procedure TESaver.Show;
var
    X, Y : word;
begin
	inherited Show;
	Randomize;
	WriteScreen := Ptr($A000, 0);
	for Y := 0 to 199 do
	 for X := 0 to 319 do
	    if Random(2) <> 0 then
	        EditScreen^[Y,X] := 255
	    else
	        EditScreen^[Y,X] := 0;
	OpenVideo;
	SetColors;
	ForeColor := 0;
	Clear ( 0, 0, 319, 199 );
	WriteScreen^ := EditScreen^;
end;

procedure TESaver.Hide;
begin
	CloseVideo;
	inherited Hide;
end;

var
	ESaver : TESaver;

begin
	ESaver.Create;
	ESaver.Execute;
	ESaver.Destroy;
end.

