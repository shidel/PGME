{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TControl --- }

{$IFDEF INTERFACE}
const
	class_TControl = 'TControl';
	
type
	PControl = ^TControl;
	{$IFDEF EventProcs}
	TControlProc = procedure (AVisual : PControl; var AEvent : TEvent); { of Object BooHoo. } 
	{$ENDIF}
	
	TControl = object (TListItem)
	private { private }
		FBehind : Pointer; { Saved Screen Pointer }
		FBSize : integer;  { Size of Saved Screen }
    public { protected }
    	FPreserve : boolean;
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FWindow : TBounds; 
		FPBounds : TBounds; { Parents Last Known Bounds }
		FTextAlign : boolean; { Flag if FHAlign and FVAlign are used }

		FAllowOutside : boolean;
		FCBounds,
		FRBounds : TBounds;
		FMargin : TMargins;
		FAttr : TTextAttrs;
		
		FAnchors : word;
		
		FHAlign : THorizontalAlign;
		FVAlign : TVerticalAlign;
		
		FShown : boolean;
		FVisible : boolean;
		FMouseOver : boolean;

		FClicking : boolean;
		FEnabled : boolean;
		FTabStop : boolean;
		
		FKeyValue : TKeyValue;
		FKeyFlags : word;
		FTrigger, 	{ Command that is sent automatically when triggered, if FCommand is cmNone }
		FCommand,   { FTrigger override to this command }
		FRespond  : word; { Tells Control to respond to this Command like it was triggered }
		
{		FPreserve : boolean;}
		FClearWindow : boolean;
		FAutoSize : boolean;
		
		FUpdating : boolean;
		
		FHelpList : TListItem;

		procedure UpdateWindow(AValue : boolean); virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure CheckMouse(var AEvent : TEvent); virtual;
		function  CheckInside(X, Y : integer) : boolean; virtual;
		procedure BeforeUpdate; virtual;
		procedure AfterUpdate; virtual;
		procedure AdjustSize; virtual;
		procedure CalcBounds(var AValue : TBounds); virtual;
		procedure GetMaxBounds(var AValue : TBounds); virtual;
		procedure CalcWindow(var AValue : TBounds); virtual;
		procedure SendCommand; virtual;
		procedure MouseVerify; virtual;
		procedure TriggerHook; virtual;

		function CheckEvent(var AEvent : TEvent) : PControl; virtual;

		procedure SetAllowedOutside(AValue : boolean); virtual;
		procedure SetCurrentBounds(AValue : TBounds); virtual;
		
		procedure BeforeShow; virtual;
		procedure AfterHide; virtual;
		procedure DoAutoSize; virtual;
		
		procedure BeginUpdate; virtual;
		procedure EndUpdate; virtual;

	public { public }
		{$IFDEF EventProcs}
		OnMouseOver,
		OnMouseClick,
		OnMouseRelease, 
		OnKeyboard,
		OnRespond, 
		OnTriggered : TControlProc;
		{$ENDIF}

	{ published }	
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		function  Left : integer; 
		function  Top : integer;
		function  Width : integer;
		function  Height : integer;
		procedure AddChild(AVisual : PControl);		
		procedure RemoveChild(AVisual : PControl);
		function  GetVisible : boolean; virtual;
		procedure SetVisible(AValue : boolean); virtual;
		function  IsFocused : boolean; virtual;
		function  GetEnabled : boolean; virtual;
		function  GetTabStop : boolean; virtual;
		function  GetAutoSize : boolean; virtual;
		procedure SetAutoSize (AValue : boolean); virtual;
		procedure SetEnabled(AValue : boolean); virtual;
		procedure SetTabStop(AValue : Boolean); virtual;
		procedure SetBounds(AValue : TBounds); virtual;
		procedure GetBounds(var AValue : TBounds); virtual;
		procedure SetMargins(AValue : TMargins);  virtual;
		procedure GetMargins(var AValue : TMargins); virtual;
		procedure SetPosition(X, Y : integer); virtual;
		procedure Show; virtual;
		procedure Hide; virtual;
		procedure Draw; virtual;
		procedure Update; virtual;
		procedure Refresh; virtual;
		procedure SetAnchors(AValue : word); virtual;
		function  GetAnchors(AValue : integer) : word; virtual;
		procedure SetTextAttr(AValue : integer); virtual;
		procedure SetTextColor(AValue : integer); virtual;
		procedure SetBackground(AValue : integer); virtual;
		procedure SetCommand(AValue : word); virtual;
		function  GetCommand : word; virtual;
		procedure SetRespond(AValue : word); virtual;
		function  GetRespond : word; virtual;
		procedure GetKeyCode(var AValue : TKeyValue; var AFlags : word); virtual;
		procedure SetKeyCode(AKeyCode, AShiftCode, AFlags : word); virtual;
		function GetAllowedOutside : boolean; virtual;
		procedure ApplyConfig( AConfig : PConfigFile ); virtual;
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		procedure ApplyKeyboard( AConfig : PConfigFile ); virtual;
		function GetShown : boolean; virtual;
		procedure ClearHelpText; virtual;
		procedure SetHelpText ( AValue : String ); virtual;
		procedure AddHelpText ( AValue : String ); virtual;
		function GetHelpText : String; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TControl.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TControl) then 
		ObjectClass := class_TControl
	else
		ObjectClass := inherited ObjectClass(AName);
