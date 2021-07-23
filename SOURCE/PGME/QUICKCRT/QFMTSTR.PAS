{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QFmtStr; { QuickCrt String Format Utility }
{$I QCRT.DEF}

interface

uses QStrings;

const
	fmtBoolean = 0;
	fmtInteger = 1;
	fmtReal    = 2;
	fmtString  = 3;

type 
	TFormatData = record
		ID : integer;
		case word of 
			fmtBoolean : ( BooleanValue : Boolean );
			fmtInteger : ( IntegerValue : LongInt );
			fmtReal    : ( RealValue : Real );
			fmtString  : ( StringValue : String );
	end;
	PFormatArray = ^TFormatArray;
	TFormatArray = array[0..9] of TFormatData;
	

function FormatStr(Format : String; const Data; Count : byte) : String;
(*  
	Example '0[Z2.1,C10]{One,Two,Three},1[U,C10,X10,T],0;Hi %2 %1 %0!'
	0-9 Value to use
	[] Formating Options
	{} String Set, if value is not in set than value is used {boolean & integer}
	Z ZeroPad Before.after decimal point { integer & real }
	T Trim Spaces from string (before padding or cutting) 
	C, L or R Space padding Center, Left and Right
	X Crop string to not excede Length
	U, D or W Change Case to Upper, Lower or by Words
	
	; ends format definitions and begins string to format
	%0-9 Format definition to insert. 
*)
	

implementation

function FormatStr(Format : String; const Data; Count : byte) : String;
var
	Buf : array [0..9] of string;
	I, B, N, LZ, RZ, SM, SP, CC, XL, XP : integer;
	StrSet, Next, S, OutStr, T : String;
	C : String[10];
	UseSet, FD, DT, XS : boolean;
begin
	FormatStr := Format; { Any errors just returns Format String }
	I := Pos(';', Format);
	if I < 1 then exit;
	for I := 0 to 9 do
		Buf[I] := '';
	
	Next := PullStr(';', Format);
	B := 0;
	while (Next <> '') and (b < 10) do begin
		N := StrInt(PullChars(1, Next));
		StrSet := '';
		LZ := -1; { Left Zeros }
		RZ := -1; { Right Zeros }
		SM := -1; { Space Pad Mode Left 0, Center 1, Right 2 }
		SP := -1; { Space Padding }
		CC := -1; { Case Change, U Upper, D LowerCase, W Words }
		DT := False; { Do Trim }
		XS := False; { Do Cut }
		XL := -1;
		XP := -1;
		repeat
			FD := False;
			if (Length(Next) > 0) then begin
				if (Next[1] = '{') then begin
					FD := True;
					StrSet := PullStr('}', Next);
				end;
				if (Next[1] = '[') then begin
					FD := True;
					PullChars(1, Next);
					T := PullStr(']', Next);
					while (T <> '') do begin
						S := PullStr(',', T);
						C := UCase(PullChars(1, S));
						if C <> '' then
							case C[1] of 
								'Z' : begin
									C := PullStr('.', S);
									LZ := StrInt(C);
									if S <> '' then
										RZ := StrInt(S);
								end;
								'C' : begin 
									SM := 0;
									SP := StrInt(S);
								end;
								'L' : begin 
									SM := 1;
									SP := StrInt(S);
								end;
								'R' : begin 
									SM := 2;
									SP := StrInt(S);
								end;
								'U' : begin CC := 0; end;
								'D' : begin CC := 1; end;
								'W' : begin CC := 2; end;
								'T' : begin DT := True; end;
								'X' : begin 
									XS := True;
									if Pos('.', S) = 0 then begin
										XP := 1;
										XL := StrInt(S);
									end else begin
										XP := StrInt(PullStr('.', S));
										XL := StrInt(S);
									end;
								end;
							end;				
					end;
				end;
			end;
		until FD = false;
				
		{ Set Values in Buffer }
		case TFormatArray(Data)[N].ID of
			fmtBoolean 	: begin
				if StrSet = '' then 
					Buf[B] := BoolStr(TFormatArray(Data)[N].BooleanValue)
				else begin
					PullChars(1, StrSet);
					if TFormatArray(Data)[N].BooleanValue then begin
						S := PullStr(',', StrSet);
						Buf[B] := PullStr(',', StrSet);
					end else
						Buf[B] := PullStr(',', StrSet);												
				end;
			end;
			fmtInteger 	: begin
				I := 1;
				if StrSet <> '' then begin
					I := TFormatArray(Data)[N].IntegerValue + 1;
					PullChars(1, StrSet);
					S := '';
					while (I > 0) and (StrSet <> '') do begin
						S := PullStr(',', StrSet);
						Dec(I);
					end;
					if I = 0 then 
						Buf[N] := S;
				end;
				if I <> 0 then begin
					Buf[B] := IntStr(TFormatArray(Data)[N].IntegerValue);
					if LZ > 0 then 
						Buf[B] := ZPad(Buf[B], LZ);
					if RZ > 0 then 
						Buf[B] := Buf[B] + '.' + ChrStr('0', RZ);
				end;
			end;
			fmtReal 	: begin
				if RZ >= 0 then 
					Str(TFormatArray(Data)[N].RealValue:1:RZ, Buf[B])
				else
					Str(TFormatArray(Data)[N].RealValue, Buf[B]);
				LZ := LZ - Pos('.', Buf[B]) + 1;
				if LZ > 0 then 
					Buf[B] := ChrStr('0', LZ) + Buf[B];
				RZ := RZ - (Length(Buf[N]) -  Pos('.', Buf[B])) - 1;
				if RZ > 0 then 
					Buf[B] := Buf[B] + ChrStr('0', RZ);
			end;
			fmtString 	: Buf[B] := TFormatArray(Data)[N].StringValue;
		end;
		
		if DT then Buf[B] := Trim(Buf[B]);
		
		case SM of
			0 : Buf[B] := CSpace(Buf[B], SP);
			1 : Buf[B] := LSpace(Buf[B], SP);
			2 : Buf[B] := RSpace(Buf[B], SP);
		end;
		
		case CC of 
			0 : Buf[B] := UCase(Buf[B]);
			1 : Buf[B] := LCase(Buf[B]);
			2 : Buf[B] := WCase(Buf[B]);
		end;
		
		if XS then Buf[B] := Copy(Buf[B], XP, XL);
		
		if (Length(Next) > 0) and (Next[1] = ',') then 
			PullChars(1, Next);
		Inc(B);
	end;
	
	OutStr := '';
	I := 1;
	while I <= Length(Format) do
		begin
			S := Format[I];
			if S = '%' then begin
				inc(I);
				if I > Length(Format) then exit;
				S := Format[I];
				if (S <> '%') then
					S := Buf[StrInt(S)];
			end;
			OutStr := OutStr + S;
			Inc(I);
		end;
	
	FormatStr := OutStr;
end;


end.

