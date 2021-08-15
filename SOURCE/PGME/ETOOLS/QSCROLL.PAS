{ Copyright 2015-2021 Jerome Shidel }

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

const
	Smooth : boolean = True;
	SmoothUp : boolean = True;
	MousePage = 10;
	const
	    ProgramUsage : string =
	        '   /S[-/+]     Force disable/enable of smooth scrolling'#13;

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
	J, K : integer;
	F : File;
	C : Word;
	S, T : String;
begin
	K := 0;
	for J := 1 to ParamCount do begin
		if (UCase(ParamStr(J)) = '/S') or (UCase(ParamStr(J)) = '/S+') then begin
			Smooth := True
		end else
		if UCase(ParamStr(J)) = '/S-' then begin
			Smooth := False;
		end else begin
			Assign(F, ParamStr(J));
			Reset(F, 1);
			S := #$0A;
			Inc(K);
			if K > 1 then begin
				AddLines(S, True);
				S := ' ' + CPad('>' + Copy(ParamStr(J), 1, 70) + '<', '=', 78)  + S;
				AddLines(S, True);
				S := #$0A;
				AddLines(S, True);

			end;
			S := '';
			if IOResult = 0 then begin
				repeat
					BlockRead(F, T[1], Max,  C);
					T[0] := Chr(C);
					S := S + FilterChar(T, #$0D);
					AddLines(S, False);
				until (C < Max) or (IOResult <> 0);
				AddLines(S, True);
			end else begin
			end;
			Close(F);
		end;
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

procedure ScrollUp( Stepping : integer );
var
	I : byte;
	P : Pointer;
begin
	if Smooth and SmoothUp then begin
		P := GetVideoPtr;
		ScreenMax := ScreenMax + $100;
		WindMax := ScreenMax;
		GotoXY(1,1);
		WaitSync;
		ScanStart(FontHeight-1);
	end;
	GotoXY(1,1);
	InsLine;
	FWrite(PtrStr(Top^.Line));
	ClrEol;
	if Smooth and SmoothUp then begin
		Stepping := 1 shl Stepping;
		I := 0;
		while (I < FontHeight) and (Stepping > 0) do begin
			Inc(I, Stepping);
			if I > FontHeight then I := FontHeight;
			WaitSync;
			ScanStart(FontHeight - I);
		end;
		GotoXY(1,1);
		ScreenMax := ScreenMax - $100;
	end else
		GotoXY(1,1);
end;

procedure ScrollDown( Stepping : integer );
var
	I : byte;
begin
	if Smooth then begin
		ScreenMax := ScreenMax + $100;
		WindMax := ScreenMax;
		GotoXY(1,Hi(ScreenMax) + 1);
		FWrite(PtrStr(Bottom^.Line));
		ClrEol;
		WaitSync;
		I := 0;
		Stepping := 1 shl Stepping;
		while (I < FontHeight) and (Stepping > 0) do begin
			Inc(I, Stepping);
			if I > FontHeight then I := FontHeight;
			WaitSync;
			ScanStart(I);
		end;
		GotoXY(1,1);
		ScanStart(0);
		DelLine;
		ScreenMax := ScreenMax - $100;
		WindMax := ScreenMax;
	end else begin
		GotoXY(1,1);
		DelLine;
		GotoXY(1,Hi(ScreenMax) + 1);
		FWrite(PtrStr(Bottom^.Line));
		ClrEol;
		GotoXY(1,1);
	end;
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
		if Top = nil then
			Count := 0
		else if Top^.Prev = nil then Count := 2;
		GetEvent(E);
		if ((E.What <> evNothing) or (Count > 2)) or (not DirectVideo) then begin
			if E.What <> evNothing then PutEvent(E);
			GotoXY(1,1);
			InsLine;
			FWrite(PtrStr(Top^.Line));
			ClrEol;
		end else
			ScrollUp(Count);
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
		if Bottom = nil then
			Count := 0
		else if Bottom^.Next = nil then Count := 1;
		GetEvent(E);
		if ((E.What <> evNothing) or (Count > 2)) or (not DirectVideo) then begin
			if E.What <> evNothing then PutEvent(E);
			GotoXY(1,1);
			DelLine;
			GotoXY(1,Hi(ScreenMax) +1);
			FWrite(PtrStr(Bottom^.Line));
			ClrEol;
		end else
			ScrollDown(Count);
	end;
end;

procedure Viewer;
var
	E : TEvent;
	Y, B : integer;
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
	if MouseAvail then SetMousePos(MouseHomeX, MouseHomeY);
	repeat
		GotoXY(1,Hi(ScreenMax)+1);
		ClearEvent(E);
		While E.What = evNothing do GetEvent(E);
		if E.What and evKeyboard <> evNothing then begin
			if UCase(E.CharCode) = 'S' then begin
				ClearEvent(E);
				Smooth := not Smooth;
			end;
			if Smooth and (UCase(E.CharCode) = 'U') then begin
				ClearEvent(E);
				SmoothUp := not SmoothUp;
			end;
		end else if E.What and evMouseMove <> evNothing then begin
			ClearEvent(E);
			B := E.Where.Y - MouseHomeY;
			SetMousePos(MouseHomeX, MouseHomeY);
			if ABS(B) > MousePage then begin
				if ABS(B) = B then
					MoveDown(Hi(ScreenMax))
				else
					MoveUp(Hi(ScreenMax));
			end else begin
				if ABS(B) = B then
					MoveDown(ABS(B))
				else
					MoveUp(ABS(B));
			end;
		end else if E.What = evCommand then
			case E.Command of
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
	until (E.What = evCommand) or (E.What = evKeyboard) or (E.What and evMouseDown <> evNothing);
	if DirectVideo and Smooth then begin
		ScreenMax := ScreenMax + $100;
		WindMax := ScreenMax;
		GotoXY(1,Hi(ScreenMax) + 1);
		ClrEol;
		ScreenMax := ScreenMax - $100;
		WindMax := ScreenMax;
	end;
    CheckCursor := True;
    GotoXY(1,Hi(ScreenMax)+1);
    ClrEol;
	NormalCursor;
end;

procedure ShowHelp;
var
	S : String;
begin
	FWriteLn('Usage: QSCROLL filename');
	S := #13 + ProgramUsage + #13;
	while S <> '' do
		FWriteLn(Space(4) + PullStr(#13, S));
	S := SoftwareLicense + #13 + #13;
	while S <> '' do
		FWriteLn(Space(4) + PullStr(#13, S));
end;

begin
    Smooth := IsDOSBox or (not IsVirtual);
    SmoothUp := Smooth;
	TurnMouseOff;
	if ParamCount = 0 then
		ShowHelp
	else
		Viewer;
end.

