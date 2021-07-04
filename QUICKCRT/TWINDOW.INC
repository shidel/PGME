{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TWindow --- }

{$IFDEF INTERFACE}
const
	class_TWindow = 'TWindow';
	
type			
	PWindow = ^TWindow;
	TWindow = object(TBox)
	private
		FCloseBtn : TButton;
		FTitle : TLabel;
		FCanClose : boolean;
		FLBrace, FRBrace : Char;
		FMainWindow : boolean;
		FStopModal : boolean;
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FAutoEndModal : boolean;
		FResult : integer;
		FStartTone, FEndTone : String;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure CheckMouse(var AEvent : TEvent); virtual;
		procedure SetModalResult(AValue : integer); virtual;
		procedure SetCurrentBounds(AValue : TBounds); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure SetTitle(AValue: String); virtual;
		function GetTitle : String; virtual;
		function GetCanClose : boolean; virtual;
		procedure SetCanClose(AValue : Boolean); virtual;
		procedure Draw; virtual;	
		function ShowModal : integer; virtual;	
		function GetModalResult : integer; virtual;	
		procedure SetMainWindow(AValue : boolean); virtual;
		function GetMainWindow : boolean; virtual;	
		procedure SetTextAttr(AValue : integer); virtual;
		procedure SetBackground(AValue : integer); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}

function TWindow.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TWindow) then 
		ObjectClass := class_TWindow
	else
		ObjectClass := inherited ObjectClass(AName);
end;

procedure TWindow.CheckLocal(var AEvent : TEvent); 
begin
	inherited CheckLocal(AEvent);
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmQuit : begin
				ClearEvent(AEvent);
				if FCanClose then begin
					Hide;
					FResult := mrCancel;
					PutCommand(cmQuit, @Self);
				end;
			end;
			cmCloseWindow : begin
				ClearEvent(AEvent);
				if FCanClose then begin
					Hide;
					if FMainWindow then begin
						AEvent.What := evCommand;
						AEvent.Command := cmQuit;
						PutEvent(AEvent);
						ClearEvent(AEvent);
					end;
				end;
			end;
			cmHelp : if Assigned(Application) then begin
				if Assigned(Application^.FHelp) then begin
					ClearEvent(AEvent);
					Application^.FHelp^.ShowHelp(@Self);
				end;
			end;
			cmEscape : if FAutoEndModal then begin
				Hide;
				ClearEvent(AEvent);
				if FMainWindow then begin
					AEvent.What := evCommand;
					AEvent.Command := cmQuit;
					PutEvent(AEvent);
					ClearEvent(AEvent);
				end;				
			end;
		end;
end;

procedure TWindow.SetModalResult(AValue : integer);
begin
	FResult := AValue;
end;

function TWindow.GetModalResult : integer; 
begin
	GetModalResult := FResult;
end;

constructor TWindow.Create(AParent : PControl; AName : String);
begin
	inherited Create(AParent, AName);
{	FPreserve := True;	}
	FCanClose := True;
	FMainWindow := False;
	FAutoEndModal := False;
	FCloseBtn.Create(nil, 'FRAME.BUTTON.CLOSE' );
	FCloseBtn.FAllowOutside := True;
	FCloseBtn.FCaption := #$FE; 
	FCloseBtn.FCaptionAsChar := True;
	FCloseBtn.SetTextColor($A);
	FCloseBtn.FCommand := cmCloseWindow;
	FCloseBtn.FFreeOnDestroy := False;
	AddChild(@FCloseBtn); 
	
	FTitle.Create(nil, 'FRAME.TITLE');
	FTitle.FAllowOutside := True;
	FTitle.FClearWindow := False;
	FTitle.SetTextColor($B);
	FTitle.FFreeOnDestroy := False;
	AddChild(@FTitle); 
	FResult := -1;
	FStartTone := '';
	FEndTone := '';
end;

destructor TWindow.Destroy;
begin
	FCloseBtn.Destroy;
	FTitle.Destroy;
	inherited Destroy;
end;

procedure TWindow.SetTitle(AValue : String);
begin
	if AValue = FTitle.FCaption then exit;
	if AValue <> '' then AValue := #$20 + AValue + #$20;
	FTitle.SetCaption(AValue);
end;

function TWindow.GetTitle : String;
var
	Temp : String;
begin
	Temp := FTitle.GetCaption;
	if Temp <> '' then
		Temp := Copy(Temp, 2, Length(Temp) - 2);
	GetTitle := Temp;
end;

procedure TWindow.Draw;
var
	WMin, WMax : word;
	I : integer;
	Temp : String;
