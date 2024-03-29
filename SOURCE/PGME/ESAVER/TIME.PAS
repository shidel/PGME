{ Copyright 2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ESAVER.DEF}
program Time;

  uses QCrt, QDos, QStrings, QScrnSvr, QFiles;

{$I ..\ETERNITY\LICENSE.INC}

  var
    Ticks : longint absolute $0040:$006c;
    ScaleX, ScaleY : integer;

  const
    Speed : word = 0;
    Last  : LongInt = 0;
    Count : Word = 0;
    LTime : String = '';
    VPos : word = 0;
    HPos : word = 0;
    Colr : word = 0;
    NChg : word = 0;

    Scale : word = 0;
    NewScale : word = 0;
    Scales : array [0..3] of TPoint = (
        (X:1; Y:1),
        (X:1; Y:2),
        (X:2; Y:2),
        (X:2; Y:3)
    );

const
    Rows = 21;
    MWide : word = 0; { Will self-adjust on first draw }

  type
    ScreenArray = array[0..199, 0..319] of byte;
    RGBColors = array[0..255,0..2] of byte;
    TDigits = array[0..11,0..Rows - 1] of String[32];

    const
        Digits : TDigits = (
            (
                ' 0000000000000000',
                '0 00000000000000 0',
                '00 000000000000 00',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '00              00',
                '0                0',
                '',
                '0                0',
                '00              00',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '00 000000000000 00',
                '0 00000000000000 0',
                ' 0000000000000000'
            ),
            (
                '',
                '                 0',
                '                00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '                00',
                '                 0',
                '',
                '                 0',
                '                00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '                00',
                '                 0',
                ''
            ),
            (
                ' 0000000000000000',
                '  00000000000000 0',
                '   000000000000 00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '                00',
                '   0000000000000 0',
                ' 0000000000000000',
                '0 00000000000000',
                '00',
                '000             ',
                '000             ',
                '000             ',
                '000             ',
                '000             ',
                '00 000000000000',
                '0 00000000000000',
                ' 0000000000000000'
            ),
            (
                ' 0000000000000000',
                '  00000000000000 0',
                '   000000000000 00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '                00',
                '  00000000000000 0',
                ' 0000000000000000',
                '  00000000000000 0',
                '                00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '   000000000000 00',
                '  00000000000000 0',
                ' 0000000000000000'
            ),
            (
                '',
                '0                0',
                '00              00',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '00              00',
                '0 00000000000000 0',
                ' 0000000000000000',
                '   0000000000000 0',
                '                00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '                00',
                '                 0',
                ''
            ),
            (
                ' 0000000000000000',
                '0 00000000000000',
                '00 000000000000',
                '000',
                '000',
                '000',
                '000',
                '000',
                '00',
                '0 00000000000000',
                ' 0000000000000000',
                '  00000000000000 0',
                '                00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '   000000000000 00',
                '  00000000000000 0',
                ' 0000000000000000'
            ),
            (
                ' 0000000000000000',
                '0 00000000000000',
                '00 000000000000',
                '000',
                '000',
                '000',
                '000',
                '000',
                '00',
                '0 00000000000000',
                ' 0000000000000000',
                '0 00000000000000 0',
                '00              00',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '00 000000000000 00',
                '0 00000000000000 0',
                ' 0000000000000000'
            ),
            (
                ' 0000000000000000',
                '  00000000000000 0',
                '   000000000000 00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '                00',
                '                 0',
                '                  ',
                '                 0',
                '                00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '                00',
                '                 0',
                ''
            ),
            (
                ' 0000000000000000',
                '0 00000000000000 0',
                '00 000000000000 00',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '00              00',
                '0 00000000000000 0',
                ' 0000000000000000',
                '0 00000000000000 0',
                '00              00',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '00 000000000000 00',
                '0 00000000000000 0',
                ' 0000000000000000'
            ),
            (
                ' 0000000000000000',
                '0 00000000000000 0',
                '00 000000000000 00',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '000            000',
                '00              00',
                '0 00000000000000 0',
                ' 0000000000000000',
                '  00000000000000 0',
                '                00',
                '               000',
                '               000',
                '               000',
                '               000',
                '               000',
                '                00',
                '                 0',
                ''
            ),
            (
                '',
                '',
                '',
                '',
                '',
                '',
                '  0000',
                '  0000',
                '  0000',
                '',
                '',
                '',
                '  0000',
                '  0000',
                '  0000',
                '',
                '',
                '',
                '',
                '',
                ''
            ),
            (
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  ',
                '  '
            )
        );

  var
    WriteScreen, EditScreen : ^ScreenArray;
    Colors : RGBColors;


  procedure SetColors;
    var
      I: byte;
      X: Word;
    begin
        FillChar ( Colors, Sizeof(Colors), 0);
        { for I := 0 to 15 do begin
          Colors[I, 0] := I * 4;
          Colors[I, 1] := I * 4;
          Colors[I, 2] := I * 4;
        end; }
        for I := 1 to 255 do begin
            repeat
                Colors[I, 0] := Random(16) * 4;
                Colors[I, 1] := Random(16) * 4;
                Colors[I, 2] := Random(16) * 4;
                X := Colors[I, 0] + Colors[I, 1] + Colors[I, 2];
            until (X <= 128) and (X > 64);

        end;
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

    procedure Clear ( X1, Y1, X2, Y2 : word; Color : byte );
    var
      Wide : word;
      J    : Word;
    begin
      Wide := X2 - X1 + 1;
      For J := Y1 to Y2 do
        FillChar(EditScreen^[J, X1], Wide, Color );
    end;

    procedure DrawBit(X, Y, B : word);
    var
        PY, PX : word;
    begin
        if B = $20 then exit;
        for PY := 0 to ScaleY - 1 do
            for PX := 0 to ScaleX - 1 do
                EditScreen^[VPos + Y * ScaleY + PY, HPos + X * ScaleX + PX] := Colr;
    end;

    function DrawChar(C : Char; X : Word ) : word;
    var
        I, J, K, W : word;
    begin
        W := 0;
        if (C >= '0') and (C <= '9') then
            I := Ord(C) - $30
        else if C = ':' then I := 10;
        if I > High(Digits) then I := High(Digits);
        for J := 0 to Rows - 1 do begin
            if Length(Digits[I,J]) > W then
                W := Length(Digits[I,J]);
            for K := 1 to Length(Digits[I,J]) do
                DrawBit(X + K - 1, J, Ord(Digits[I,J][K]))
        end;
        DrawChar := X + W + 2;
    end;

    procedure RandColor;
    begin
        Colr := Random(255) + 1;
    end;

    procedure RandPos;
    begin
        HPos := Random(320 - MWide * ScaleX);
        VPos := Random(200 - Rows * ScaleY);
    end;

    procedure Rescale;
    begin
        if Scale > High(Scales) then Scale := High(Scales);
        if NewScale = Scale then exit;
        Scale:=NewScale;
        ScaleX:= Scales[Scale].X;
        ScaleY:= Scales[Scale].Y;
        if HPos + MWide * ScaleX >= 320 then
            HPos :=320 - MWide * ScaleX - 1;
        if VPos + MWide * ScaleX >= 200 then
            VPos := 200 - Rows * ScaleY - 1;
        RandPos;
        RandColor;
    end;

    procedure ShowTime;
    var
        I, X : word;
        H, M, S: word;
        T : String;
    begin
        GetTime(H, M, S, I);
        T := ZPad(IntStr(H), 2) + ':' + ZPad(IntStr(M), 2) + ':' + ZPad(IntStr(S), 2);
        if T = LTime then Exit;
        LTime := T;
     	Clear (HPos, VPos, HPOS + MWide * ScaleX, VPOS + (Rows * ScaleY) - 1, 0);
     	Rescale;
        if NChg = 0 then begin
            NChg := Random(10) + 5;
            if Random(3) <> 0 then
                RandPos
            else
                RandColor;
        end;
        Dec(NChg);
        X := 0;
        for I := 1 to Length(T) do
            X := DrawChar(T[I], X);
        MWide := X;
    end;

