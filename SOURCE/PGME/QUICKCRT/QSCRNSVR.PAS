{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

unit QScrnSvr; { QuickCrt Screen Saver }
{$I QCRT.DEF}

interface

{$IFDEF TEMPLATES}
	uses QClass, QObjects, QCrt, QInfo;
{$ELSE}
	uses QClass, QCrt, QInfo;

	const
		ftESaver = 'ESAVER';
{$ENDIF}

const
	class_TScreenSaver : TObjectClass = 'TScreenSaver';

type
	PScreenSaver = ^TScreenSaver;
	TScreenSaver = object(TObject)
	public { protected }
		Info : TInformation;
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
		procedure Refresh; virtual;
		procedure Show; virtual;
		procedure Hide; virtual;
		procedure Execute; virtual;
		procedure Configure; virtual;
		procedure CheckEvent(var Event : TEvent); virtual;
	end;

implementation

const
	TickCount : integer = 0;
	LastTick : longint = 0;

var
	TimerTick : LongInt absolute $0040:$006c;


function TScreenSaver.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TScreenSaver) then
		ObjectClass := class_TScreenSaver
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TScreenSaver.Create;
begin
	Info.Create(ftESaver);
end;

destructor TScreenSaver.Destroy;
begin
	Info.Destroy;
end;

procedure TScreenSaver.Execute;
var
	Size : integer;
	Screen, Temp : Pointer;
	Event : TEvent;
	QCrtSettings : TQCrtSettings;
begin
	Configure;
	{$IFNDEF TEMPLATES}
	Screen := nil;
	GetQCrtState(QCrtSettings);
	Size := GetVideoSize;
	Temp := GetVideoPtr;
	if Size > MaxAvail then Halt(8);
	GetMem(Screen, Size);
	SetVideoPtr(Screen);
	CopyFromVideo;
	SetVideoPtr(Temp);

	TurnMouseOff;
	CheckScroll := False;
	HideCursor;

	Show;
	repeat
		ClearEvent(Event);
		GetEvent(Event);
		Refresh;
		if (TickCount < 18) and (LastTick <> TimerTick) then begin
			PurgeEvents;
			Inc(TickCount);
			LastTick := TimerTick;
		end;
		CheckEvent(Event);
	until (Event.What <> evNothing) and (TickCount >= 18);
	Hide;

	SetQCrtState(QCrtSettings);
	if Assigned(Screen) then
		begin
			Temp := GetVideoPtr;
			SetVideoPtr(Screen);
			CopyToVideo;
			SetVideoPtr(Temp);
			FreeMem(Screen, Size);
		end;
	{$ENDIF}
	PurgeEvents;
end;

procedure TScreenSaver.Refresh;
begin
end;

procedure TScreenSaver.Configure;
begin
end;

procedure TScreenSaver.Show;
begin
	TextAttr := $07;
	ClrScr;
end;

procedure TScreenSaver.Hide;
begin
end;

procedure TScreenSaver.CheckEvent(var Event : TEvent);
begin
end;

end.