begin
	inherited Draw;
	WMin := WindMin;
	WMax := WindMax;
	if FCloseBtn.FVisible then
		begin
			with FCloseBtn.FWindow do Window(Left - 1, Top, Left + 2, Top);
			if (FStyle = bxSingle) or (FStyle = bxDoubleSide) then
				FWrite(#$B4#$20#$C3)
			else
				FWrite(#$B5#$20#$C6);
		end;
	
{	Margins are already set. so no need to do them again!
	Window(Lo(WMin) + FMargin.Left + 1, Hi(WMin) + FMargin.Top + 1, 
		Lo(WMax) - FMargin.Right + 1, Hi(WMax) - FMargin.Bottom + 1);  }

	Window(Lo(WMin) + 1, Hi(WMin) + 1, Lo(WMax) + 1, Hi(WMax) + 1);  
end;

function TWindow.GetCanCLose : boolean;
begin
	GetCanClose := FCanClose;
end;

procedure TWindow.SetCanClose(AValue : Boolean); 
begin
	If AValue = FCanClose then exit;
	FCanClose := AValue;
{	FCloseBtn.SetVisible(AValue); }
	Update;			
end;

function TWindow.ShowModal : integer;
var
	Event : TEvent;
	{$IFDEF DEVLOG_EVENTS}
		TempStr : String;
	{$ENDIF}
begin
	{$IFDEF DEVLOG} 
		writeLog('Show Modal:' + GetPathID);
		LogInc;
	{$ENDIF}
	FVisible := True;
	AdjustSize;
	if Not FShown then Show;
	MouseVerify;	
	{$IFDEF DEVLOG}
		WriteLog('Start ShowModal Loop');
		LogInc;
	{$ENDIF}
	FStopModal := false;
	FResult := mrNone;
	if FStartTone <> '' then SpeakerPlay(FStartTone);
	repeat
		ClearEvent(Event);		
		while Event.What = evNothing do begin
			GetEvent(Event);			
			if (Event.What = evNothing) and Assigned(Application) then Application^.Idle;
		end;
		{$IFDEF DEVLOG_EVENTS}
		if Event.What and evMouse = evNone then begin
			TempStr := 'Event: ';
			case Event.What of
				evNone : TempStr := TempStr + 'evNone';
				evKeyboard :  begin
					TempStr := TempStr + 'evKeyboard ';
					TempStr := TempStr + ZPad(BinStr(Event.ShiftCode), 16) + ':' + 
						ZPad(HexStr(Event.KeyCode), 4);
					if Event.Original <> 0 then
						TempStr := TempStr + ' (' + ZPad(HexStr(Event.Original), 4) + ')';
						end;
				evCommand : begin
					TempStr	:= TempStr + ' evCommand ' + Application^.GetCommandName(Event.Command);
				end;
			end;		
			WriteLog(TempStr);
		end;
		{$ENDIF}			
		CheckEvent(Event);
	until (Not FShown) or (not FVisible) or FStopModal or 
	(Assigned(Application) and Application^.FTerminator); 
	if FEndTone <> '' then SpeakerPlay(FEndTone);
	if Assigned(Application) then Application^.FFocused := nil;

	{$IFDEF DEVLOG}
		LogDec;
		WriteLog('End ShowModal Loop');
	{$ENDIF}
	if GetVisible then Hide;
	{ Inform Any Parents that mouse has moved to blah, blah }
	ClearEvent(Event);
	MouseVerify;
	{ Hide; }
	{$IFDEF DEVLOG} 
		LogDec;
		writeLog('End Modal:' + GetPathID + ' (' + IntStr(FResult) + ')');
	{$ENDIF}
	ShowModal := FResult;
	FVisible := False;
end;

procedure TWindow.SetCurrentBounds(AValue : TBounds); 
begin
	inherited SetCurrentBounds(AValue);
	Bounds(4 - FMargin.Left, 1-FMargin.Top, 1, 1, AValue);
	FCloseBtn.SetBounds(AValue);
	if FCloseBtn.FVisible then
		Bounds( 6 - FMargin.Left, 1-FMargin.Top, Width - 8, 1, AValue)
	else
		Bounds( 4 - FMargin.Left, 1-FMargin.Top, Width - 8, 1, AValue);
	FTitle.SetBounds(AValue);
end;

procedure TWindow.SetMainWindow(AValue : boolean);
begin
	FMainWindow := AValue;
end;

function TWindow.GetMainWindow : boolean;
begin
	GetMainWindow := FMainWindow;
end;

procedure TWindow.SetTextAttr(AValue : integer);
begin
	inherited SetTextAttr(AValue);
	FAttr.Click := FAttr.Normal;
end;

procedure TWindow.SetBackground(AValue : integer);
begin
	inherited SetBackground(AValue);
	FCloseBtn.SetBackground(AValue);
end;

procedure TWindow.CheckMouse(var AEvent : TEvent); 
var
	Hold : TEvent;
begin
	Hold := AEvent;
	inherited CheckMouse(AEvent);
	if not GetVisible then exit;
	if not GetShown then exit;
	if (Not FMouseOver) and (FAutoEndModal) and (Hold.What = evMouseDown) then
		FStopModal := True;	
end;

procedure TWindow.ApplyTheme( AConfig : PConfigFile );
begin
	inherited ApplyTheme(AConfig);
	FStartTone := AConfig^.GetValue('TONE.START', FStartTone);	
	FEndTone := AConfig^.GetValue('TONE.END', FEndTone);	
end;

{$ENDIF}