end;

function TControl.GetShown : boolean;
begin
	GetShown := FShown;
end;

procedure TControl.MouseVerify;
var
    Event : TEvent;
	X, Y, B : integer;
begin
	if Assigned(Application) then Application^.FFocused := nil;
	ClearEvent(Event);
	Event.What := evMouseMove;
	Event.Double := false;
	GetMousePos(X, Y, B);
	Event.Where.X := X;
	Event.Where.Y := Y;
	Event.Buttons := B;
	PutEvent(Event);
end;

constructor TControl.Create;
begin
	inherited Create(AName);
	FClearWindow := True;
	FPreserve := ClassOf(class_TPanel) or true;
	FUpdating := False;
{	FPreserve := False;}
	FHelpList.Create('HELP');
	FAllowOutside := False;
	FWindow.Left := 0;
	FWindow.Top := 0;
	FWindow.Width := 0;
	FWindow.Height := 0;
	FCBounds := FWindow;
	FRBounds := FCBounds;
	FMargin.Left := 0;
	FMargin.Top := 0;
	FMargin.Right:= 0;
	FMargin.Bottom := 0;
	FShown := False;
	FVisible := True;
	FBehind := nil;
	FAnchors := abLeft or abTop or alNone;	
	FHAlign := AlignCenter;
	FVAlign := AlignMiddle;
	FEnabled := True;
	FTabStop := False;
	FMouseOver := False;
	FClicking := False;
	FillChar(FKeyValue, Sizeof(FKeyValue), 0);
	FKeyFlags := $0001;
	FCommand := cmNone;
	FRespond := cmNone;
	FTrigger := cmNone;
	{$IFDEF EventProcs}
		OnMouseOver := nil;
		OnMouseClick := nil;
		OnMouseRelease := nil;
		OnKeyboard := nil;
		OnRespond := nil;
		OnTriggered := nil;
	{$ENDIF}
	FAutoSize := false;
	if Assigned(AParent) then AParent^.AddChild(@Self);
	FTextAlign := False;
	SetTextAttr(TextAttr);
end;

destructor TControl.Destroy;
begin
	if FShown then Hide;
	if Assigned(FBehind) then begin
		FreeMem(FBehind, FBSize);
		FBehind := nil;
		FBSize := 0;
	end;
	FHelpList.Clear;
	FHelpList.Destroy;
	inherited Destroy;
end;

procedure TControl.AddChild (AVisual : PControl);
begin
	if not Assigned(AVisual) then exit;
	Add(AVisual);
	AVisual^.FPBounds := FCBounds;
end;

procedure TControl.RemoveChild(AVisual : PControl);
begin
	if not Assigned(AVisual) then exit;
	if (AVisual^.Parent <> @Self) then Halt(erInvalid_Pointer_Operation); 
	if AVisual^.FShown then AVisual^.Hide;
	Remove(AVisual);
end;

function  TControl.GetVisible : boolean; 
begin
	GetVisible := FVisible;
end;

procedure TControl.SetVisible(AValue : boolean); 
begin
	if FVisible = AValue then exit;
{	if AValue then Show;
	if Not AValue then Hide; }
	FVisible := AValue;
	Update; 
end;

function  TControl.IsFocused : boolean; 
begin
	if Assigned(Application) then
		IsFocused := Application^.FFocused = @Self
	else
		IsFocused := False;
end;

function  TControl.GetEnabled : boolean; 
begin
	GetEnabled := FEnabled;
end;

function  TControl.GetTabStop : boolean; 
begin
	GetTabStop := FTabStop;
end;

function  TControl.GetAutoSize : boolean; 
begin
	GetAutoSize := FAutoSize;
end;

procedure TControl.SetAutoSize (AValue : boolean);
begin
	if FAutoSize <> AValue then exit;
	FAutoSize := AValue;
	Update;
end;

procedure TControl.SetEnabled(AValue : boolean); 
begin
	if FEnabled = AValue then exit;
	FEnabled := AValue;
	Update;
end;

