{ Copyright 2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ESAVER.DEF}
program Snow;

  uses QCrt, QScrnSvr;

{$I ..\ETERNITY\LICENSE.INC}

  var
    Ticks : longint absolute $0040:$006c;

  const
    Speed : word = 0;
    Last  : LongInt = 0;
    Count : Word = 0;

  type
    ScreenArray = array[0..199, 0..319] of byte;
    RGBColors = array[0..255,0..2] of byte;

  var
    WriteScreen, EditScreen : ^ScreenArray;
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
	X : Word;
	Y : word;

begin
	inherited Refresh;
	for Y := 0 to 199 do
	 for X := 0 to 319 do
        EditScreen^[Y,X] := 192 + Random(64);
	While (Ticks - Last < Speed) and (Ticks - Last >= 0) do;
	WriteScreen^ := EditScreen^;
	Last := Ticks;
end;

procedure TESaver.Show;
var
    X, Y : word;
begin
	inherited Show;
	Randomize;
	OpenVideo;
	SetColors;
	Clear ( 0, 0, 319, 199 );
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

