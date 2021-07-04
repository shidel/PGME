{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TLine --- }

{$IFDEF INTERFACE}
const
	class_TLine = 'TLine';
	
type
	PLine = ^TLine;
	TLine = object(TControl)
	private
		FStyle : integer;
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure SetStyle(AValue : integer); virtual;
		function GetStyle : integer; virtual;
		procedure Draw; virtual;		
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
	end;
	{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TLine.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TLine) then 
		ObjectClass := class_TLine
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TLine.Create(AParent : PControl; AName : String);
begin
	inherited Create(AParent, AName);
{	FUseCaption := false; }

	FClearWindow := false;
	FStyle := lnSingle or lnHorizontal;
	FTextAlign := False;
	FAttr.Click := FAttr.Normal;
end;

procedure TLine.SetStyle(AValue : integer);
begin
	if FStyle = AValue then exit;
	FStyle := AValue;
	Refresh;
end;

function TLine.GetStyle : integer;
begin
	GetStyle := FStyle;
end;

procedure TLine.Draw; 
var
	WMin, WMax : word;
begin
	WMin := WindMin;
	WMax := WindMax;
	Window(1,1,Lo(ScreenMax)+ 1,Hi(ScreenMax) + 1);
	if FStyle and lnVertical = lnVertical then
		DrawLine(FWindow.Left, FWindow.Top, Height, FStyle)
	else
		DrawLine(FWindow.Left, FWindow.Top, Width, FStyle);
	Window(Lo(WMin) + 1, Hi(WMin) + 1, Lo(WMax) + 1, Hi(WMax) + 1);
    inherited Draw; 
	{$IFDEF DEVLOG_DRAW}
		LogInc;
		WriteLog('STYLE ' + HexStr(FStyle)); 
		LogDec;
	{$ENDIF}
end;

procedure TLine.ApplyTheme( AConfig : PConfigFile );
begin
	inherited ApplyTheme(AConfig);
	AConfig^.GetInts('STYLE', FStyle, Sizeof(FStyle) div SizeOf(Integer));
end;
{$ENDIF}