procedure TControl.SetTabStop(AValue : Boolean); 
begin
	if FTabStop = AValue then exit;
	FTabStop := AValue;
end;

function TControl.Left : integer; 
begin
	Left := FCBounds.Left;
end;

function TControl.Top : integer;
begin
	Top := FCBounds.Top;
end;

function TControl.Width : integer;
begin
	Width := FCBounds.Width;
end;

function TControl.Height : integer;
begin
	Height := FCBounds.Height;
end;

function TControl.CheckInside(X, Y : integer) : boolean;
begin
	CheckInside := (X >= FWindow.Left) and (Y >= FWindow.Top) and 
	  (X < FWindow.Left + FWindow.Width) and (Y < FWindow.Top + FWindow.Height);	
end;

procedure TControl.BeforeUpdate; 
begin
	QObjects.StartDrawing;
end;

procedure TControl.AfterUpdate; 
begin
	QObjects.FinishDrawing;
end;

procedure TControl.CalcBounds(var AValue : TBounds);
var
	Max : TBounds;	
begin	
	GetMaxBounds(Max);
		
	{ Adjust Alignment }
	if (FAnchors and alClient = alClient) then begin
		AValue := Max;
	end else if (FAnchors and alClient) <> 0 then begin
		if (FAnchors and (alLeft or alRight)) = (alLeft or alRight) then begin
			AValue.Left := 1;
			AValue.Width := Max.Width;
		end else if (FAnchors and alLeft) = alLeft then begin
			AValue.Left := 1;
		end else if (FAnchors and alRight) = alRight then begin
			AValue.Left := 1 + Max.Width - AValue.Width;
		end;
		if (FAnchors and (alTop or alBottom)) = (alTop or alBottom) then begin
			AValue.Top := 1;
			AValue.Height := Max.Height;
		end else if (FAnchors and alTop) = alTop then begin
			AValue.Top := 1;
		end else if (FAnchors and alBottom) = alBottom then begin
			AValue.Top := 1 + Max.Height - AValue.Height;
		end
	end;

 	if FAllowOutside then exit;
						
	{ Crop to Maximum Values }
	if AValue.Left < Max.Left then 
		AValue.Left := Max.Left;
	if AValue.Top < Max.Top then 
		AValue.Top := Max.Top;
	if AValue.Left + AValue.Width > Max.Left + Max.Width then
		AValue.Width := (Max.Left + Max.Width) - AValue.Left;
	if AValue.Top + AValue.Height >= Max.Top + Max.Height then 
		AValue.Height := (Max.Top + Max.Height) - AValue.Top;
	if AValue.Width < 0 then 
		AValue.Width := 0;
	if AValue.Height < 0 then 
		AValue.Height := 0;
end;

procedure TControl.GetMaxBounds( var AValue : TBounds );
var
	WMX, WMY : integer;
begin
	WMX := Lo(WindMax);
	WMY := Hi(WindMax);
	{ Determine Max Bounds either By Screen if no Parent or By Parent with Margins }
	if Assigned(Parent) then begin
		AValue.Left := 1;
		AValue.Top := 1;
		AValue.Width := 
			PControl(Parent)^.FCBounds.Width - 
			(PControl(Parent)^.FMargin.Left + PControl(Parent)^.FMargin.Right);
		AValue.Height := PControl(Parent)^.FCBounds.Height - 
			(PControl(Parent)^.FMargin.Top + PControl(Parent)^.FMargin.Bottom);
		if FAllowOutside then begin 
			AValue.Left := 1 - AValue.Left;
			AValue.Top := 1 - AValue.Top;
			AValue.Width := WMX + 1;
			AValue.Height := WMY + 1;
		end;
	end else
		Bounds(1,1, WMX + 1, WMY + 1, AValue);
	{$IFDEF DEVLOG_BOUNDS}
	with AValue do WriteLog('Maximum Bounds - ' + IntStr(Left) + ':' + IntStr(Top) + ',' + 
		IntStr(Width) + ':' + IntStr(Height));  
	{$ENDIF}
end;

procedure TControl.CalcWindow(var AValue : TBounds);
begin
	if Assigned(Parent) then begin
		AValue.Left := PControl(Parent)^.FWindow.Left + PControl(Parent)^.FMargin.Left +
			AValue.Left - 1;
		AValue.Top := PControl(Parent)^.FWindow.Top + PControl(Parent)^.FMargin.Top + 
			AValue.Top - 1;
	end;
end;

procedure TControl.SetCurrentBounds(AValue : TBounds);
var
	ReShow : boolean;
	P : PControl;
