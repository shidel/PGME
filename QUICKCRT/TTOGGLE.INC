{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TToggle --- }

{$IFDEF INTERFACE}
const
	class_TToggle = 'TToggle';
	
type
	PToggle = ^TToggle;
	TToggle = object(TButton)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { Protected }
		FStr : String;
		FThumb : String;
		FColors : array[0..1] of integer;
		FOn	: boolean;
		function OnStr : String; virtual;
		function OffStr : String; virtual;
		function TextWidth : integer; virtual;
		procedure DoAutoSize; virtual;
		procedure TriggerHook; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
		function GetText : String; virtual;
		procedure SetText( AValue : String ); virtual;
		function GetThumb : String; virtual;
		procedure SetThumb( AValue : String ); virtual;
		function GetOn : boolean; virtual;
		procedure SetOn(AValue : boolean); virtual;
		procedure Draw; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TToggle.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TToggle) then 
		ObjectClass := class_TToggle
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TToggle.Create;
begin
	inherited Create(AParent, AName);
	FStr := 'off,ON';
{	FThumb := #$B2#$B2#$B2;
	FThumb := #$DB#$DB#$0A#$DB#$DB;}
	FThumb := #$B0#$B0#$B2#$B0#$B0;
	FOn := False;
	FColors[1] := $2F;
	FColors[0] := $47;
	FUseCaption := False;
	FTrigger := cmToggle;
end;

procedure TToggle.ApplyTheme( AConfig : PConfigFile );
begin
	inherited ApplyTheme( AConfig );
	FThumb := AConfig^.GetHexStr('THUMB', StrHex(FThumb, False));   
	AConfig^.GetInts('COLORS', FColors, Sizeof(FColors) div SizeOf(Integer));
end;

procedure TToggle.ApplyLanguage( AConfig : PConfigFile );
begin
	inherited ApplyLanguage(AConfig);
	SetText(AConfig^.GetValue(GetPathID, GetText));
end;

function TToggle.GetText : String;
begin
	GetText := FStr;
end;

procedure TToggle.SetText( AValue : String );
begin
	if AValue = FStr then exit;
	FStr := AValue;
	DoAutoSize;
end;

function TToggle.GetThumb : String; 
begin
	GetThumb := FThumb;
end;

procedure TToggle.SetThumb( AValue : String ); 
begin
	if AValue = FThumb then exit;
	FThumb := AValue;
	DoAutoSize;
end;

function TToggle.GetOn : boolean; 
begin
	GetOn := FOn;
end;

procedure TToggle.SetOn( AValue : Boolean ); 
begin
	if AValue = FOn then exit;
	FOn := AValue;
	Update;
end;

function TToggle.OnStr : String;
var
	P : integer;
begin
	P := Pos(',', FStr);
	if P = 0 then
		OnStr := FStr
	else
		OnStr := Copy(FStr, P + 1, Length(FStr));
end;

function TToggle.OffStr : String;
var
	P : integer;
begin
	P := Pos(',', FStr);
	if P = 0 then
		OffStr := ''
	else
		OffStr := Copy(FStr, 1, P - 1);
end;

function TToggle.TextWidth : integer;
var
	F, S : Integer;
begin
	F := Length(OnStr) ;
	S := Length(OffStr) ;
	if F > S then 
		TextWidth := F
	else
		TextWidth := S;
end;

procedure TToggle.DoAutoSize; 
var
	B : TBounds;
begin
	if FAutoSize then begin
		GetBounds(B);		
		Bounds(B.Left, B.Top, TextWidth + Length(FThumb), 1, B);
		SetBounds(B);
	end else
		inherited DoAutoSize;
end;

procedure TToggle.TriggerHook;
begin
	SetOn(not GetOn);
	inherited TriggerHook;
end;

procedure TToggle.Draw;
var
	T : integer;
begin
	{inherited Draw;}
	T := TextAttr;
	if GetOn then begin
		TextAttr := FColors[1];
		FWrite(CSpace(OnStr, Width - Length(FThumb)));
		TextAttr := T;
		FWrite(FThumb);
	end else begin
		FWrite(FThumb);
		TextAttr := FColors[0];
		FWrite(CSpace(OffStr, Width - Length(FThumb)));
		TextAttr := T;
	end;
end;


{$ENDIF}
