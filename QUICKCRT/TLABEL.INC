{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TLabel --- }

{$IFDEF INTERFACE}
const
	class_TLabel = 'TLabel';
	
type
	PLabel = ^TLabel;
	TFormatProc = procedure (AVisual : PControl; var AValue : String); { of Object BooHoo. } 
	
	TLabel = object (TControl)
	private
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FCaption : String;
		FCaptionAsChar : boolean;
		FUseCaption : boolean;
		function Formatted : String; virtual;
		procedure DoAutoSize; virtual;
	public
			OnFormat : TFormatProc;
		constructor Create(AParent : PControl; AName : String);
		procedure SetCaption(AValue: String); virtual;
		function GetCaption : String; virtual;
		procedure Draw; virtual;		
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TLabel.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TLabel) then 
		ObjectClass := class_TLabel
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TLabel.Create;
begin
	inherited Create(AParent, AName);
	FUseCaption := True;
	FCaptionAsChar := False;
	FCaption := '';
	FHAlign := AlignCenter;
	FVAlign := AlignMiddle;
	OnFormat := nil;
	FTextAlign := True;
	FAttr.Click := FAttr.Normal;
end;

procedure TLabel.SetCaption(AValue: String);
begin
	if FCaption = AValue then exit;
	FCaption := AValue;
	DoAutoSize;
	Update;
end;

function TLabel.GetCaption : String;
begin
	GetCaption := FCaption;
end;

procedure TLabel.Draw; 
var
	X, Y : integer;
	W, H : integer;
	S : String;
begin
	inherited Draw;
	if FUseCaption then begin
		S := Formatted;
		W := Lo(WindMax) - Lo(WindMin) + 1;
		H := Hi(WindMax) - Hi(WindMin) + 1;
		case FHAlign of
			AlignLeft   : X := 0;
			AlignCenter : X := W shr 1 - Length(S) shr 1;
			AlignRight  : X := W - Length(S);
		end;
		case FVAlign of
			AlignTop    : Y := 0;
			AlignMiddle : Y := H shr 1;
			AlignBottom : Y := H - 1;
		end;
		if X < 0 then X := 0;
		if Y < 0 then Y := 0;
		GotoXY(1 + X, 1 + Y);
		FWrite(CropStr(S, W));
   end;
end;

procedure TLabel.ApplyLanguage( AConfig : PConfigFile );
begin
	inherited ApplyLanguage(AConfig);
	if (not FCaptionAsChar) and (FUseCaption) then SetCaption(AConfig^.GetValue(GetPathID, GetCaption));
end;

procedure TLabel.ApplyTheme( AConfig : PConfigFile );
var
	Temp : Integer;
begin
	inherited ApplyTheme(AConfig);
	if FCaptionAsChar then begin
		FCaption := AConfig^.GetHexStr('DISPLAY', StrHex(FCaption, false));
	end; 
end;

function TLabel.Formatted : string;
var
	TempStr : String;
begin
	if Assigned(OnFormat) then begin
		TempStr := FCaption;
		OnFormat(@Self, TempStr);
		Formatted := TempStr;
	end else
		Formatted := FCaption;	
end;

procedure TLabel.DoAutoSize; 
var
	B : TBounds;
begin
	if not FAutoSize then exit;

	GetBounds(B);
	Bounds(B.Left, B.Top, Length(Formatted), 1, B);
	SetBounds(B);
end;
{$ENDIF}