begin
	{$IFDEF DEVLOG_BOUNDS}
	with AValue do WriteLog('Request Bounds of ' + Self.GetNameID + '> ' +
		 IntStr(Left) + ':' + IntStr(Top) + ',' + IntStr(Width) + ':' + IntStr(Height) +
		 '  OUTSIDE:' + BoolStr(FAllowOutside) + ', ANCHOR: ' + BinStr(FAnchors));  
	LogInc;
	{$ENDIF}
	ReShow := FShown;
	if ReShow then Hide;

	CalcBounds(AValue);

	FCBounds := AValue;	
	{$IFDEF DEVLOG_BOUNDS}
	with AValue do WriteLog('Apply to Bounds of ' + Self.GetNameID + '> ' +
		 IntStr(Left) + ':' + IntStr(Top) + ',' + IntStr(Width) + ':' + IntStr(Height));  
	{$ENDIF}

	CalcWindow(AValue);
	FWindow := AValue;
		
	{$IFDEF DEVLOG_BOUNDS}
	with FWindow do WriteLog('Apply to Window of ' + Self.GetNameID + '> ' +
		 IntStr(Left) + ':' + IntStr(Top) + ',' + IntStr(Width) + ':' + IntStr(Height));  
	{$ENDIF}

	P := PControl(First);
	while Assigned(P) do
		begin
			P^.AdjustSize;
			P := PControl(P^.Next);
		end;
	if ReShow then Show;
	{$IFDEF DEVLOG_BOUNDS}
	LogDec;
	{$ENDIF}
end;

procedure TControl.SetBounds(AValue : TBounds);
begin
	StartDrawing;
	FRBounds := AValue;
	if Assigned(Parent) then 
		PControl(Parent)^.GetBounds(FPBounds);
	SetCurrentBounds(FRBounds);
	FinishDrawing;
end;

procedure TControl.AdjustSize; 
var
	Temp, Ofs : TBounds;
begin
	Temp := FRBounds;
	{$IFDEF DEVLOG_ADJUST}
	WriteLog('AdjustSize of ' + Self.GetNameID );
	LogInc;
	with Temp do WriteLog('Previous ' + Self.GetNameID + '> ' +
		 IntStr(Left) + ':' + IntStr(Top) + ',' + IntStr(Width) + ':' + IntStr(Height));  
	{$ENDIF}
	if Assigned(Parent) then begin
		Ofs := PControl(Parent)^.FCBounds;
		Dec(Ofs.Top, FPBounds.Top);
		Dec(Ofs.Left, FPBounds.Left);
		Dec(Ofs.Width, FPBounds.Width);
		Dec(Ofs.Height, FPBounds.Height);
		{$IFDEF DEVLOG_ADJUST}
		with Ofs do WriteLog('Deviant ' + Self.GetNameID + '> ' +
			 IntStr(Left) + ':' + IntStr(Top) + '/' + IntStr(Width) + ':' + IntStr(Height));  
		{$ENDIF}
		
		if FAnchors and abAll <> 0 then begin
			if FAnchors and (abLeft or abRight) = (abLeft or abRight) then begin
				Temp.Width := Temp.Width + Ofs.Width;
			end else
			if FAnchors and abRight = abRight then begin
				Temp.Left := Temp.Left + Ofs.Width;
				if FAnchors and alLeft = alLeft then begin
					{ Stuff }
				end;
			end else
			if FAnchors and abLeft = abLeft then begin
				if FAnchors and alRight = alRight then begin
					Temp.Left := Temp.Left + Ofs.Width;
				end;
			end;
			
			if FAnchors and (abTop or abBottom) = (abTop or abBottom) then begin
				Temp.Height := Temp.Height + Ofs.Height;
			end else
			if FAnchors and abBottom = abBottom then begin
				Temp.Top := Temp.Top + Ofs.Height;
				if FAnchors and alTop = alTop then begin
					{ Stuff }
				end;
			end else
			if FAnchors and abTop = abTop then begin
				if FAnchors and alBottom = alBottom then begin
					Temp.Height := Temp.Height + Ofs.Height;
				end;
			end;
		end;
		{FPBounds := New; }
	end;
	{$IFDEF DEVLOG_ADJUST}
	with Temp do WriteLog('Suggest ' + Self.GetNameID + '> ' +
		 IntStr(Left) + ':' + IntStr(Top) + ',' + IntStr(Width) + ':' + IntStr(Height));  
	{$ENDIF}
	SetCurrentBounds(Temp);
	{$IFDEF DEVLOG_ADJUST}
		LogDec;
	with FCBounds do WriteLog('Actual ' + Self.GetNameID + '> ' +
		 IntStr(Left) + ':' + IntStr(Top) + ',' + IntStr(Width) + ':' + IntStr(Height));  
	{$ENDIF}
end;

procedure TControl.GetBounds(var AValue : TBounds);
begin
	AValue := FCBounds;
end;

