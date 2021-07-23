{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QSpeaker; { QuickCRT Timer Unit }
{$I QCRT.DEF}

interface

{$DEFINE INTERFACE}	
	
	procedure SpeakerPlay(ASong : String);
	procedure SpeakerNext;
	function SpeakerPlaying : boolean;
	procedure SpeakerOn;
	procedure SpeakerOff;
	function SpeakerState : boolean;

{$UNDEF INTERFACE}

implementation

uses QCrt, QTimer, QStrings;

var
	OldIdleProc : procedure; 
	OldExitProc : pointer;
	CSong : String;
	State : boolean;
	Playing : boolean;

{$DEFINE IMPLEMENTATION}
function SpeakerPlaying : boolean;
begin
	SpeakerPlaying := (Playing) and (State);
end;

function SpeakerState : boolean;
begin
	SpeakerState := State;
end;

procedure SpeakerOn;
begin
	State := True;
	if CSong <> '' then begin
		PutSystem(cmSpeaker, nil);
		Playing := True;
	end;
end;

procedure SpeakerOff;
begin
	if Playing then NoSound;
	State := False;
	Playing := False;
end;

procedure SpeakerPlay(ASong : String);
begin
	NoSound;
	CSong := ASong;
	PutSystem(cmSpeaker, nil);
	Playing := True;
end;
	
procedure SpeakerNext;
var
 	Note, Time : String;
begin
	Playing := False;
	if (CSong = '') or (not State) then begin
		NoSound;
		Exit;
	end;
	Time := trim(PullStr(',', CSong));
	Note := trim(PullStr(':', Time));
	if (Note = '') or (Time = '') then begin
		NoSound;
		Exit;
	end;
	if StrInt(Note) = 0 then NoSound else Sound(StrInt(Note));
	ScheduleSystem(cmSpeaker, nil, StrInt(Time) * 55);
	Playing := True;
end;

procedure ExitSpeakerUnit; far;
begin
	ExitProc := OldExitProc;	
	NoSound;
end;

procedure InitSpeakerUnit;
begin
	OldExitProc := ExitProc;
	ExitProc := @ExitSpeakerUnit;
	CSong := '';
	Playing := False;
	State := true;
end;

{$UNDEF IMPLEMENTATION}
begin
	InitSpeakerUnit;
end.
