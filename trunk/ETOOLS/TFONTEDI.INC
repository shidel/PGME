{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

type 
	PFontEdit = ^TFontEdit;
	TFontEdit = object(TBox)
	public { protected }
		FMUL, FOFS : TPoint;
		FNullChar : Char;
		FNullColor : Integer; 
		procedure SetCurrentBounds(AValue : TBounds); virtual;
	public
			FFont : PFont;
			FCurrent : integer;
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure Draw; virtual;
		function GetMaxHeight : integer; virtual;
	end;
	
constructor TFontEdit.Create;
begin
	inherited Create(AParent, AName);
	FFont := New(PFont, Create('EDITFONT'));
	FStyle := bxSingle;
	FShadow := False;
	FCurrent := -1;
	FNullChar := 'X';
	FNullColor := TextAttr and $F0 + $08;
	FMUL.X := 1;
	FMUL.Y := 1;
	FOFS.X := 0;
	FOFS.Y := 0;
end;

destructor TFontEdit.Destroy;
begin
	if Assigned(FFont) then Dispose(FFont, Destroy);
	inherited Destroy;
end;

procedure TFontEdit.SetCurrentBounds(AValue : TBounds); 
var
	ExtraSize : integer;
begin
	ExtraSize := 0;
	if FBorder then Inc(ExtraSize, 2);
	if FShadow then Inc(ExtraSize, 2);
	FMUL.X := 1;
	FMUL.Y := 1;
	FOFS.X := 0;
	FOFS.Y := 0;
	
	{ Fix Height }
	if FShadow then Dec(ExtraSize, 1);
	
	if Assigned(FFont) and (FFont^.GetHeight > 0) then 
		AValue.Height := FFont^.GetHeight
	else if Assigned(UserFontPtr) and (Seg(UserFontPtr^) <> $C000)then
		AValue.Height := FontHeight
	else
		AValue.Height := 16;

	if AValue.Height > GetMaxHeight then 
		AValue.Height := GetMaxHeight;

	if ((Application^.FFont.GetEnabled) and (Application^.FFont.GetHeight >= 12)) or
		(AValue.Height >= 12) then 
		FMUL.X := 2;
		
	if ExtraSize > 0 then begin
		Inc(FOFS.X);
		Inc(FOFS.Y);
	end;
	{ if running in less than an 8x10 font then dont do doublewide } 
	AValue.Width := 8 * FMUL.X + ExtraSize;

		
	AValue.Height := AValue.Height * FMUL.Y + ExtraSize;
	AValue.Top := (PControl(Parent)^.Height div 2) - (AValue.Height div 2);
	inherited SetCurrentBounds(AValue);
end;

function TFontEdit.GetMaxHeight : integer;
var
	ExtraSize : integer;
begin
	ExtraSize := 0;
	if FBorder then Inc(ExtraSize, 2);
	if FShadow then Inc(ExtraSize, 1);
	GetMaxHeight := (PControl(Parent)^.Height div FMUL.Y) -
	  (PControl(Parent)^.FMargin.Top) -
	  (PControl(Parent)^.FMargin.Bottom) -
	  ExtraSize - 2;
end;

procedure TFontEdit.Draw;
var
	C : Char;
begin
	Inherited Draw;
	if FCUrrent = -1 then begin
		C := TextChar;
		TextAttr := FNullColor;
		TextChar := FNullChar;
		ClrScr;
		TextChar := C;
		Exit;
	end;
end;