procedure TControl.SetMargins(AValue : TMargins); 
begin
	FMargin.Left := AValue.Left;
	FMargin.Right := AValue.Right;
	FMargin.Top := AValue.Top;
	FMargin.Bottom := AValue.Bottom;
end; 

procedure TControl.GetMargins(var AValue : TMargins);
begin
	AValue.Left := FMargin.Left;
	AValue.Top := FMargin.Top;
	AValue.Right := FMargin.Right;
	AValue.Bottom := FMargin.Bottom;
end; 

procedure TControl.SetPosition(X, Y : integer);
var
	Temp : TBounds;
begin
	{$IFDEF DEVLOG_BOUNDS}
		WriteLog('SetPosition of ' + GetPathID + ' to ' + IntStr(X) +':' + IntStr(Y) );
	{$ENDIF}
	Temp.Left := X;
	Temp.Top := Y;
	Temp.Width := Width;
	Temp.Height := Height;
	SetCurrentBounds(Temp);
end;

procedure TControl.Show;
var	
	HMin, HMax : word;
	HAttr, HX, HY : integer;
	P : PControl;
begin
	if FShown then exit;
	if Not FVisible then exit;
	if (FWindow.Left < 1) then exit;
	if (FWindow.Top < 1) then exit;
	if (FWindow.Width < 1) then exit;
	if (FWindow.Height < 1) then exit;
	BeforeShow;
	StartDrawing;
	if FPreserve then begin
		if not Assigned(FBehind) then 
			begin
				FBSize := WindowSize(FWindow.Left, FWindow.Top, FWindow.Left + 
					FWindow.Width - 1, FWindow.Top + FWindow.Height - 1);
				if not MemCheck(FBSize) then
						Halt(erInsufficient_Memory); 
				Window(1, 1, Lo(ScreenMax) + 1, Hi(ScreenMax) + 1);
				GetMem(FBehind, FBSize);
				GetWindow(FWindow.Left, FWindow.Top, FWindow.Left + FWindow.Width - 1, 
				FWindow.Top + FWindow.Height - 1, FBehind^);
				FShown := True;
				UpdateWindow(True);
			end;
	end else begin
		Window(1, 1, Lo(ScreenMax) + 1, Hi(ScreenMax) + 1);
		FShown := True;
		UpdateWindow(True);
	end; 
{	P := PControl(First);
	while Assigned(P) do begin
		P^.Show;
		P := PControl(P^.Next);
	end; }
	FinishDrawing;
end;

procedure TControl.Hide;
var
	P : PControl;
begin
	if not FShown then exit;
	StartDrawing;
	P := PControl(First);
	while Assigned(P) do begin
		P^.Hide;
		P := PControl(P^.Next);
	end;
	if Assigned(FBehind) then 
		begin
			Window(1, 1, Lo(ScreenMax) + 1, Hi(ScreenMax) + 1);
			PutWindow(FWindow.Left, FWindow.Top, FBehind^);
			FreeMem(FBehind, FBSize );
			FBehind := nil;
			FBSize := 0;
			FShown := False;
		end;
	FinishDrawing;
	AfterHide;
end;

procedure TControl.BeforeShow;
begin
end;

procedure TControl.AfterHide;
begin
end;

procedure TControl.Draw;
begin
{$IFDEF DEVLOG_DRAW}
	WriteLog('DRAW (' + GetClassID + ') ' + GetPathID + '> ' +
	IntStr(Left) + ',' + IntStr(Top) + ',' + IntStr(Width) + ',' + + IntStr(Height) ); 
{$ENDIF}
end;

procedure TControl.UpdateWindow(AValue : boolean);
var	
	P : PControl;
begin
	if FUpdating then exit;
	if Not FShown then exit;
	StartDrawing;
	TextAttr := FAttr.Normal;
	if IsFocused 	then TextAttr := FAttr.Focused;
	if FMouseOver   then TextAttr := FAttr.Hover;
	if FClicking 	then TextAttr := FAttr.Click;
	if Not FEnabled then TextAttr := FAttr.Disabled;
		
	{$IFDEF DEVLOG_BOUNDS}
	with FWindow do WriteLog('Set Window ' + Self.GetNameID + '> ' +
		 IntStr(Left) + ':' + IntStr(Top) + ',' + IntStr(Width) + ':' + IntStr(Height));  
	{$ENDIF}
	Window(FWindow.Left, FWindow.Top, FWindow.Left + FWindow.Width - 1, FWindow.Top + 
	FWindow.Height - 1);
	
	if AValue and FClearWindow then ClrScr;
	Draw;
	P := PControl(First);
	while Assigned(P) do begin
		if P^.FShown then
			P^.UpdateWindow(AValue)
		else
			P^.Show;
		P := PControl(P^.Next);
	end;
	
	FinishDrawing;
