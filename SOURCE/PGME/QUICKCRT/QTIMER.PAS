{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

unit QTimer; { QuickCRT Timer Unit }
{$I QCRT.DEF}

interface

{$DEFINE INTERFACE}
uses QCrt;

	function ScheduleEvent(Event : TEvent; ms : longInt) : boolean;
	function ScheduleCommand(Command : word; Sender : Pointer; ms : longInt) : boolean;
	function ScheduleSystem(Command : word; Sender : Pointer; ms : longInt) : boolean;
	procedure ClearTimers;

{$UNDEF INTERFACE}

implementation

{$DEFINE IMPLEMENTATION}

type
	PTimerRec = ^TTimerRec;
	TTimerRec = record
		Prev, Next : PTimerRec;
		When : LongInt;
		Event : TEvent;
	end;

var
	OldIdleProc : procedure;
	OldExitProc : pointer;
	FirstTimer,
	LastTimer : PTimerRec;
	TimerTick : LongInt absolute $0040:$006c;

function ScheduleEvent(Event : TEvent; ms : longInt) : boolean;
var
	N, T : PTimerRec;
begin
	ScheduleEvent := False;
	if MaxAvail < Sizeof(TTimerRec) then Exit;
	New(N);
	N^.Next := nil;
	N^.Prev := nil;
	N^.Event := Event;
	N^.When := TimerTick + (ms div 55); { May overflow, I don't care! }
	if Not Assigned(FirstTimer) then begin
		FirstTimer := N;
		LastTimer := N;
	end else if FirstTimer^.When > N^.When then begin
		N^.Next := FirstTimer;
		N^.Next^.Prev := N;
		FirstTimer := N;
	end else if LastTimer^.When <= N^.When then begin
		N^.Prev := LastTimer;
		LastTimer^.Next := N;
		LastTimer := N;
	end else begin
		T := LastTimer;
		while Assigned(T) and (T^.When > N^.When) do T := T^.Prev;
		N^.Next := T^.Next;
		N^.Prev := T;
		T^.Next := N;
		N^.Next^.Prev := N;
	end;
	ScheduleEvent := True;
end;

function ScheduleCommand(Command : word; Sender : Pointer; ms : longInt) : boolean;
var
	Event : TEvent;
begin
	ClearEvent(Event);
	Event.What := evCommand;
	Event.Command := Command;
	Event.InfoPtr := Sender;
	ScheduleCommand := ScheduleEvent(Event, ms);
end;

function ScheduleSystem(Command : word; Sender : Pointer; ms : longInt) : boolean;
var
	Event : TEvent;
begin
	ClearEvent(Event);
	Event.What := evSystem;
	Event.Command := Command;
	Event.InfoPtr := Sender;
	ScheduleSystem := ScheduleEvent(Event, ms);
end;

procedure IdleTimerUnit; far;
var
	T : PTimerRec;
begin
	OldIdleProc;
	if Assigned(FirstTimer) then begin
		if TimerTick >= FirstTimer^.When then
			begin
				PutEvent(FirstTimer^.Event);
				T := FirstTimer^.Next;
				Dispose(FirstTimer);
				FirstTimer := T;
				if not Assigned(T) then
					LastTimer := T;
			end;
	end;
end;

procedure ClearTimers;
var
	T : PTimerRec;
begin
	While Assigned(FirstTimer) do begin
        T := FirstTimer^.Next;
        Dispose(FirstTimer);
        FirstTimer := T;
        if not Assigned(T) then
            LastTimer := T;
	end;
end;

procedure ExitTimerUnit; far;
var
	T : PTimerRec;
begin
	ExitProc := OldExitProc;
	IdleProc := OldIdleProc;
	While Assigned(FirstTimer) do begin
		T := FirstTimer^.Next;
		Dispose(FirstTimer);
		FirstTimer := T;
	end;
end;

procedure InitTimerUnit;
begin
	FirstTimer := nil;
	OldExitProc := ExitProc;
	OldIdleProc := IdleProc;
	ExitProc := @ExitTimerUnit;
	IdleProc := IdleTimerUnit;
end;

{$UNDEF IMPLEMENTATION}
begin
	InitTimerUnit;
end.
