(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ Copyright 1992-2015 Jerome Shidel }

{$I ESAVER.DEF}
program Atom_Screen_Saver;

  uses QCrt, QScrnSvr;
  
{$I ..\ETERNITY\LICENSE.INC}

  const
    Speed        = 1;
    Energy       = 1000;
    EnergyNeeded = 125;
    StartMax     = 50;
    MaxJump      = 25;
    Life         = 500;
    Explode		 = 364;

  var
    Ticks : longint absolute $0040:$006c;

  const
    Last  : LongInt = 0;
    Count : Word = 0;

  type
    ScreenArray = array[0..199, 0..319] of byte;

  var
    Colors : array[0..255,0..2] of byte;
    WriteScreen : ^ScreenArray;
    ForeColor,
    BackColor   : byte;

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

  procedure NewPixel ( X, Y : word; C : byte ); forward;

  type
    PPixel = ^OPixel;
    OPixel = object
        Next : PPixel;
        X, Y : word;
        D, C : byte;
        MV   : word;
        Left : word;
      constructor Init ( XX, YY : word; CC : Byte );
      destructor  Done;
      procedure   Draw;
    end;

  constructor OPixel.Init;
    begin
      X := XX;
      Y := YY;
      D := Random( 8 );
      C := CC;
      MV   := EnergyNeeded;
      Next := nil;
      Left := Life
    end;

  destructor OPixel.Done;
    begin
      WriteScreen^[Y, X] := 0;
    end;

  procedure OPixel.Draw;
    var
      TNext : PPixel;
      V     : Word;
    begin
      WriteScreen^[Y, X] := 0;
      if Left = 0 then Exit;
      if X > 319 then X := 1;
      if Y > 199 then Y := 1;
      if X < 1   then X := 319;
      if Y < 1   then Y := 199;
      Case D of
        0 : begin inc ( X ); end;
        1 : begin inc ( Y ); end;
        2 : begin dec ( X ); end;
        3 : begin dec ( Y ); end;
        4 : begin inc ( X ); inc ( Y ); end;
        5 : begin inc ( X ); dec ( Y ); end;
        6 : begin dec ( X ); dec ( Y ); end;
        7 : begin dec ( X ); inc ( Y ); end;
      end;
      Dec(Left);
      if MV > 0 then
        begin
          V := Energy div Count;
          if V > MV then
            MV := 0
          else
            Dec(MV, V);
        end;
      if (WriteScreen^[Y, X] <> 0 ) then
        begin
          D    := Random ( 8 );
          if MV = 0 then
            begin
              Left := Life;
              NewPixel ( X, Y, C);
            end;
          MV   := EnergyNeeded;
        end;
      if Left = 0 then Dec( Count );
      WriteScreen^[Y, X] := C + 63 - (MV SHR 3);
      if Assigned(Next) and ( Next^.Left = 0) then
        begin
          TNext := Next;
          Next := Next^.Next;
          Dispose(TNext, Done);
        end;
    end;

  const
    First : PPixel = nil;
    Total : word   = 0;

  procedure NewPixel;
    var
      TPixel : PPixel;
    begin
      if (MaxAvail > Sizeof(OPixel) + 1024 ) then
        begin
          TPixel := New(PPixel, Init ( X, Y, C ));
          TPixel^.Next := First;
          First := TPixel;
          Inc (Count);
          Total := Count;
        end
      else
        Count := StartMax;
    end;

  procedure DonePixel;
    var
      TPixel : PPixel;
    begin
      While Assigned(First) do
        begin
          TPixel := First^.Next;
          Dispose(First, Done);
          First := TPixel;
        end
    end;

  procedure Draw;
    var
      TPixel : PPixel;
    begin
      TPixel := First;
      While Assigned(TPixel) do
        begin
          TPixel^.Draw;
          TPixel := TPixel^.Next;
        end;
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
end;

destructor TESaver.Destroy;
begin
	inherited Destroy;
end;

procedure TESaver.Refresh;
var
	I : Word;
	J : word;
const
	S : LongInt = 0;
	
begin
	inherited Refresh;
	Draw;
	if (Ticks - S > Explode) or (Count = 0) then begin
		J := StartMax * 2 - Count;
		if J > MaxJump then J := MaxJump;
		if Count < StartMax then
			for I := 1 to J do NewPixel(160, 100, Random(4) * 64 );
		S := Ticks;
	end;
	While (Ticks - Last < Speed) and (Ticks - Last >= 0) do;
	Last := Ticks;
end;

procedure TESaver.Show;
begin
	inherited Show;
	Randomize;
	WriteScreen := Ptr($A000, 0);
	OpenVideo;
	SetColors;
	ForeColor := 0;
	Clear ( 0, 0, 319, 199 );
end;

procedure TESaver.Hide;
begin  
    DonePixel;
	CloseVideo;
	inherited Hide;
end;
	
var
	ESaver : TESaver;

begin
	ESaver.Create;
	ESaver.Execute;
	ESaver.Destroy;   
end.
