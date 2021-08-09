{ Copyright 2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ESAVER.DEF}
program Static;

  uses QCrt, QScrnSvr;

{$I ..\ETERNITY\LICENSE.INC}

  var
    Ticks : longint absolute $0040:$006c;

  const
    Last  : LongInt = 0;
    Count : Word = 0;

  type
    PScreenArray = ^TScreenArray;
    TScreenArray = array[0..199, 0..319] of byte;
    RGBColors = array[0..255,0..2] of byte;

  var
    WriteScreen, EditScreen : PScreenArray;
    Colors : RGBColors;

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
        FillChar(EditScreen^[J, X1], Wide, 0 );
    end;


type
	TESaver = object (TScreenSaver)
	        Method : byte;
	        Index : word;
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
	Index := 0;
end;

destructor TESaver.Destroy;
begin
    Dispose(EditScreen);
	inherited Destroy;
end;

procedure TESaver.Refresh;
var
	X,Y : Word;
    P : PScreenArray;
begin

	inherited Refresh;
	case Method of
	    0: begin
	        P := EditScreen;
        	for Y := 0 to 199 do
        	 for X := 0 to 319 do
                EditScreen^[Y,X] := 192 + Random(64);
           end;
	    1: begin
	            P := Ptr(Index,0);
	            Inc(Index,$1100);
	            if Index > $8000 then Dec(Index, $9000);
           end;

    end;
	{ While (Ticks = Last) do; }
	move(P^, WriteScreen^, Sizeof(TScreenArray));
	Last := Ticks;
end;

procedure TESaver.Show;
var
    X, Y : word;
begin
	inherited Show;
	Randomize;
	OpenVideo;
	{ SetColors; }
	WriteScreen := Ptr($A000, 0);
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