end;

procedure TControl.Update;
begin
	UpdateWindow(True);
end;

procedure TControl.Refresh;
begin
	UpdateWindow(False);
end;

procedure TControl.SetTextAttr(AValue : integer);
begin
	FAttr.Normal := AValue;
	FAttr.Disabled := FAttr.Normal and $77;
	FAttr.Hover := FAttr.Normal;
	FAttr.Click := FAttr.Normal or $08;
	FAttr.Focused :=  FAttr.Normal;
end;

procedure TControl.SetTextColor(AValue : integer);
begin
	SetTextAttr(FAttr.Normal and $F0 + AValue);
end;

procedure TControl.SetBackground(AValue : integer);
begin
	SetTextAttr(FAttr.Normal and $0F + AValue shl 4);
end;

procedure TControl.CheckLocal(var AEvent : TEvent);
begin
	if (AEvent.What = evSystem) and (AEvent.Command = cmSpeaker) then begin
		ClearEvent(AEvent);
		QSpeaker.SpeakerNext;
		exit;
	end;
	if not FEnabled then exit;
	if not FShown then exit;
	if AEvent.What = evKeyboard then begin
		case FKeyFlags of
			kfContains : if AEvent.ShiftCode and FKeyValue.ShiftCode = 0 then exit;
			kfMatches : if AEvent.ShiftCode and FKeyValue.ShiftCode <> FKeyValue.ShiftCode then exit;
		end;
		if AEvent.KeyCode <> FKeyValue.KeyCode then exit;
		{$IFDEF EventProcs}
			if Assigned(OnKeyboard) then OnKeyboard(@Self, AEvent);
		{$ENDIF}
		ClearEvent(AEvent);
		TriggerHook; 
	end else if AEvent.What = evCommand then begin
		if (FRespond = cmNone) or (AEvent.Command <> FRespond) then exit;
		{$IFDEF EventProcs}
			if Assigned(OnRespond) then OnRespond(@Self, AEvent);
		{$ENDIF}
		ClearEvent(AEvent);
		TriggerHook;
	end;
end;

procedure TControl.CheckMouse(var AEvent : TEvent);
begin
	if not FEnabled then exit;
	if not FShown then exit;
	if FMouseOver then
		begin
			if (AEvent.What and evMouseDown = evMouseDown) then
				begin
					if Not FClicking then
						begin
							FClicking := True;
							if FAttr.Hover <> FAttr.Click then Update;
							{$IFDEF EventProcs}
								if Assigned(OnMouseCLick) then OnMouseCLick(@Self, AEvent);
							{$ENDIF}
						end;
					ClearEvent(AEvent);
				end 
			else
			if (AEvent.What and evMouseUp = evMouseUp) then
				begin
					if FClicking then
						begin
							FClicking := False;
							if FAttr.Hover <> FAttr.Click then Update;
							{$IFDEF EventProcs}
								if Assigned(OnMouseRelease) then OnMouseRelease(@Self, AEvent);
							{$ENDIF}
							TriggerHook;
						end;
					ClearEvent(AEvent);
				end;
		end;
end;

procedure TControl.TriggerHook;
begin
	SendCommand;
end;

procedure TControl.SendCommand;
{$IFDEF EventProcs}
var
	Event : TEvent;
{$ENDIF}
begin
	{$IFDEF EventProcs}
		ClearEvent(Event);
		Event.What := evCommand;
		Event.Command := FCommand;
		{ Event.InfoPtr := @Self; }
		if Assigned(OnTriggered) then OnTriggered(@Self, Event);
	{$ENDIF}
	if FCommand <> cmNone then 
		PutCommand(FCommand, @Self)
	else if FTrigger <> cmNone then
		PutCommand(FTrigger, @Self);
end;

function TControl.CheckEvent(var AEvent : TEvent) : PControl;
var
	T, P : PControl;
	CMouseOver : boolean;
	{ Handled : boolean;}
	{ Hold : TEvent; }