type
	TESaver = object (TScreenSaver)
		constructor Create;
		destructor Destroy; virtual;
		procedure Refresh; virtual;
		procedure Show; virtual;
		procedure Hide; virtual;
        procedure CheckEvent(var Event : TEvent); virtual;
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
	ShowTime;
	While (Ticks - Last < Speed) and (Ticks - Last >= 0) do;
	WriteScreen^ := EditScreen^;
	Last := Ticks;
end;

procedure TESaver.Show;
var
    X, Y : word;
begin
	inherited Show;
	ScaleX := Scales[Low(Scales)].X;
	ScaleY := Scales[Low(Scales)].Y;
	Randomize;
	OpenVideo;
	WriteScreen := Ptr($A000, 0);
	SetColors;
	{ Predraw once to calculate MWide, reset LTime so it is draw }
	ShowTime;
	LTime := '';
    RandColor;
    RandPos;
    Clear(0,0,319,199,0);
end;

procedure TESaver.Hide;
begin
	CloseVideo;
	inherited Hide;
end;

procedure TESaver.CheckEvent(var Event : TEvent);
begin
    if Event.What = evKeyDown then
        if (UCase(Event.CharCode) = 'S') then begin
            Inc(NewScale);
            if NewScale > High(Scales) then NewScale := Low(Scales);
            ClearEvent(Event);
        end;
end;

procedure Run;
var
	ESaver : TESaver;
	Cfg : TFile;
	IScale : Integer;
	FM : Word;
	CFN, S, O : String;
begin
    Cfn:=Dir(GetExePath) + GetExeBase + '.CFG';
    FM := FileMode;
    FileMode := 0;
    Cfg.Create;
    Cfg.Assign(Cfn);
    Cfg.Reset;
    While (Cfg.Result = 0) and (Not Cfg.EOF) do begin
        Cfg.ReadLn(S, Sizeof(S) - 1);
        O := Ucase(Trim(FilterChar(PullStr('=', S),#9)));
        S := Trim(S);
        if (O = 'SCALE') and (S<>'') then
            NewScale:=StrInt(S)
    end;
    Cfg.Close;
    IScale := NewScale;
    FileMode := FM;
    IdleMethod := imAny;
	ESaver.Create;
	ESaver.Execute;
	ESaver.Destroy;
	if (IScale <> Scale) then begin
        Cfg.Rewrite;
        S := '['+ GetEXEBase + ']';
        Cfg.WriteLn(S);
        S := '';
        Cfg.WriteLn(S);
        S := #9+'SCALE=' + IntStr(Scale);
        Cfg.WriteLn(S);
        Cfg.Close;
    end;
    Cfg.Destroy;
end;

begin
    Run;
end.

