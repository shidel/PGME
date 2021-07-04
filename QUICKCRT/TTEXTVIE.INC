{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TTextView --- }

{$IFDEF INTERFACE}
const
	class_TTextView = 'TTextView';
	
type
	PTextView = ^TTextView;
	TTextView = object(TControl)
		FLB : TBounds; { Our Last Visual Bounds, for Scroll Bar Moving and Sizing }
		FScrollEdge : boolean; { Scroll Outside Control "On Window Edge" }
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FItems : TListItem;
		FVScroll {, FHScroll }: TScrollBar;
		FTop : PListItem;
		FIndex : integer;
		FSubItemPad : Str20;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure AdjustBars; virtual;
		procedure CheckMarkers; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure SetBounds(AValue : TBounds); virtual;
		procedure Draw; virtual;
		procedure DrawItem(var AItem : PListItem); virtual;
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		procedure Down( ARefresh : boolean ); virtual;
		procedure Up( ARefresh : boolean ); virtual;
		procedure PageDown; virtual;
		procedure PageUp; virtual;
		procedure Bottom; virtual;
		procedure Top; virtual;
		procedure SetIndex(AValue : integer); virtual;
		function GetIndex : integer; virtual;
		procedure SetScrollEdge(AValue : boolean); virtual;
		function GetScrollEdge : boolean; virtual;
		procedure AddItem( AValue : String); virtual;
		procedure SetSorted(AValue : boolean); virtual;
		procedure ClearItems; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TTextView.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TTextView) then 
		ObjectClass := class_TTextView
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TTextView.Create;
var
	B : TBounds;
	C : integer;
begin
	C := TextAttr;
	inherited Create(AParent, AName);
	TextAttr := C;
	FItems.Create('ITEMS');
	FLB := FCBounds;
	Bounds(1,1,1,1,B);
	TextAttr := C;
	FVScroll.Create(@Self, 'VScroll');
	FVScroll.SetFreeOnDestroy(False);
	FVScroll.SetBounds(B);
	FVScroll.FAllowOutSide := True;
{	FVScroll.SetAnchors(alTop or alRight); }
	FTop := nil;
	FIndex := -1;
	FScrollEdge := False;
	FSubItemPad := ' ';
end;

destructor TTextView.Destroy;
var
	I : integer;
begin
	FVScroll.Destroy;
	FItems.Destroy;
	inherited Destroy;
end;

procedure TTextView.ApplyLanguage( AConfig : PConfigFile );
begin
	inherited ApplyLanguage(AConfig);
end;

procedure TTextView.ApplyTheme( AConfig : PConfigFile );
begin
	inherited ApplyTheme(AConfig); 
end;

procedure TTextView.AdjustBars; 
var
	AValue : TBounds;
	T : integer;
begin
	GetBounds(AValue);
	FLB := AValue;
	T := AValue.Height;
	
	FVScroll.SetMin(1);
	FVScroll.SetMax(FItems.Count - T + 1);
	FVScroll.SetValue(FIndex + 1);
	AValue.Left := Width;
	AValue.Top :=  1;
	AValue.Width := 1;
	if FScrollEdge then Inc(AValue.Left);
	AValue.Height := Height;

	FVScroll.SetBounds(AValue); 

	FVScroll.SetVisible(FItems.Count > T);
end;

procedure TTextView.CheckMarkers;
begin
	if (FTop = nil) then begin
		FTop := FItems.First;
		if Assigned(FTop) then
			FIndex := 0
		else
			FIndex := -1;
	end;
	
	if (FLB.Width <> FCBounds.Width) or (FLB.Height <> FCBounds.Height) or 
	(FVScroll.GetMax <> FItems.Count) then AdjustBars;
end;

procedure TTextView.SetBounds(AValue : TBounds); 
begin
	inherited SetBounds(AValue);
	AdjustBars;
end;

procedure TTextView.Draw;
var
	Line, MH, MW : integer;
	P : PListItem;
	WMin, WMax : word;
begin
	CheckMarkers;
	Window(FWindow.Left, FWindow.Top, FWindow.Left + FWindow.Width - 1, FWindow.Top + 
		FWindow.Height - 1);

	{ inherited Draw;  }
	TextAttr := FAttr.Normal;
	ClrScr;

	Line := 1;
	MW := Width;
	if FVScroll.GetVisible and (Not FScrollEdge) then Dec(MW);
	MH := Height;	
	{ if FHScroll.GetVisible then Dec(MH); }
	P := FTop;
	WMin := WindMin;
	WMax := WindMax;
	while (Line <= MH) and Assigned(P) do begin
		Window(Lo(WMin) + 1, Hi(WMin) + Line, Lo(WMin) + MW, Hi(WMin) + Line);
		DrawItem(P);
		Inc(Line);
		P := P^.Next;
	end;
	Window(Lo(WMin) + 1, Hi(WMin) + 1, Lo(WMax) + 1, Hi(WMax) + 1);
end;

procedure TTextView.DrawItem(var AItem : PListItem); 
var
	I, TW, MW, X, Y, B : integer;
	P : PListItem;