begin
	T := nil;
	if Assigned(Application) and Assigned(Application^.FFocused) and (AEvent.What and evMessage <> evNothing ) then begin
		{ StartDrawing; }
		P := Application^.FFocused;
		{while Assigned(P) and not assigned(T) do begin}
			P^.CheckLocal(AEvent);
			if AEvent.What = evNothing then 
				T := P;
		{	P := PControl(P^.Parent);
		end;}
		if Assigned(T) and (T <> @Self) then begin
			Update;
		end;
		{ FinishDrawing; }
	end;
	{ Handled := false; }		
	if FVisible and FShown then begin 
{		Hold := AEvent;}
		if Assigned(First) then 
			begin
				P := PControl(First);
				while Assigned(P) and not assigned(T) do begin
					T := P^.CheckEvent(AEvent);
					P := PControl(P^.Next);
				end;
			end;

		if AEvent.What and evMouseMove <> evNone then
			begin
				CMouseOver := CheckInside(AEvent.Where.X, AEvent.Where.Y) and 
					(not Assigned(T)) and FShown;
								
				if CMouseOver <> FMouseOver then
					begin
						if Assigned(Application) then
							begin
								if CMouseOver then 
									Application^.SetFocused(@Self)
								else begin
									P := PControl(Parent);
									while Assigned(P) and (not P^.FMouseOver) do 
										P := PControl(P^.Parent);
									Application^.SetFocused(P);
								end;
							end;

						FMouseOver := CMouseOver;
						if FClicking then
							FClicking := CMouseOver;
						{$IFDEF EventProcs}
							if Assigned(OnMouseOver) then OnMouseOver(@Self, AEvent);
						{$ENDIF}
						if FAttr.Hover <> FAttr.Normal then Update;  
					end;
				{ if CMouseOver then Handled := true;}
			end;
		
		if (AEvent.What and evMouse <> evNothing) and (not Assigned(T)) then
			begin
				CheckMouse(AEvent);
				if AEvent.What = evNothing then
					T := @Self;							
			end 
		else
		if (AEvent.What and $FF10 <> evNothing) and (not Assigned(T)) then begin
			CheckLocal(AEvent);
			if AEvent.What = evNothing then
				T := @Self;							
		end;
	end;
	
	{ if Handled and (not Assigned(T)) then T := @Self; }

{	if Assigned(Next) and (Not Assigned(T)) then 
		T := PControl(Next)^.CheckEvent(AEvent); }

	CheckEvent := T;
end;

procedure TControl.SetCommand(AValue : word); 
begin
	FCommand := AValue;
end;

function  TControl.GetCommand : word; 
begin
	GetCommand := FCommand;
end;

procedure TControl.SetRespond(AValue : word); 
begin
	FRespond := AValue;
end;

function  TControl.GetRespond : word; 
begin
	GetRespond := FRespond;
end;

procedure TControl.GetKeyCode(var AValue : TKeyValue; var AFlags : word); 
begin
	AValue := FKeyValue;
	AFlags := FKeyFlags;
end;

procedure TControl.SetKeyCode(AKeyCode, AShiftCode, AFlags : word); 
begin
	FKeyValue.ShiftCode := AShiftCode;
	FKeyValue.KeyCode := AKeyCode;
	FKeyFlags := AFlags;
end;

procedure TControl.SetAllowedOutside(AValue : boolean); 
begin
	if FAllowOutside = AValue then exit;
	FAllowOutside := AValue;
	Update;
end;

function TControl.GetAllowedOutside : boolean; 
begin
	GetAllowedOutside := FAllowOutside;
end;

procedure TControl.ApplyConfig( AConfig : PConfigFile );
begin
	AConfig^.OpenSection(GetPathID);
end;

procedure TControl.ApplyLanguage( AConfig : PConfigFile );
var
	C : PControl;
	P, M : PListItem;
	S : String;
begin
	{$IFDEF DEVLOG_LANGUAGES}
	WriteLog('Apply Language to ' + GetPathID );
	LogInc;
	{$ENDIF}
	C := PControl(First);
	while Assigned(C) do begin
		C^.ApplyLanguage(AConfig);
		C := PControl(C^.Next);
	end;
	{$IFDEF DEVLOG_LANGUAGES}
	LogDec;
	{$ENDIF}
	AConfig^.OpenSection('HELP');
	P := AConfig^.FindKey(GetPathID);
    if Assigned(P) then begin
		FHelpList.Clear;
		P := P^.First;
		while Assigned(P) do begin
			FHelpList.Add(New(PListItem, Create(P^.GetNameId)));
			P := P^.Next;
		end;
	{$IFDEF TEMPLATES}
	end else begin
		AConfig^.SetValue(GetPathID , '');
		P := AConfig^.FindKey(GetPathID);
		if Assigned(P) and Assigned(FHelpList.First) then begin
			P^.Clear;
			M := FHelpList.First;
			while Assigned(M) do begin
				S := M^.GetNameID;
				if MaxAvail < Sizeof(TListItem) + Length(S) + 1 then Halt(8);
				P^.Add(New(PListItem, Create(S)));
				M := M^.Next;
			end;
		end;
	{$ENDIF}
	end; 
	AConfig^.OpenSection('STRINGS');
end;

procedure TControl.ApplyTheme( AConfig : PConfigFile );
var
	P : PControl;
	A : record
		H, V : Byte;
	end;
	Temp : TBounds;
