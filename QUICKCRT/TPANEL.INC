{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TPanel --- }

{$IFDEF INTERFACE}
const
	class_TPanel = 'TPanel';
	
type
	PPanel = ^TPanel;
	TPanel = object (TLabel)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FBorder, FShadow : boolean;
		FStyle : integer;
		procedure AdjustMargins; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure SetBorder(AValue : boolean); virtual;
		function GetBorder : boolean; virtual;
		procedure SetShadow(AValue : boolean); virtual;
		function GetShadow : boolean; virtual;
		procedure SetStyle(AValue : integer); virtual;
		function GetStyle : integer; virtual;
		procedure Draw; virtual;		
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TPanel.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TPanel) then 
		ObjectClass := class_TPanel
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TPanel.Create;
begin
	inherited Create(AParent, AName);
	FUseCaption := True;
	FBorder := False;
	FShadow := False;
	FStyle := bxSingle;
	FTextAlign := False;
end;

procedure TPanel.AdjustMargins;
var
	Temp : TMargins;
begin
	Temp.Left := 0;
	Temp.Right := 0;
	Temp.Top := 0;
	Temp.Bottom := 0;
	if FBorder then 
		begin
			Inc(Temp.Left);
			Inc(Temp.Top);
			Inc(Temp.Bottom);
			Inc(Temp.Right);
		end;
	if FShadow then 
		begin
			Inc(Temp.Bottom);
			Inc(Temp.Right, 2);
		end;
	SetMargins(Temp);
end;

procedure TPanel.SetBorder(AValue: boolean);
begin
	if AValue = FBorder then exit;
	FBorder := AValue;
	AdjustMargins;
	Update;
end;

function TPanel.GetBorder : boolean;
begin
	GetBorder := FBorder;
end;

procedure TPanel.SetShadow(AValue: boolean);
begin
	if AValue = FShadow then exit;
	FShadow := AValue;
	AdjustMargins;
	Update;
end;

function TPanel.GetShadow : boolean;
begin
	GetShadow := FShadow;
end;

procedure TPanel.SetStyle(AValue: integer);
begin
	if AValue = FStyle then exit;
	FStyle := AValue;
	Update;
end;

function TPanel.GetStyle;
begin
	GetStyle := FStyle;
end;

procedure TPanel.Draw; 
var
	W, H : integer;
begin
	W := Lo(WindMax) - Lo(WindMin) + 1;
	H := Hi(WindMax) - Hi(WindMin) + 1;
	if (H < 2) or (W < 2) or (Not FBorder) then 
		begin
			inherited Draw;
			exit;
		end;
	if (H < 3) or (W < 4) or (Not FShadow) then 
		begin
			PutWindow(1,1, FBehind^);
			DrawBox(1,1, W, H, FStyle);
			SubWindow(2,2, W-1, H - 1);
			ClrScr; 
			inherited Draw;
			exit;
		end;
	PutWindow(1,1, FBehind^);
	DrawBox(1,1, FWindow.Width-2, FWindow.Height-1, FStyle);
	DrawShadow(1,1, FWindow.Width-2, FWindow.Height-1, bsDoubleWide);
	SubWindow(2,2, W - 3, H - 2);
	ClrScr; 
	inherited Draw;
end;

procedure TPanel.ApplyTheme( AConfig : PConfigFile );
begin
	inherited ApplyTheme(AConfig);
	FBorder := StrBool(AConfig^.GetValue('BORDER', BoolStr(FBorder)));   
	FShadow := StrBool(AConfig^.GetValue('SHADOW', BoolStr(FShadow)));   
	AConfig^.GetInts('STYLE', FStyle, Sizeof(FStyle) div SizeOf(Integer));
end;
{$ENDIF}
