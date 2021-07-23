{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TProgressBar --- }

{$IFDEF INTERFACE}
const
	class_TProgressBar = 'TProgressBar';
	
type
	TProgressBarElements = record
		Filled, Partial, Empty : Char;
	end;
	PProgressBar = ^TProgressBar;
	TProgressBar = object(TControl)
	private
		FProgress : integer;
		FChars	: TProgressBarElements;
		FPercent : String;
		FTextWidth : integer;
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure SetProgress(AValue : integer); virtual;
		function GetProgress : integer; virtual;
		procedure Draw; virtual;		
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		function GetPercentText : string; virtual;
		procedure SetPercentText (AValue : String); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TProgressBar.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TProgressBar) then 
		ObjectClass := class_TProgressBar
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TProgressBar.Create;
begin
	inherited Create(AParent, AName);
	FProgress := 0;
	FChars.Empty := #$B0;
	FChars.Partial := #$B0;
	FChars.Filled := #$B1;
	FTextWidth := 0;
	FPercent := '';
	SetPercentText('0;%0%%');
	FTextAlign := True;
end;

procedure TProgressBar.SetProgress(AValue : integer);
begin
	if AValue < 0 then AValue := 0;
	if AValue > 100 then AValue := 100;
	if AValue = FProgress then exit;
	FProgress := AValue;
	Refresh;;
end;

function TProgressBar.GetProgress : integer; 
begin
	GetProgress := FProgress;
end;

procedure TProgressBar.Draw; 
var	
	TW, I, A : integer;
	S : String;
	D : TFormatData;
begin
	TW := Width;
	D.ID := fmtInteger;
	D.IntegerValue := FProgress;
	S := FormatStr(FPercent, D, 1);
	if (S <> '') and (FHAlign <> AlignCenter) then begin
		TW := TW - FTextWidth;
	end;
	
	if FHAlign = AlignLeft then begin
		if FProgress < 100 then 
			TextAttr := FAttr.Disabled
		else
			TextAttr := FAttr.Normal;
		FWrite(RSpace(S, FTextWidth));
	end;	
	TextAttr := FAttr.Normal;
	FWrite(ChrStr(FChars.Filled, Trunc(TW * FProgress / 100 )));
	if FProgress < 100 then begin
		if Trunc(TW * FProgress / 100 ) < Round(TW * FProgress / 100 ) then begin
			FWriteChar(FChars.Partial);
			TextAttr := FAttr.Disabled;
			FWrite(ChrStr(FChars.Empty, TW - Trunc(TW * FProgress / 100 ) - 1));
		end else begin
			TextAttr := FAttr.Disabled;
			FWrite(ChrStr(FChars.Empty, TW - Trunc(TW * FProgress / 100 ) ));
		end;
		TextAttr := FAttr.Disabled;
	end;
{	TextAttr := FAttr.Normal; }
	if FHAlign = AlignCenter then begin
		for I := 1 to Length(S) do begin
			GotoXY(Width div 2 - Length(S) div 2 + I, 1);
			TextAttr := FReadAttr;
			GotoXY(Width div 2 - Length(S) div 2 + I, 1);
			FWriteChar(S[I]);
		end;
	end else if FHAlign = AlignRight then begin
		FWrite(LSpace(S, FTextWidth));
	end;
end;

procedure TProgressBar.ApplyLanguage( AConfig : PConfigFile );
begin
	inherited ApplyLanguage(AConfig);
	SetPercentText(AConfig^.GetValue(GetPathID, GetPercentText));
end;

procedure TProgressBar.ApplyTheme( AConfig : PConfigFile );
var
	Temp : Array[0..2] of integer;
begin
	inherited ApplyTheme(AConfig);
	Temp[0] := byte(FChars.Empty);
	Temp[1] := byte(FChars.Partial);
	Temp[2] := byte(FChars.Filled);
	AConfig^.GetInts('DISPLAY', Temp, SizeOf(Temp) div SizeOf(Integer));
	FChars.Empty := Chr(Temp[0]);
	FChars.Partial := Chr(Temp[1]);
	FChars.Filled := Chr(Temp[2]);
end;

function TProgressBar.GetPercentText : string;
begin
	GetPercentText := FPercent;
end;

procedure TProgressBar.SetPercentText (AValue : String);
var
	D : TFormatData;
begin
	if AValue = FPercent then exit;
	FPercent := AValue;
	D.ID := 0;
	D.IntegerValue := 100;
	FTextWidth := Length(FormatStr(FPercent, D, 1)) + 1;
	Refresh;
end;
{$ENDIF}
