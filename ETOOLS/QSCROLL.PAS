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
	if Not MemCheck(1024) then halt(8);
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
	T^.Line := StrPtr(RTrim(S))
end;

procedure AddLines(var S :String; All : boolean);
var
	Max, I : integer;
begin
	Max := Lo(WindMax);
	while ((S <> '') and All) or (Length(S) > Max) do
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
				if (I < 1) or (I > Max) then
					I := Max;
			end;
			if I <> 0 then begin
				AddLine(FilterChar(Copy(S, 1, I), #$0A));
				S := Copy(S, I + 1, Length(S));
			end;
		until (Length(S) < Max) or (I = 0);
end;

procedure LoadFiles;
const
	Max = 80;

var
	J : integer;
	F : File;
	C : Word;
	S, T : String;
	
begin
	for J := 1 to ParamCount do begin
		Assign(F, ParamStr(J));
		Reset(F, 1);
		S := '';
		repeat
			BlockRead(F, T[1], Max,  C);
			T[0] := Chr(C);
			S := S + FilterChar(T, #$0D);
			AddLines(S, False);
		until (C < Max) or (IOResult <> 0);
		AddLines(S, True);
		Close(F);
	end;
end;

procedure Redraw;
var
	I : integer;
	P : PLine;
begin
	Bottom := Top;
	P := Top;
	I := 0; 
	while Assigned(P) and (I <= Hi(WindMax)) do begin 
		Bottom := P;
		Inc(I);
		GotoXY(1,I);
		FWrite(PtrStr(P^.Line)); 
		ClrEol;
		P := P^.Next;
	end;	
end;

procedure MoveUp(Count : integer);
begin
	while Count > 0 do begin
		if Top^.Prev = nil then exit;
		Top := Top^.Prev;
		Bottom := Bottom^.Prev;
		Dec(Count);
		GotoXY(1,1);
		InsLine;
		FWrite(PtrStr(Top^.Line)); 
		ClrEol;
	end;
end;

procedure MoveDown(Count : integer);
begin
	while Count > 0 do begin
		if Bottom^.Next = nil then exit;
		Top := Top^.Next;
		Bottom := Bottom^.Next;
		Dec(Count);
		GotoXY(1,1);
		DelLine;
		GotoXY(1,Hi(WindMax) +1);
		FWrite(PtrStr(Bottom^.Line)); 
		ClrEol;
	end;
end;
	
procedure Viewer;
var
	E : TEvent;
begin
	CheckScroll := False;
	CheckCursor := False;
	InitQCrtMaps;
	LoadFiles;
	ClrScr;
	Top := First;
	Redraw;
	repeat
		GotoXY(1,Hi(WindMax)+1);
		PurgeEvents;
		ClearEvent(E);
		While E.What = evNothing do GetEvent(E);
		if E.What = evCommand then
			Case E.Command of
				cmUp : begin
					ClearEvent(E);
					MoveUp(1);
				end;
				cmDown : begin
					ClearEvent(E);
					MoveDown(1);
				end;
				cmPageUp : begin
					ClearEvent(E);
					MoveUp(Hi(WindMax));
				end;
				cmPageDown : begin
					ClearEvent(E);
					MoveDown(Hi(WindMax));
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