begin
	{$IFDEF DEVLOG_THEMES}
		WriteLog('Apply Theme to ' + GetPathID );
		LogInc;
	{$ENDIF}
	P := PControl(First);
	while Assigned(P) do begin
		P^.ApplyTheme(AConfig);
		P := PControl(P^.Next);
	end;
	AConfig^.OpenSection(GetPathID);
	FAutoSize := StrBool(AConfig^.GetValue('AUTOSIZE', BoolStr(FAutoSize)));  
	Temp := FRBounds; 
	AConfig^.GetInts('BOUNDS', Temp, SizeOf(Temp) div SizeOf(Integer));
	AConfig^.GetInts('MARGINS', FMargin, SizeOf(FMargin) div SizeOf(Integer));
	AConfig^.GetInts('COLORS', FAttr, SizeOf(FAttr) div SizeOf(Integer));	
	AConfig^.GetValue('CLASS', GetClassID + ' (Informational only)');	
	AConfig^.GetInts('ANCHORS', FAnchors, SizeOf(FAnchors) div SizeOf(Integer));	
	FAllowOutside := StrBool(AConfig^.GetValue('OUTSIDE', BoolStr(FAllowOutside)));   
	FEnabled := StrBool(AConfig^.GetValue('ENABLED', BoolStr(FEnabled)));   
{	FTabStop := StrBool(AConfig^.GetValue('TABSTOP', BoolStr(FTabStop)));  }
	if FTextAlign then begin
		A.H := Byte(FHAlign);
		A.V := Byte(FVAlign);
		AConfig^.GetInts('ALIGN', A, Sizeof(A) div SizeOf(Integer));
		FHAlign := THorizontalAlign(A.H);
		FVAlign := TVerticalAlign(A.V);
	end;
	FVisible := StrBool(AConfig^.GetValue('VISIBLE', BoolStr(FVisible)));  
	if (Temp.Left <> FRBounds.Left) or (Temp.Top <> FRBounds.Top) or
	(Temp.Width <> FRBounds.Width) or (Temp.Height <> FRBounds.Height) then
		SetBounds(Temp);
	{$IFDEF DEVLOG_THEMES}
		LogDec;
	{$ENDIF}
end;

procedure TControl.ApplyKeyboard( AConfig : PConfigFile );
var
	P : PControl;
	T : String;
begin
	{$IFDEF DEVLOG_KEYBOARDS}
		WriteLog('Apply Keymap to ' + GetPathID );
		LogInc;
	{$ENDIF}
	P := PControl(First);
	while Assigned(P) do begin
		P^.ApplyKeyboard(AConfig);
		P := PControl(P^.Next);
	end;
	
	if Assigned(Application) then begin
		AConfig^.OpenSection('EVENTS');	
		FCommand := Application^.GetCommandCode(AConfig^.GetValue(GetPathID, Application^.GetCommandName(FCommand)));
		{$IFDEF DEVLOG_KEYBOARDS}
			WriteLog('Set Command to ' + Application^.GetCommandName(FCommand));
		{$ENDIF}
		AConfig^.OpenSection('KEYSTROKES');	
		T := AConfig^.GetValue(GetPathID, KeyAsStr(FKeyValue, FKeyFlags));
		{$IFDEF DEVLOG_KEYBOARDS}
			WriteLog('Set KeyCode to ' + T);
		{$ENDIF}
		StrAsKey(T, FKeyValue, FKeyFlags);
	end;
	{$IFDEF DEVLOG_KEYBOARDS}
		LogDec;
	{$ENDIF}

end;

procedure TControl.SetAnchors(AValue : word);
begin
	if AValue = FAnchors then exit;
	FAnchors := AValue;
end;

function  TControl.GetAnchors(AValue : integer) : word; 
begin
	GetAnchors := FAnchors;
end;

procedure TControl.DoAutoSize; 
begin
	if not FAutoSize then exit;
	Update;
end;

procedure TControl.AddHelpText ( AValue : String ); 
begin
	FHelpList.Add(New(PListItem, Create(AValue))); 
end;

procedure TControl.SetHelpText ( AValue : String ); 
begin
	FHelpList.Clear; 
	AddHelpText(AValue);
end;

function TControl.GetHelpText : String; 
begin
	if Assigned(FHelpList.First) then
		GetHelpText := FHelpList.First^.GetNameID
	else
		GetHelpText := ''; 
end;

procedure TControl.ClearHelpText;
begin
	FHelpList.Clear;
end;

procedure TControl.BeginUpdate;
begin
	FUpdating := True;
end;

procedure TControl.EndUpdate;
begin
	if Not FUpdating then Exit;
	FUpdating := False;
	AdjustSize;
	Update;
end;
{$ENDIF}
