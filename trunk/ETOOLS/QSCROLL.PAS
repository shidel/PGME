{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Text_Scroller; { X Series }

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
	Max := Lo(ScreenMax) + 1;
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
	while Assigned(P) and (I <= Hi(ScreenMax)) do begin 
		Bottom := P;
		Inc(I);
		GotoXY(1,I);
		FWrite(PtrStr(P^.Line)); 
		ClrEol;
		P := P^.Next;
	end;	
end;

procedure WaitSync; 
begin
	while Port[$3da] and $8 <> $8 do;
	while Port[$3da] and $8 = $8 do;
end;

procedure ScanStart(Line : Byte);
begin
	Port[$3d4] := $08;
	Port[$3d5] := Line;
end;

procedure ScrollUp;
var
	I : byte;
	P : Pointer;
begin
	P := GetVideoPtr;
	ScreenMax := ScreenMax + $100;
	WindMax := ScreenMax;
	GotoXY(1,1);
	WaitSync;
	ScanStart(FontHeight);
	GotoXY(1,1);
	InsLine;
	FWrite(PtrStr(Top^.Line));
	ClrEol;
	for I := 0 to FontHeight do begin
		ScanStart(FontHeight - I);
		WaitSync;
	end;
	GotoXY(1,1);
	ScreenMax := ScreenMax - $100;
end;

procedure ScrollDown;
var
	I : byte;
begin
	ScreenMax := ScreenMax + $100;
	WindMax := ScreenMax;
	GotoXY(1,Hi(ScreenMax) + 1);
	FWrite(PtrStr(Bottom^.Line));
	ClrEol;
	WaitSync;
	for I := 0 to FontHeight do begin
		ScanStart(I);
		WaitSync;
	end;
	GotoXY(1,1);
	ScanStart(0);
	DelLine;
	ScreenMax := ScreenMax - $100;
	WindMax := ScreenMax;
end;

procedure MoveUp(Count : integer);
var
	E : TEvent;
begin
	while Count > 0 do begin
		if Top^.Prev = nil then exit;
		Top := Top^.Prev;
		Bottom := Bottom^.Prev;
		Dec(Count);
		GetEvent(E);
		if ((E.What <> evNothing) or (Count > 0)) and DirectVideo then begin
			if E.What <> evNothing then PutEvent(E);
			GotoXY(1,1);
			InsLine;
			FWrite(PtrStr(Top^.Line)); 
			ClrEol;
		end else
			ScrollUp;
	end;
end;

procedure MoveDown(Count : integer);
var
	E : TEvent;
begin
	while Count > 0 do begin
		if Bottom^.Next = nil then exit;
		Top := Top^.Next;
		Bottom := Bottom^.Next;
		Dec(Count);
		GetEvent(E);
		if ((E.What <> evNothing) or (Count > 0)) and DirectVideo then begin
			if E.What <> evNothing then PutEvent(E);
			GotoXY(1,1);
			DelLine;
			GotoXY(1,Hi(ScreenMax) +1);
			FWrite(PtrStr(Bottom^.Line)); 
			ClrEol;
		end else
			ScrollDown;
	end;
end;
	
procedure Viewer;
var
	E : TEvent;
begin
	HideCursor;
	CheckScroll := False;
	CheckCursor := False;
	InitQCrtMaps;
	LoadFiles;
	ClrScr;
	Top := First;
	Redraw;
	PurgeEvents;
	repeat
		GotoXY(1,Hi(ScreenMax)+1);
		ClearEvent(E);
		While E.What = evNothing do GetEvent(E);
		if E.What = evCommand then
			Case E.Command of
				cmUp : begin
					PurgeEvents;
					ClearEvent(E);
					MoveUp(1);
				end;
				cmDown : begin
					PurgeEvents;
					ClearEvent(E);
					MoveDown(1);
				end;
				cmPageUp : begin
					ClearEvent(E);
					MoveUp(Hi(ScreenMax));
				end;
				cmPageDown : begin
					ClearEvent(E);
					MoveDown(Hi(ScreenMax));
				end;
			end;
	until (E.What = evCommand) or (E.What = evKeyboard);
	CheckCursor := True;
	GotoXY(1,Hi(ScreenMax)+1);		
	NormalCursor;
end;

procedure ShowHelp;
var
	S : String;
begin
	FWriteLn('Usage: QSCROLL filename');
	FWriteLn('');
	S := SoftwareLicense;
	while S <> '' do 
		FWriteLn(Space(4) + PullStr(#13, S));
end;

begin
	TurnMouseOff;
	if ParamCount = 0 then
		ShowHelp
	else 
		Viewer;
end.
