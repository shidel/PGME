{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Config_File_Toolkit; { X Series }

uses QCrt, QStrings;
{$I ..\ETERNITY\LICENSE.INC}

{$I-} { No I/O Checking }

{ --- Global Variables --- }

type 
	PLine = ^TLine;
	TLine = record
		Prev, Next : PLine;
		Line : PString;
	end; 
	
var
	Top, Bottom, First, Last : PLine;
	
procedure AddLine(S : String);
var
	T : PLine;
	I : integer;
begin
	if Not MemCheck(Sizeof(String)) then halt(8);
	S := RTrim(S);
	T := New(PLine);
	T^.Next := nil;
	if Not Assigned(First) then begin
		First := T;
		Last := T;
		T^.Prev := nil;
	end else begin
		Last^.Next := T;
		T^.Prev := Last;
		Last := T;
	end;
	T^.Line := StrPtr(S)
end;
	
procedure LoadFiles;
var
	J, I, Max : integer;
	F : File;
	C : Word;
	S, T : String;
begin
	Max := Lo(WindMax) - 1;
	for J := 1 to ParamCount do begin
		Assign(F, ParamStr(J));
		Reset(F, 1);
		S := '';
		repeat
			BlockRead(F, T[1], Max, C);
			T[0] := Chr(C);
			S := S + FilterChar(T, #$0D);
			if S <> '' then begin
				repeat	
					{ Expand Tabs before CR }				
					I := Pos(#$09, S);
					while (I > 0) and (I < Max) and (I < Pos(#$0A, S)) do begin
						Delete(S, I, 1);
						Insert(Space(4 - ((I-1) mod 4)), S, I);
						I := Pos(#$09, S);
					end;
					{ Find CR }
					I := Pos(#$0A, S);
					if (I = 0) or (I > Max) then begin
						{ if Not found before max, look for space }
						I := LastPos(#32, Copy(S, 1, Max));
						if (I < 1) or (I >= Max) then
							I := Max;
					end;
					if I <> 0 then begin
						AddLine(FilterChar(Copy(S, 1, I), #$0A));
						S := Copy(S, I + 1, Length(S));
					end;
				until (Length(S) < Max) or (I = 0);
			end;
		Until (C < Max) or (IOResult <> 0);
		if Length(S) > 0 then AddLine(S);
		Close(F);
	end;
end;
	
procedure Viewer;
var
	I : integer;
	E : TEvent;
begin
	InitQCrtMaps;
	LoadFiles;
	Top := First;
	repeat
		Bottom := Top;
		ClrScr;
		I := 0; 
		while Assigned(Bottom) and (I < Hi(WindMax) - 1) do begin 
			Inc(I);
			FWriteLn(PtrStr(Bottom^.Line));
			Bottom := Bottom^.Next;
		end;
		ClearEvent(E);
		While E.What = evNothing do GetEvent(E);
		if E.What = evCommand then
			Case E.Command of
				cmUp : begin
					ClearEvent(E);
					if Top^.Prev <> nil then
						Top := Top^.Prev;
				end;
				cmDown : begin
					ClearEvent(E);
					if Top^.Next <> nil then
						Top := Top^.Next;
				end;
			end;
	until E.What <> evNothing;
		
end;

begin
	TurnMouseOff;
	if ParamCount = 0 then
		WriteLn('Usage: QSCROLL filename')
	else
		Viewer;
end.
