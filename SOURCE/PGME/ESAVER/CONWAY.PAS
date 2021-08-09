{ Copyright 2015-2021 Jerome Shidel }

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
    CellUnder = 2;
    CellBirth = 3;
    CellOver  = 3;

  var
    Ticks : longint absolute $0040:$006c;

  const
    Last  : LongInt = 0;
    Interval : LongInt = 0;
    MaxInterval : longint = 30;
    Current : byte = 5;


  type
    ScreenArray = array[0..199, 0..319] of byte;
    RGBColors = array[0..255,0..2] of byte;

  var
    WriteScreen, EditScreen : ^ScreenArray;
    Colors : RGBColors;
    DrawColor : byte;

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
        FillChar(WriteScreen^[J, X1], Wide, 0 );
    end;


type
	TESaver = object (TScreenSaver)
	        Method : integer;
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
	Method := 1;
end;

destructor TESaver.Destroy;
begin
    Dispose(EditScreen);
	inherited Destroy;
end;

procedure RandScreen;
var
    I, J, K, X , Y, C : word;
    F : Boolean;
begin
    if Interval < MaxInterval then Inc(Interval, 5);
    Inc(Current);
    if Current > 3 then Current := 0;
    DrawColor := Current * 64 + 63;
    case 1 of
        0: begin
            for Y := 0 to 199 do
             for X := 0 to 319 do begin
                C := Random(100);
                F:= ((C < 3) and (Not F)) or ((C >20) and F);
                if F then
                    WriteScreen^[Y,X] := DrawColor;
             end;
            for X := 0 to 319 do
             for Y := 0 to 199 do begin
                C := Random(100);
                F:= ((C < 3) and (Not F)) or ((C >20) and F);
                if F then
                    WriteScreen^[Y,X] := DrawColor;
             end;
        end;
        1: begin
           for I := 0 to Random(100) + 5 do begin
                C := Random(20) + 1;
                X := Random(320 - C);
                Y := Random(200 - C);
                for J := 0 to C do
                    for K := 0 to C do
                        WriteScreen^[Y +J,X+K] := DrawColor;
           end;

        end;
    end;

end;

procedure Generation; assembler;
asm
    mov     bl, DrawColor;
    push    ds
    les     di, EditScreen
    lds     si, WriteScreen
    mov     cx, 320 * 200 - 1
    push    cx
    push    ds
    push    si
    push    es
    push    di
    cld
@@GenLoop:
    xor     ah, ah
    push    si
    sub     si, 321
    lodsb
    cmp     al, 0
    jz      @@Dead1
    inc     ah
@@Dead1:
    lodsb
    cmp     al, 0
    jz      @@Dead2
    inc     ah
@@Dead2:
    lodsb
    cmp     al, 0
    jz      @@Dead3
    inc     ah
@@Dead3:
    add     si, 317
    lodsb
    cmp     al, 0
    jz      @@Dead4
    inc     ah
@@Dead4:
    inc     si { Skip self }
    lodsb
    cmp     al, 0
    jz      @@Dead5
    inc     ah
@@Dead5:
    add     si, 317
    lodsb
    cmp     al, 0
    jz      @@Dead6
    inc     ah
@@Dead6:
    lodsb
    cmp     al, 0
    jz      @@Dead7
    inc     ah
@@Dead7:
    lodsb
    cmp     al, 0
    jz      @@Dead8
    inc     ah
@@Dead8:
    cmp     ah, CellUnder
    jb      @@DieOff
    cmp     ah, CellOver
    ja      @@DieOff
    cmp     ah, CellBirth
    jne     @@NextCell
@@GiveBirth:
    mov     ah, es:[di]
    cmp     ah, 0
    jne     @@NextCell
    mov     ah, bl
    jmp     @@ChangeCell
@@DieOff:
    mov     ah, 0
@@ChangeCell:
    mov     es:[di], ah
@@NextCell:
    pop     si
    inc     si
    inc     di
    loop    @@GenLoop
    pop     si
    pop     ds  { EditScreen now ds:[si] }
    pop     di
    pop     es  { WriteScreen now es:[di] }
    pop     cx
    shr     cx, 1
    cld
@@CopyLoop:
    lodsw   { mov ax, ds:[si]; add si, 2 }
    stosw   { mov es:[di], ax; add di, 2 }
    loop    @@CopyLoop
    pop     ds
end;

procedure TESaver.Refresh;
begin
	inherited Refresh;
    Generation;
	if (Ticks - Last > Interval * 18) or (Ticks < Last) then begin
	    RandScreen;
    	Last := Ticks;
    end;
end;

procedure TESaver.Show;
begin
	inherited Show;
	WriteScreen := Ptr($a000,0);
	Randomize;
	OpenVideo;
	SetColors;
	Clear ( 0, 0, 319, 199 );
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

