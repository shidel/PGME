{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

unit QStrings;
{$I QCRT.DEF}
{$D-,Y-,L-}

interface

{$I QSTRINGS.INC}

  function NextPos ( SubStr : String; S : String; StartPos : byte ) : byte;
  { Like Pos function, except starts searching at Startpos Byte. Uses Pos function. }
  function LastPos ( SubStr : String; S : String ) : byte;
  { Like Pos function, except returns the last position of SubStr or zero. }
  function UCase ( Str : String ) : String;
  { Converts lowercase letters to uppercase }
  function LCase ( Str : String ) : String;
  { Converts uppercase letters to lowercase }
  function WCase ( Str : String ) : String;
  { Converts lowercase letters to uppercase }
  function RTrim ( Str : String ) : String;
  { removes trailing spaces and tabs }
  function LTrim ( Str : String ) : String;
  { removes preceding spaces and tabs }
  function Trim ( Str : String ) : String;
  { removes trailing and preceding spaces and tabs }

  function LPad( Str : String; C : Char; N : Byte ) : String;
  { Pads a string to the left with a character }
  function RPad( Str : String; C : Char; N : Byte ) : String;
  { Pads a string to the right with a character }
  function CPad( Str : String; C : Char; N : Byte ) : String;
  { Pads a string to the left and right with a character }

  function Space ( N : byte ) : String;
  { creates a string of N spaces }
  function LSpace ( Str : String; N : byte ) : String;
  { pads a string with spaces to the left like "Write (Str:N)" }
  function RSpace ( Str : String; N : Byte ) : String;
  { like LSpace except spaces go to the right }
  function CSpace ( Str : String; N : Byte ) : String;
  { like LSpace except centers inbetween spaces }

  function ZPad ( Str : String; N : Byte ) : String;
  { like LSpace except zero's are inserted }
  function ChrStr ( C : Char; N : byte ) : String;
  { creates a string of characters N bytes long }

  function CropStr( Str : String; N : Byte ) : String;
  { Insures a string does not exceed a specified max length }

  function HEXByte ( B : byte ) : Str2;
  function HEXWord ( W : Word ) : Str4;
  function HEXLong ( L : LongInt ) : Str8;
  function BINByte ( B : Byte ) : Str8;
  function BINWord ( W : Word ) : Str16;
  function BINLong ( L : LongInt ) : Str32;

  function BoolStr ( B : Boolean ) : Str5;
  { Returns True/False }
  function IntStr ( L : LongInt ) : Str20;
  { Returns String from Long Integer }
  function BinStr (D : LongInt) : String;
  { Returns a the number as a string of binary numbers, to most significant bit }
  function HexStr (I : LongInt ) : String;
  { Returns a the number as a string of binary numbers, to most significant byte }
  function RealStr ( R : {$IFOPT N+} Extended {$ELSE} Real {$ENDIF}; P : byte ) : Str40;
  { Returns String from Extended Real number }
  function Comma ( S : String ) : String;
  { Inserts commas into a number string handles positive, negative, integer,
    and formated reals of any size }

  function  StrBool(S : String) : Boolean;
  { Converts a String to a Boolean. True, On, Ok, Yes and Y return true }
  function  StrInt(S : String) : integer;
  { Converts a String to an Integer. If an error occurs, Zero is returned }
  function StrHex(S : String; PascalStyle : boolean) : String;
  { Converts a Strings to its Hex Values }
  function HexToStr(AStr : String) : String;

  function StrAsHex(AStr : String) : String;
  { Converts the ascii values of AStr to Hex values }

  function WhichStr (B : boolean; FalseStr, TrueStr : String) : String;
  { Depending on Boolean value, returns either FalseStr or TrueStr }
  function PluralStr(I : LongInt) : String;
  { Tests value and returns 's' if I is any value other than 1 }

  function FirstChar ( Str : String ) : Byte;
  { returns the position of the First character }
  function FilterChar ( Str : String; C : Char ) : String;
  { removes matching characters from a string }
  function ReplaceChar ( Str : String; O, N : Char ) : String;
  { Replaces characters in a string }
  procedure AddChar ( var S : String; C : Char );
  { Adds Character to end of String }
  procedure AddStr ( var S1 : String; S2 : String );
  { Adds Character to end of String }

  function PullStr(SubStr : String; var S : String) : String;
  { Returns String up to SubString and removes it from S. If SubStr is
  not present the entire string is "Pulled". SubStr is not returned in
  the result. }
  function PullChars(Count : Integer; var S : String) : String;

  function ReduceStr(S : String; Max : integer) : String;

type
	PString = ^String;

function  StrPtr(const S : String) : PString;
function  PtrStr(const P : PString) : String;
procedure FreeStr(var P : PString);

function PascalStr(const AsciiZ) : String;

function ReplaceStr ( Str : String; O, N : String; All : Boolean ) : String;
{ Replaces strings within a string }
function CountStr(SubStr, Str : String) : integer;
{ Returns total number of occurrences of SubStr within Str }
function FirstWord(Str : String) : String;
{ Returns the first "word" in a string, separated by first non-alphanumeric }
function PullWord(var Str : String) : String;
{ Returns "FirstWord", removing it and trimming Str }

function MatchWildCard ( AWild, AStr : String ) : boolean;

procedure SetLength(var S; NewLen : byte);

function OnlyAlpha(S : String) : string;
function OnlyAlphaNum(S : String) : string;
function OnlyChars(S : String; Allowed : String) : string;
function NotChars(S : String; Allowed : String) : string;

function FlipStr(S : String) : string;

function IsStr(TheStr, AStr, AsStr : String) : String;
{ Caseless, When The String is AStr return AsStr, otherwise return TheStr }
function NotStr(TheStr, AStr, AsStr : String) : String;
{ Caseless, When The String is not AStr return AsStr, otherwise return TheStr }
function StrIndex(TheStr : String; Strs : array of String) : integer;
{ Caseless, Returns the index in an array of the string, or -1 if not found }
procedure ROLStr(var AStr : String);
{ Rotates a string left, placing the first character at the end }
function SamePartOfStr(S1, S2 : String) : String;
{ return the start portion of the strings that are identical }

implementation

  function NextPos ( SubStr : String; S : String; StartPos : byte ) : byte;
    var
      StrPtr : ^String;
      TPos   : Byte;
    begin
      if (StartPos = 0) or (StartPos > Length(S)) then
        NextPos := 0
      else
        begin
          Dec(StartPos);
          S[StartPos] := Char(Length(S) - StartPos);
          StrPtr := @S[StartPos];
          TPos := Pos(SubStr, StrPtr^);
          if TPos > 0 then Inc(TPos, StartPos);
          NextPos := TPos;
        end;
    end;

  function LastPos ( SubStr : String; S : String ) : byte;
  var
  	Temp, P : byte;
  begin
  	Temp := 0;
    P := 0;
  	repeat
  		Inc(P);
  		P := NextPos(SubStr, S, P);
  		if P > 0 then Temp := P;
  	until P = 0;
  	LastPos := Temp;
  end;

{----------------------------------------------------------------------------}
  function UCase ( Str : String ) : String;
    begin
      asm
        CLD
        LEA     SI, Str
        LES     DI, @Result
        SEGSS   LODSB
        STOSB
        XOR     AH, AH
        XCHG    AX, CX
        JCXZ    @3
      @1:
        SEGSS   LODSB
        CMP     AL, 'a'
        JB      @2
        CMP     AL, 'z'
        JA      @2
        SUB     AL, 20h
      @2:
        STOSB
        LOOP    @1
      @3:
      end;
    end;
{----------------------------------------------------------------------------}
  function LCase ( Str : String ) : String;
    begin
      asm
        CLD
        LEA     SI, Str
        LES     DI, @Result
        SEGSS   LODSB
        STOSB
        XOR     AH, AH
        XCHG    AX, CX
        JCXZ    @3
      @1:
        SEGSS   LODSB
        CMP     AL, 'A'
        JB      @2
        CMP     AL, 'Z'
        JA      @2
        ADD     AL, 20h
      @2:
        STOSB
        LOOP    @1
      @3:
      end;
    end;
{----------------------------------------------------------------------------}
  function WCase ( Str : String ) : String;
    begin
      Str := LCase(Str);
      asm
      	XOR		BL, BL
        CLD
        LEA     SI, Str
        LES     DI, @Result
        SEGSS   LODSB
        STOSB
        XOR     AH, AH
        XCHG    AX, CX
        JCXZ    @Done
      @1:
        SEGSS   LODSB
        CMP     AL, 'a'
        JB      @2
        CMP     AL, 'z'
        JA      @2
        CMP		BL, 0
        JNE		@3
        INC 	BL
        SUB     AL, 20h
        JMP		@3
      @2:
      	XOR 	BL, BL
      @3:
        STOSB
        LOOP    @1

      @Done:
      end;
    end;
{----------------------------------------------------------------------------}
  function FilterChar ( Str : String; C : Char ) : String;
    begin
      asm
        CLD
        MOV     BL, C
        LEA     SI, Str
        LES     DI, @Result
        PUSH 	DI
        SEGSS   LODSB
        STOSB
        XOR     AH, AH
        XCHG    AX, CX
        JCXZ    @4
      @1:
        SEGSS   LODSB
        CMP 	AL, BL
        JE 		@3
      @2:
      	INC     AH
        STOSB
      @3:
        LOOP    @1
      @4:
      	POP 	DI
      	MOV 	ES:[DI], AH
      end;
    end;
{----------------------------------------------------------------------------}
  function ReplaceChar ( Str : String; O, N : Char ) : String;
    begin
      asm
        CLD
        MOV     BL, O
        MOV     BH, N
        LEA     SI, Str
        LES     DI, @Result
        SEGSS   LODSB
        STOSB
        XOR     AH, AH
        XCHG    AX, CX
        JCXZ    @3
      @1:
        SEGSS   LODSB
        CMP     AL, BL
        JNE     @2
        MOV     AL, BH
      @2:
        STOSB
        LOOP    @1
      @3:
      end;
    end;

  function FirstChar ( Str : String ) : Byte; assembler;
    asm
      PUSH DS
      LDS  DI, Str
      MOV  AX, 0
      MOV  CH, 0
      MOV  CL, DS:[DI]
    @@1:
      JCXZ @@2
      DEC  CX
      INC  DI
      INC  AH
      MOV  BL, DS:[DI]
      CMP  BL, 32
      JE   @@1
      MOV  AL, AH
    @@2:
      POP  DS
    end;
 { Non Asm Versions }
  function RTrim ( Str : String ) : String;
  begin
      asm
        CLD
		XOR		BL, BL
        LEA     SI, Str
        LES     DI, @Result
        PUSH 	DI
        SEGSS   LODSB
        STOSB
        XOR     AH, AH
        XCHG    AX, CX
        JCXZ    @4
      @1:
        SEGSS   LODSB
      	INC     AH
        CMP 	AL, 20h
        JE		@2
        CMP 	AL, 09h
        JE		@2
		MOV		BL, AH
      @2:
        STOSB
      @3:
        LOOP    @1
      @4:
      	POP 	DI
      	MOV 	ES:[DI], BL
      end;
  end;

  function LTrim ( Str : String ) : String;
  begin
      asm
        CLD
        MOV 	BL, False
        LEA     SI, Str
        LES     DI, @Result
        PUSH 	DI
        SEGSS   LODSB
        STOSB
        XOR     AH, AH
        XCHG    AX, CX
        JCXZ    @5
      @1:
        SEGSS   LODSB
        CMP 	BL, True
        JE 		@3
        CMP 	AL, 20h
        JE		@4
        CMP 	AL, 09h
        JE		@4
      @2:
      	MOV		BL, True
      @3:
      	INC     AH
        STOSB
      @4:
        LOOP    @1
      @5:
      	POP 	DI
      	MOV 	ES:[DI], AH
      end;
  end;

  function Trim ( Str : String ) : String;
  begin
      asm
        CLD
        MOV 	BL, False
        LEA     SI, Str
        LES     DI, @Result
        PUSH 	DI
        SEGSS   LODSB
        STOSB
        XOR     AH, AH
        XCHG    AX, CX
        JCXZ    @5
      @1:
        SEGSS   LODSB
        CMP 	BL, True
        JE 		@3
        CMP 	AL, 20h
        JE		@4
        CMP 	AL, 09h
        JE		@4
      @2:
      	MOV		BL, True
      @3:
      	INC     AH
        STOSB
      @4:
        LOOP    @1
      @5:
      	POP 	DI
      	XOR 	BH, BH
      	MOV 	BL, AH
      @6:
      	CMP		BX, 0h
      	JE		@8
      	MOV		AL, ES:[DI + BX]
      	CMP		AL, 20h
      	JE		@7
      	CMP		AL, 09h
      	JNE		@8
      @7:
      	DEC		BL
      	DEC		AH
      	JMP		@6
      @8:
      	MOV 	ES:[DI], AH
      end;
   end;


  function Space ( N : byte ) : String;
    Var
      T : String;
    begin
      FillChar ( T[1], N, 32);
      T[0] := Chr(N);
      Space := T;
    end;

  function LPad( Str : String; C : Char; N : Byte ) : String;
    begin
      if Length(Str) < N then
        LPad := ChrStr( C, N - Length(Str) ) + Str
      else
        LPad := Str;
    end;

  function RPad( Str : String; C : Char; N : Byte ) : String;
    begin
      if Length(Str) < N then
        RPad := Str + ChrStr( C,  N - Length(Str) )
      else
        RPad := Str;
    end;

  function CPad( Str : String; C : Char; N : Byte ) : String;
  begin
      CPad := RPad(ChrStr(C, (N div 2) - (Length(Str) div 2)) + Str, C, N);
  end;

  function LSpace ( Str : String; N : byte ) : String;
    begin
      if Length(Str) < N then
        LSpace := Space( N - Length(Str) ) + Str
      else
        LSpace := Str;
    end;

  function RSpace ( Str : String; N : Byte ) : String;
    begin
      if Length(Str) < N then
        RSpace := Str + Space( N - Length(Str) )
      else
        RSpace := Str;
    end;

  function CSpace ( Str : String; N : Byte ) : String;
    begin
      Str := Trim(Str);
      CSpace := RSpace(Space((N div 2) - (Length(Str) div 2)) + Str, N);
    end;

  function ZPad ( Str : String; N : byte ) : String;
    begin
      if Length(Str) < N then
        ZPad := ChrStr('0', N - Length(Str) ) + Str
      else
        ZPad := Str;
    end;

 function CropStr( Str : String; N : Byte ) : String;
 begin
 	if Length(Str) > N then
	  	if N < 1 then
 			Str := ''
	 	else
			Delete(Str, N + 1, Length(Str) - N );
	CropStr := Str;
 end;

{----------------------------------------------------------------------------}
  function HEXbyte ( B : byte ) : Str2; assembler;
    asm
      LES DI, @RESULT
      MOV AL, $02
      MOV ES:[DI], AL
      MOV AL, B
      MOV AH, B
      AND AH, $0F
      AND AL, $F0
      MOV CL, $04
      SHR AL, CL
      CMP AL, $9
      JNA @@1
      ADD AL, $07
    @@1:
      ADD AL, $30
      CMP AH, $9
      JNA @@2
      ADD AH, $07
    @@2:
      ADD AH, $30
      MOV ES:[DI + 1], AX
    end;

  function HEXWord ( W : Word ) : Str4;
    begin
      HEXWord := HEXByte(Hi(W)) + HEXByte(Lo(W));
    end;

  function HEXLong ( L : LongInt ) : Str8;
    type
      WordCast = array[0..1] of word;
    begin
      HEXLong := HEXWord(WordCast(L)[1]) + HEXWord(WordCast(L)[0]);
    end;

  function BINByte ( B : Byte ) : Str8; assembler;
    asm
      LES DI, @Result
      MOV AL, 8
      MOV ES:[DI], AL
      MOV CX, 8
      MOV AL, B
    @@1:
      INC DI
      MOV AH, '1'
      CMP AL, $80
      JAE @@2
      MOV AH, '0'
    @@2:
      MOV ES:[DI], AH
      SHL AL, 1
      DEC CX
      JNZ @@1
    end;

  function BINWord ( W : Word ) : Str16;
    begin
      BINWord := BINByte(Hi(W)) + BINByte(Lo(W));
    end;

  function BINLong ( L : LongInt ) : Str32;
    type
      WordCast = array[0..1] of word;
    begin
      BINLong := BINWord(WordCast(L)[1]) + BINWord(WordCast(L)[0]);
    end;

  function IntStr  ( L : LongInt ) : Str20;
    var
      S : Str20;
    begin
      Str ( L, S );
      IntStr := S;
    end;

	function BinStr (D : LongInt ) : String;
	var
		Temp : String;
	begin
		Temp := '';
		repeat
			if Odd(D) then
				Temp := '1' + Temp
			else
				Temp := '0' + Temp;
			D := D shr 1;
		until D = 0;
		BinStr := Temp;
	end;

	function HexStr (I : LongInt) : String;
	var
		Temp : string;
	begin
		Temp := '';
		repeat
			Temp := HEXByte(I) + Temp;
			I := I shr 8;
		until I = 0;
		HexStr := Temp;
	end;

  function RealStr ( R : {$IFOPT N+} Extended {$ELSE} Real {$ENDIF}; P : byte ) : Str40;
    var
      S : Str40;
    begin
      if P = 0 then
        Str ( R, S )
      else
        Str ( R:1:P, S );
      RealStr := S;
    end;

  function ChrStr ( C   : Char;   N : byte ) : String;
    var
      S : String;
    begin
      S[0] := Char(N);
      if N > 0 then FIllChar ( S[1], N, C );
      ChrStr := S;
    end;

  function BoolStr  ( B : Boolean ) : Str5;
    begin
      if B then BoolStr := 'TRUE' else BoolStr := 'FALSE';
    end;
{----------------------------------------------------------------------------}
  function Comma ( S : String ) : String;
    var
      I : Byte;
      P : Byte;
    begin
      S := Trim(S);
      P := Pos ('.', S);
      if P = 0 then P := Length(S) else Dec(P);
      I := P;
      While (I > 1) and (Pos(S[I - 1], '0123456789') <> 0) do
        if (P - I) and $3 = 2 then
          begin
            Insert ( ',', S, I );
            Inc(P);
          end
        else
          Dec(I);
      Comma := S;
    end;

  function  StrBool(S : String) : Boolean;
  { Converts a String to a Boolean. True, On, Ok, Yes, T and Y return true }
  begin
  	S := UCase(Trim(S));
  	StrBool := (S = 'TRUE') or (S = 'ON') or (S = 'YES') or (S = 'OK') or
  		(S = 'T') or (S = 'Y');
  end;

  function StrInt(S : String) : integer;
  var
    Temp, Err : integer;
  begin
    Val(S, Temp, Err);
    if Err <> 0 then Temp := 0;
  	StrInt := Temp;
  end;

function HexToStr(AStr : String) : String;
var
	I : integer;
	S, T, O : String;
begin
	O := '';
	S := AStr;
	while (S <> '') do begin
		I := Pos('$', S);
		if I > 0 then begin
			T := Copy(S, 1, I - 1);
			Delete(S, 1, I);
		end else begin
			T := S;
			S := '';
		end;
		if T <> '' then begin
			O := O + Char(StrInt('$' + T));
		end;
	end;
	HexToStr := O;
end;


  function StrHex(S : String; PascalStyle : boolean) : String;
	var
		I : integer;
		T : String;
	begin
		T := '';
		if PascalStyle then begin
			for I := 1 to Length(S) do
				T := T + '#$' + HexStr(Byte(S[I]));
		end else begin
			for I := 1 to Length(S) do
				T := T + '$' + HexStr(Byte(S[I]));
		end;
		StrHex := T;
	end;

  function WhichStr (B : boolean; FalseStr, TrueStr : String) : String;
  begin
  	if B then
  		WhichStr := TrueStr
  	else
  		WhichStr := FalseStr;
  end;

  function PluralStr(I : LongInt) : String;
  begin
  	PluralStr := WhichStr(I = 1, 's', '');
  end;

  procedure AddChar ( var S : String; C : Char );
    begin
      Inc(Byte(S[0]));
      S[Byte(S[0])] := C;
    end;

  procedure AddStr ( var S1 : String; S2: String );
    var
      MoveLen : Byte;
    begin
      MoveLen := Byte(S2[0]);
      if MoveLen + Length(S1) > 255 then MoveLen := 255 - Length(S1);
      Move ( S2[1], S1[Length(S1) + 1], MoveLen );
      Inc(Byte(S1[0]), MoveLen);
    end;

function StrPtr(const S : String) : PString;
var
	P : PString;
begin
	if MaxAvail < Length(S) + 1 then Halt(8);
	GetMem(P, Length(S) + 1);
	Move(S, P^, Length(S) + 1);
	StrPtr := P;
end;

function PtrStr(const P : PString) : String;
var
	S : String;
begin
	if Assigned(P) then
		Move(P^, S, Length(P^) + 1)
	else
		S := '';
	PtrStr := S;
end;

procedure FreeStr(var P : PString);
begin
	if Assigned(P) then
		FreeMem(P, Length(P^) + 1);
	P := nil;
end;

function PullStr(SubStr : String; var S : String) : String;
var
	P : integer;
begin
	P := Pos(SubStr, S);
	if P = 0 then begin
		PullStr := S;
		S := '';
	end else begin
		PullStr := Copy(S, 1, P - 1);
		S := Copy(S, P + Length(SubStr), Length(S));
	end;
end;

function PullChars(Count : Integer; var S : String) : String;
begin
	if Count = 0 then
		exit
	else if Count >= Length(S) then begin
		PullChars := S;
		S := '';
	end else begin
		PullChars := Copy(S, 1, Count);
		S := Copy(S, Count + 1, Length(S));
	end;
end;

function PascalStr(const AsciiZ) : String;
type
	AsciiZStr = array[0..$FF] of char;
var
	I : integer;
	S : String;
begin
	PascalStr := '';
	I := 0;
	while (I <= 254) and (AsciiZStr(AsciiZ)[I] <> #0) do Inc(I);
	if I <= 254 then begin
		Move(AsciiZ, S[1], I);
		S[0] := Chr(I);
		PascalStr := S;
	end;
end;

function ReduceStr(S : String; Max : integer) : String;
const
	Vowels : String = 'aeiouy';
	Keepers : String = '_-+=';
var
	I : Integer;
	T : String;
	F, P : boolean;
begin
	{ Strip spaces }
	S := FilterChar(WCase(S), ' ');

	{Strip lower case vowels on at a time}
	I := 0;
	while (Length(S) > Max) and (I < Length(Vowels)) do begin
		Inc(I);
		S := FilterChar(S, Vowels[I]);
	end;

	{ remove all special chars }
	I := Length(S);
	while (I > 0) and (Length(S) > Max) do begin
		if (S[I] > #$81) or (S[I] < #$20) then
			Delete(S, I, 1);
		Dec(I);
	end;

	{ remove all non letters }
	I := Length(S);
	while (I > 0) and (Length(S) > Max) do begin
		if ((UCase(S[I]) > 'Z') or (UCase(S[I]) < 'A')) and
		((Pos(S[I], Keepers) < 1) or ((I = 1) or (I = Length(S)))) then
			Delete(S, I, 1);
		Dec(I);
	end;

	{ remove repeating letters }
	I := Length(S);
	while (Length(S) > Max) and (I > 1) do begin
		Dec(I);
		if UCase(S[I]) = UCase(S[I + 1]) then
			Delete(S, I, 1);
	end;

	{ remove lowercase letters from words one at a time }
	F := True;
	while (Length(S) > Max) and F do begin
		F := False;
		I := Length(S);
		P := True;
		while I >2 do begin
			Dec(I);
			if S[I] = UCase(S[I]) then
				P := True
			else if P then begin
				P := False;
				F := True;
				Delete(S, I, 1);
			end;
		end;
	end;

	{Strip upper case vowels on at a time}
	T := UCase(Vowels);
	I := 0;
	while (Length(S) > Max) and (I < Length(T)) do begin
		Inc(I);
		S := FilterChar(S, T[I]);
	end;

   { remove repeating letters }
    P := True;
    while P do begin
    	P := False;
		I := Length(S);
		while (Length(S) > Max) and (I > 1) do begin
			Dec(I);
			if UCase(S[I]) = UCase(S[I + 1]) then begin
				Delete(S, I, 1);
				P := True;
			end;
		end;
	end;

	ReduceStr := Copy(UCase(S), 1, 8);
end;

function ReplaceStr ( Str : String; O, N : String;  All : boolean ) : String;
var
    P : integer;
begin
    P := Pos(O, Str);
    while P > 0 do begin
        Delete(Str, P, Length(O));
        Insert(N, Str, P);
        P :=NextPos(O, Str, P + Length(N));
        if not All then Break;
    end;
    ReplaceStr := Str;
end;

function SubMatchWC(AWild, AStr :String) : boolean;
var
    I : Integer;
begin
    SubMatchWC:= False;
    if Length(AWild) <> Length(AStr) then exit;
    for I := 1 to Length(AWild) do
        if (AWild[I] <> AStr[I]) and (AWild[I] <> '?') then exit;
    SubMatchWC := True;
end;

function QPos(ASub, AStr : String) : integer;
var
    I : integer;
begin
    QPos := 0;
    for I := 1 to Length(AStr) - Length(ASub) + 1 do
        if SubMatchWC(ASub, Copy(AStr, I, Length(ASub))) then begin
            QPos := I;
            Break;
        end;
end;

function MatchWildCard ( AWild, AStr : String ) : boolean;
{ far from perfect, but good enough for now. :-) }
var
    PW, PS : integer;
    X : integer;
begin
    X := 0;
    MatchWildCard := True;
    if AWild = AStr then Exit;
    if AWild = '' then begin
        MatchWildCard := False;
        Exit;
    end;
    repeat
        Inc(X);
        if AWild[1] = '*' then begin
            { WriteLn('[A-',AWild, '/',AStr,']'); }
            While (AWild<> '') and (AWild[1] = '*') do Delete(AWild, 1,1);
            if AWild = '' then Exit;
            PW := Pos('*', AWild);
            if PW < 1 then PW := Length(AWild)+ 1;
            { WriteLn(PW, ';', Copy(AWild, 1, PW -1), ';', AStr); }
            PS := QPos(Copy(AWild, 1, PW -1), AStr);
            { WriteLn(PS); }
            if PS < 1 then Break;
            Delete(AStr, 1, PS - 1);
        end;
        { WriteLn('[B-',AWild, '/',AStr,']'); }
        if SubMatchWC(AWild, AStr) then Exit;
        PW := Pos('*', AWild) - 1;
        { WriteLn(PW); }
        if PW < 1 then PW := Length(AWild) + 1;
        if not SubMatchWC(Copy(AWild,1, PW), Copy(AStr, 1, PW)) then Break;
        Delete(AWild, 1, PW);
        Delete(AStr, 1, PW);
        { WriteLn('[C-',AWild, '/',AStr,']'); }
    until (AWild = '') or (AStr = '') or (X = 1000);
    { WriteLn('[D-',AWild, '/',AStr,']'); }
    MatchWildCard := ((AWild = '*') or (AWild = '')) and (AStr = '');
end;

function CountStr(SubStr, Str : String) : integer;
var
    I, P : integer;
begin
    I := 0;
    P := 1;
    if SubStr <> '' then
        While True do begin
            P := NextPos(SubStr, Str, P);
            if P > 0 then begin
                Inc(I);
                P := P + Length(SubStr);
            end else
                break;
        end;
    CountStr := I;
end;

procedure HexDump(const D; L : word);
type
    M = array[0..65534] of byte;
var
    I : word;
begin
    I := 0;
    while I <= L do begin
        Write(HEXbyte(M(D)[I]), ' ');
        Inc(I);
        if I mod $10 = 0 then WriteLn;

    end;
    if I mod $10 <> 0 then WriteLn;
end;

function OnlyAlpha(S : String) : string;
var
    I : integer;
begin
    I := 1;
    while I <= Length(S) do begin
        if ((S[I] >= 'a') and (S[I] <= 'z')) or
        ((S[I] >= 'A') and (S[I] <= 'Z')) then
            Inc(I)
        else
            Delete(S,I,1);
    end;
    OnlyAlpha := S;
end;

function OnlyAlphaNum(S : String) : string;
var
    I : integer;
begin
    I := 1;
    while I <= Length(S) do begin
        if ((S[I] >= 'a') and (S[I] <= 'z')) or
        ((S[I] >= 'A') and (S[I] <= 'Z')) or
        ((S[I] >= '0') and (S[I] <= '9')) then
            Inc(I)
        else
            Delete(S,I,1);
    end;
    OnlyAlphaNum := S;
end;

function OnlyChars(S : String; Allowed : String) : string;
var
    I : integer;
begin
    I := 1;
    while I <= Length(S) do begin
        if (Pos(S[I], Allowed) > 0) then
            Inc(I)
        else
            Delete(S,I,1);
    end;
    OnlyChars := S;
end;

function NotChars(S : String; Allowed : String) : string;
var
    I : integer;
begin
    I := 1;
    while I <= Length(S) do begin
        if (Pos(S[I], Allowed) = 0) then
            Inc(I)
        else
            Delete(S,I,1);
    end;
    NotChars := S;
end;

function FlipStr(S : String) : string;
var
    I : integer;
    C : Char;
begin
    for I := 1 to (Length(S) + 1) div 2 do begin
        C := S[I];
        S[I] := S[Length(S) - I + 1];
        S[Length(S) - I + 1] := C;
    end;
    FlipStr:=S;

end;

function PullWord(var Str : String) : String;
var
   S : String;
begin
    S := FirstWord(Str);
    Delete(Str, 1, Length(S));
    PullWord := Trim(S);
    Str := Trim(Str);
end;

procedure SetLength(var S; NewLen : byte);
begin
    Byte(S) := NewLen;
end;

function FirstWord(Str : String) : String; assembler;
{ Returns the first "word" in a string, separated by first non-alphanumeric }
asm
    PUSH    DS
    CLD
    LES     DI, @Result
    LEA     SI, Str
    LDS     SI, SS:[SI]
    PUSH    DI
    LODSB
    SEGES   STOSB
    XOR     AH, AH
    XCHG    AX, CX
    JCXZ    @4
@1:
    LODSB
    CMP     AL, '0'
    JB      @3
    CMP     AL, '9'
    JBE     @2
    CMP     AL, 'A'
    JB      @3
    CMP     AL, 'Z'
    JBE     @2
    CMP     AL, 'a'
    JB      @3
    CMP     AL, 'z'
    JA     @3
@2:
    INC     AH
    SEGES   STOSB
    LOOP    @1
@3:
@4:
    POP     DI
    MOV     AL, AH
    SEGES   STOSB
    POP     DS
end;

function IsStr(TheStr, AStr, AsStr : String) : String;
{ Caseless, When The String is AStr return AsStr, otherwise return TheStr }
begin
    if UCase(TheStr) <> UCase(AStr) then
        IsStr := TheStr
    else
        IsStr := AsStr
end;

function NotStr(TheStr, AStr, AsStr : String) : String;
{ Caseless, When The String is not AStr return AsStr, otherwise return TheStr }
begin
    if UCase(TheStr) <> UCase(AStr) then
        NotStr := AsStr
    else
        NotStr := TheStr
end;

function StrIndex(TheStr : String; Strs : array of String) : integer;
{ Caseless, Returns the index in an array of the string, or -1 if not found }
var
    I : Integer;
begin
    StrIndex := -1;
    TheStr := UCase(TheStr);
    for I := Low(Strs) to High(Strs) do
        if (TheStr = UCase(Strs[I])) then begin
            StrIndex := I;
            Break;
        end;

end;

procedure ROLStr(var AStr : String); assembler;
asm
    xor     ch, ch
    push    ds
    lea     si, AStr
    lds     si, SS:[SI]
    mov     cl, [si]
    cmp     cl, 2
    jb      @@RotateDone
    mov     dh, [si + 1]
@@RotateStr:
    dec     cl
    jz      @@LastChar
    inc     si
    mov     dl, [si + 1]
    mov     [si], dl
    jmp     @@RotateStr
@@LastChar:
    mov     [si], dh
@@RotateDone:
    pop     ds
end;

function StrAsHex(AStr : String) : String;
var
    I : Integer;
    S : String;
begin
    S := '';
    for I := 1 to length(AStr) do
        S := S + HexByte(Ord(AStr[I]));
    StrAsHex := S;
end;

function SamePartOfStr(S1, S2 : String) : String;
{ return the start portion of the strings that are identical }
var
    I : integer;
begin
    I := 0;
    While (I < Length(S1)) and (I < Length(S2)) and (S1[i+1] = S2[I+2]) do Inc(I);
    SamePartOfStr := Copy(S1, 1, I);
end;


end.