begin
	TextAttr := FAttr.Normal;
	ClrScr; 
{	if Not Assigned(AItem) then begin
	end else }

	MW := Width;
	if FVScroll.GetVisible and (Not FScrollEdge)  then Dec(MW);
	TW := Length(AItem^.GetNameID);
	if TW > MW then TW := MW;
	Dec(MW, TW);
	FWrite(CropStr(AItem^.GetNameID, TW));
	P := AItem^.First;
	while Assigned(P) and (MW > 1) do begin
		TW := Length(P^.GetNameID) + 1;
		if TW > MW then TW := MW;
		Dec(MW, TW);
		{ TextAttr := FAttr.Normal; }
		FWrite(FSubItemPad + CropStr(P^.GetNameID, TW));
		P := P^.Next;
	end;
	{ TextAttr := FAttr.Normal; }
	FWrite(Space(MW));
end;

procedure TTextView.CheckLocal(var AEvent : TEvent); 
begin
	if AEvent.What = evCommand then
{	if (AEvent.InfoPtr = @FVScroll) or (AEvent.InfoPtr = @Self) or
	(Application^.FFocused = @Self)  then }
		case AEvent.Command of
			cmUp : begin
				ClearEvent(AEvent);
				Up(True);
			end;
			cmDown : begin
				ClearEvent(AEvent);
				Down(True);
			end;
			cmPageUp : begin
				ClearEvent(AEvent);
				PageUp;
			end;
			cmPageDown : begin
				ClearEvent(AEvent);
				PageDown;
			end;
			cmTextStart, cmLineStart : begin
				ClearEvent(AEvent);
				Top;
			end;
			cmTextEnd, cmLineEnd :  begin
				ClearEvent(AEvent);
				Bottom;
			end;
			cmScrollBar : if (AEvent.InfoPtr = @FVScroll) then  begin
				ClearEvent(AEvent);
				if FVScroll.GetValue - 1 <> FIndex then begin
					SetIndex(FVScroll.GetValue - 1);
				end;
			end;
		end;
	inherited CheckLocal(AEvent);
end;

procedure TTextView.Up( ARefresh : boolean ); 
var
	LI : integer;
begin
	LI := FIndex;
	CheckMarkers;
	if FIndex <> LI then Refresh;
	if Not Assigned(FTop) then exit;
	if Not Assigned(FTop^.Prior) then exit;
	FTop := FTop^.Prior;
	Dec(FIndex);
	if ARefresh then begin
		Refresh;
		FVScroll.SetValue(FIndex + 1); 
	end;
end;

procedure TTextView.Down( ARefresh : boolean );
var
	LI : integer;
begin	
	LI := FIndex;
	CheckMarkers;
	if FIndex <> LI then Refresh;
	if Not Assigned(FTop) then exit;
	if Not Assigned(FTop^.Next) then exit;
	if FIndex + Height >= FItems.Count then exit;

	FTop := FTop^.Next;
	Inc(FIndex);

	if ARefresh then begin
		Refresh;
		FVScroll.SetValue(FIndex + 1); 
	end;
end;

procedure TTextView.PageDown;
var
	MH : integer;
begin
	MH := Height;
	Dec(MH);
	While MH > 0 do begin
		Down(True);
		Dec(MH);
	end;
end;

procedure TTextView.PageUp; 
var
	MH : integer;
begin
	MH := Height;
	Dec(MH);
	While MH > 0 do begin
		Up(True);
		Dec(MH);
	end;
end;

procedure TTextView.Bottom;
begin
	SetIndex(FItems.Count - 1);
end;

procedure TTextView.Top; 
begin
	SetIndex(0);
end;

procedure TTextView.SetIndex(AValue : integer); 
var
	Temp : integer;
begin
	if AValue = FIndex then exit;
	CheckMarkers;
	if Not Assigned(FTop) then exit;
	if AValue < FIndex then begin
		Temp := FIndex;
		While ( AValue < Temp ) and ( FIndex > 0 ) and Assigned(FTop^.Prior) do begin
			Up(False);
			Inc(AValue);
		end;
	end else if AValue > FIndex then begin
		Temp := FIndex;
		While ( AValue > Temp ) and ( FIndex < FItems.Count - 1 ) and Assigned(FTop^.Next) do begin
			Down(False);
			Dec(AValue);
		end;
	end;
	Refresh;
	FVScroll.SetValue(FIndex + 1); 
end;

function TTextView.GetIndex : integer;
begin
	GetIndex := FIndex;
end;

procedure TTextView.SetScrollEdge(AValue : boolean);
begin
	if FScrollEdge = AValue then exit;
	FScrollEdge := AValue;
	AdjustBars;
	Update;
end;

function TTextView.GetScrollEdge : boolean;
begin
	GetScrollEdge := FScrollEdge;
end;

procedure TTextView.AddItem( AValue : String); 
begin
	FItems.Add(New(PListItem, Create(AValue)));
	AdjustSize;
	Update;
end;

procedure TTextView.SetSorted(AValue : boolean);
begin
	FItems.Sorted := AValue;
end;

procedure TTextView.ClearItems;
begin
	FTop := nil;
	FItems.Clear;
	FIndex := -1;
	AdjustSize;
	Update;
end;
{$ENDIF}
