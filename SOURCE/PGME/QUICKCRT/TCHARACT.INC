{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TCharacterMap --- }

{$IFDEF INTERFACE}
const
	class_TCharacterMap = 'TCharacterMap';
	
type 
	PCharacterMap = ^TCharacterMap;
	TCharacterMap = object (TDialog )
	private
		FColors : record
			Hexs, Chrs : integer;
		end;
		FLast : integer;
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		procedure CheckMouse(var AEvent : TEvent); virtual;
		procedure UpdateChar(C : integer; Toggle: boolean); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure Draw; virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
	end;
	
{$ENDIF}

{$IFDEF IMPLEMENTATION}

const
	CharMapOfsX = 3;
	CharMapOfsY = 1;
	
function TCharacterMap.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TCharacterMap) then 
		ObjectClass := class_TCharacterMap
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TCharacterMap.Create;
var
	B : TBounds;
begin
	inherited Create(AParent, AName);
	SetTitle('Character Map');
	Bounds(1,1,58,20,B);
	SetBounds(B);	
	FStyle := bxSingle;
	FLast := -1;
	FColors.Hexs := (FAttr.Normal and $70) or $01;
	FColors.Chrs := (FAttr.Normal and $70) or $04;
	FMargin.Left := 0;
	FMargin.Top := 0;
	FMargin.Right:= 0;
	FMargin.Bottom := 0;
	SetHelpText('This is a standard Character Map dialog box.');
	AddHelpText('');
	AddHelpText('It can aid in the selection of characters that are outside of the');
	AddHelpText('range of normal keystroke combinations.');
end;

procedure TCharacterMap.Draw;
var
	I, J : integer;
begin
	inherited Draw;
	for I := 0 to $F do begin
		TextAttr := FColors.Hexs;
		GotoXY(CharMapOfsX + 3 + I * 3 ,CharMapOfsY);
		FWrite(HexStr(I));
		GotoXY(CharMapOfsX , CharMapOfsY + 1 + I );
		FWrite(HexStr(I * $10));
		TextAttr := FColors.Chrs;
		for J := 0 to $F do begin
			FWrite(Space(2) +  Chr(J + I * $10));
		end;
	end;	
end;

procedure TCharacterMap.UpdateChar(C : integer; Toggle: boolean);
var
	I : integer;
begin
	if Toggle then 
		TextAttr := (FColors.Chrs and $07) shl 4 + (FColors.Chrs and $F0) shr 4
	else
		TextAttr := FColors.Chrs;
	Window(FWindow.Left, FWindow.Top, FWindow.Left + FWindow.Width - 1, FWindow.Top + FWindow.Height - 1);
	GotoXY( CharMapOfsX + 3 + (C and $F) * 3, CharMapOfsY + 2 + (C and $F0) shr 4);
	for I := 0 to 4 do
		FWriteAttr;
end;

procedure TCharacterMap.CheckMouse(var AEvent : TEvent); 
var
	X, Y : integer;
	Hold : TEvent;
begin
	Hold := AEvent;
	inherited CheckMouse(AEvent);
	if not GetVisible then exit;
	if not GetShown then exit;
	if FLast <> -1 then UpdateChar(FLast, False);
	FLast := -1;
	X := (AEvent.Where.X - FWindow.Left);
	Y := (AEvent.Where.Y - FWindow.Top);
	if (X < CharMapOfsX + 3) or (Y < CharMapOfsY + 1) then exit;
	X := (X - CharMapOfsX - 3) div 3;
	Y := Y - CharMapOfsY - 1;
	if (X > $0F) or (Y > $0F) then exit;
	FLast := Y shl 4 + X;
	UpdateChar(FLast, True);
	if Hold.What = evMouseUp then begin
		ClearEvent(Hold);		
		Hold.What := evCommand;
		Hold.Command := cmCloseWindow;
		PutEvent(Hold);
		FResult := FLast;
	end;
end;
	
procedure TCharacterMap.ApplyTheme( AConfig : PConfigFile );
begin
	inherited ApplyTheme(AConfig);
	AConfig^.GetInts('HEXCOLORS', FColors, SizeOf(FColors) div SizeOf(Integer));
end;
{$ENDIF}
