{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TListView --- }

{$IFDEF INTERFACE}
const
	class_TListView = 'TListView';
	
type
	TColumn = record
		Width : integer;
		Colors : TTextAttrs;
		Name  : PString;
		Align : THorizontalAlign;
	end;
	PColumns =^TColumns;
	TColumns = array[0..$8FE] of TColumn;
	PListView = ^TListView;
	TListView = object(TControl)
		FLB : TBounds; { Our Last Visual Bounds, for Scroll Bar Moving and Sizing }
		FLD : boolean; { Last Click was a DoubleCLick }
		FLL : integer; { Last Highlighted Line based on top of window }
		FScrollEdge : boolean; { Scroll Outside Control "On Window Edge" }
		FAllowNull : boolean; { Allow Selecting Empty/Null FCurrent }
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FColumns : PColumns;
		FColumnCount : integer;
		FNAttr : TTextAttrs; { No Header Attributes }
		FHAttr : integer; { Header Attributes }
		FItems : TListItem;
		FVScroll {, FHScroll }: TScrollBar;
		FNoColumnPad : Str80; { Pad String between items when no headers }
		FTop, FCurrent : PListItem;
		FIndex : integer;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure TriggerHook; virtual;
		procedure AdjustBars; virtual;
		procedure CheckMouse(var AEvent : TEvent); virtual;
		procedure CheckMarkers(FNoNill : boolean); virtual;
		procedure EndUpdate; virtual;
		procedure AdjustSize; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure SetColumns(AValue : integer); virtual;
		procedure SetBounds(AValue : TBounds); virtual;
		procedure Draw; virtual;
		procedure DrawHeader; virtual;
		procedure DrawItem(var AItem : PListItem; AState : integer); virtual;
		function  GetItemAttr(var AItem, ASubItem : PListItem; AColumn, AState : integer) : integer; virtual;
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
		function AddItem( AValue : String) : PListItem; virtual;
		procedure SetSorted(AValue : boolean); virtual;
		procedure ScrollTo(AValue : String); virtual;
		function GetCurrent : PListItem; virtual;
		procedure ClearItems; virtual;
		procedure SetAllowEmpty(AValue : boolean); virtual;
		function GetAllowEmpty : boolean; virtual;
		procedure DeleteItem(AItem : PListItem); virtual;		
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TListView.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TListView) then 
		ObjectClass := class_TListView
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TListView.Create;
var
	B : TBounds;
	C : integer;
begin
	C := TextAttr;
	inherited Create(AParent, AName);
	FColumnCount := 0;
	FColumns := nil;
	TextAttr := C;
	FItems.Create('ITEMS');
	{ FUseCaption := False; }
	FLB := FCBounds;
	FNoColumnPad := #32;
	Bounds(1,1,1,1,B);
	TextAttr := C;
	FVScroll.Create(@Self, 'VScroll');
	FVScroll.SetFreeOnDestroy(False);
	FVScroll.SetBounds(B);
	FVScroll.FAllowOutSide := True;
{	FVScroll.SetAnchors(alTop or alRight); }
	FTop := nil;
	FCurrent := nil;
	FHAttr := FAttr.Normal and $77;
	FAttr.Hover := FAttr.Normal xor $FF;
	FLL := 0;
	FLD := False;
	FTrigger := cmListView;
	FIndex := -1;
	FScrollEdge := False;
	FNAttr := FAttr;
	FAllowNull := True;
	if GetBlink then
		begin
			FNAttr.Hover := (FAttr.Normal and $80) shr 4 + (FAttr.Normal and $07) shl 4;
			FNAttr.Focused := (FAttr.Normal and $07) + $08 + (FAttr.Normal and $07) shl 4;
			FNAttr.Click := $09 or ((FAttr.Normal and $80) shr 4) + (FAttr.Normal and $07) shl 4;
		end
	else
		begin
			FNAttr.Hover := (FAttr.Normal and $80) shr 4 + (FAttr.Normal and $0F) shl 4;
			FNAttr.Focused := (FAttr.Normal and $07) + $08 + (FAttr.Normal and $07) shl 4;
			FNAttr.Click := $09 or ((FAttr.Normal and $80) shr 4) + (FAttr.Normal and $07) shl 4;
		end;
end;

destructor TListView.Destroy;
var
	I : integer;
begin
	FVScroll.Destroy;
	FItems.Destroy;
	if assigned(FColumns) then begin
		for I := 0 to FColumnCount - 1 do 
			FreeStr(FColumns^[I].Name);
		FreeMem(FColumns, sizeof(TColumn) * FColumnCount);
		FColumns := nil;
	end;
	inherited Destroy;
end;

procedure TListView.ApplyLanguage( AConfig : PConfigFile );
var
	I : integer;
	S : String;
begin
	inherited ApplyLanguage(AConfig);
	FNoColumnPad := AConfig^.GetValue(GetPathID + '.COLUMN.NONE', FNoColumnPad);
	if FColumnCount > 0 then 
		for I := 0 to FColumnCount - 1 do with FColumns^[I] do begin
			S := AConfig^.GetValue(GetPathID + '.COLUMN.' + IntStr(I), PtrStr(Name));
			FreeStr(Name);
			Name := StrPtr(S);
		end;
end;

procedure TListView.ApplyTheme( AConfig : PConfigFile );
var
	I : integer;
	B : Integer;
begin
	inherited ApplyTheme(AConfig); 
	AConfig^.GetInts('COLORS.HEADER', FHAttr, SizeOf(FHAttr) div SizeOf(Integer));
	AConfig^.GetInts('COLORS.COLUMNS', FNAttr, SizeOf(FNAttr) div SizeOf(Integer));
	if FColumnCount > 0 then 
		for I := 0 to FColumnCount - 1 do with FColumns^[I] do begin
			AConfig^.GetInts('COLUMN.' + IntStr(I) + '.COLOR', FColumns^[I].Colors, SizeOf(TTextAttrs) div SizeOf(Integer));
			AConfig^.GetInts('COLUMN.' + IntStr(I) + '.WIDTH', FColumns^[I].Width, SizeOf(integer) div SizeOf(Integer));
			B := Byte(FColumns^[I].Align);
			AConfig^.GetInts('COLUMN.' + IntStr(I) + '.ALIGN', B, SizeOf(B) div SizeOf(Integer));
			FColumns^[I].Align := THorizontalAlign(B);
		end;
end;

procedure TListView.AdjustBars; 
var
	AValue : TBounds;
	T : integer;
begin
	GetBounds(AValue);
	FLB := AValue;
	T := AValue.Height;
	if FColumnCount > 0 then Dec(T);
	
	FVScroll.SetMin(1);
	FVScroll.SetMax(FItems.Count);
	FVScroll.SetValue(FIndex + 1);
	AValue.Left := Width;
	AValue.Top :=  1;
	AValue.Width := 1;
	if FScrollEdge then Inc(AValue.Left);
	AValue.Height := Height;

	FVScroll.SetBounds(AValue); 

	FVScroll.SetVisible(FItems.Count > T);
end;

procedure TListView.CheckMarkers;
var
	LI : integer;
begin
	LI := FIndex;
	if (FTop = nil) or (FCurrent = nil) then begin
		FTop := FItems.First;
		if ((Not FAllowNull) and (FCurrent = nil)) or (FNoNill) then 
			FCurrent := FTop;
		if Assigned(FCurrent) then 
			FIndex := 0
		else
			FIndex := -1;
	end;
	if (FLB.Width <> FCBounds.Width) or (FLB.Height <> FCBounds.Height) or 
	(FVScroll.GetMax <> FItems.Count) or (LI <> FIndex) then AdjustBars;
end;

procedure TListView.SetBounds(AValue : TBounds); 
begin
	inherited SetBounds(AValue);
	AdjustBars;
end;

procedure TListView.SetColumns(AValue : integer); 
var
	I : integer;
begin
	if assigned(FColumns) then begin
		for I := 0 to FColumnCount - 1 do 
			FreeStr(FColumns^[I].Name);
		FreeMem(FColumns, sizeof(TColumn) * FColumnCount);
		FColumns := nil;
	end;
	if MaxAvail < sizeof(TColumn) * AValue then Halt(8);
	FColumnCount := AValue;
	GetMem(FColumns, sizeof(TColumn) * FColumnCount);
	for I := 0 to FColumnCount - 1 do begin
		FColumns^[I].Width := Width div FColumnCount;
		FColumns^[I].Colors := FAttr;
		if GetBlink then
			begin
				FColumns^[I].Colors.Hover := (FAttr.Normal and $80) shr 4 + (FAttr.Normal and $07) shl 4;
				FColumns^[I].Colors.Focused := (FAttr.Normal and $07) + $08 + (FAttr.Normal and $07) shl 4;
				FColumns^[I].Colors.Click := $09 or ((FAttr.Normal and $80) shr 4) + (FAttr.Normal and $07) shl 4;
			end
		else
			begin
				FColumns^[I].Colors.Hover := (FAttr.Normal and $80) shr 4 + (FAttr.Normal and $0F) shl 4;
				FColumns^[I].Colors.Focused := (FAttr.Normal and $07) + $08 + (FAttr.Normal and $07) shl 4;
				FColumns^[I].Colors.Click := $09 or ((FAttr.Normal and $80) shr 4) + (FAttr.Normal and $07) shl 4;
			end;
		FColumns^[I].Name  := nil;
		FColumns^[I].Align := AlignLeft;
	end;
end;

procedure TListView.Draw;
var
	Line, MH, MW : integer;
	P : PListItem;
	WMin, WMax : word;
begin
	CheckMarkers(False);
	Window(FWindow.Left, FWindow.Top, FWindow.Left + FWindow.Width - 1, FWindow.Top + 
		FWindow.Height - 1);

	{ inherited Draw;  }
	TextAttr := FAttr.Normal;
	ClrScr;

	Line := 1;
	if FColumnCount > 0 then begin
		DrawHeader;
		Inc(Line);
	end;
	MW := Width;
	if FVScroll.GetVisible and (Not FScrollEdge) then Dec(MW);
	MH := Height;	
	{ if FHScroll.GetVisible then Dec(MH); }
	P := FTop;
	WMin := WindMin;
	WMax := WindMax;
	while (Line <= MH) and Assigned(P) do begin
		Window(Lo(WMin) + 1, Hi(WMin) + Line, Lo(WMin) + MW, Hi(WMin) + Line);
		if FLL = Line then 
			DrawItem(P, 1)
		else if P = FCurrent then 
			DrawItem(P, 3)
		else
			DrawItem(P, 0);		
		Inc(Line);
		P := P^.Next;
	end;
	Window(Lo(WMin) + 1, Hi(WMin) + 1, Lo(WMax) + 1, Hi(WMax) + 1);
end;

procedure TListView.DrawHeader; 
var
	I, TW, MW : integer;
begin
	GotoXY(1,1);
	MW := Width;
	if FVScroll.GetVisible and (Not FScrollEdge)  then Dec(MW);
	for I := 0 to FColumnCount - 1 do begin
		TW := FColumns^[I].Width;
		if TW > MW then TW := MW;
		Dec(MW, TW);
		TextAttr := FHAttr;
		case FColumns^[I].Align of
			AlignLeft : FWrite(CropStr(RSpace(PtrStr(FColumns^[I].Name), FColumns^[I].Width), TW) );
			AlignRight : FWrite(CropStr(LSpace(PtrStr(FColumns^[I].Name), FColumns^[I].Width), TW) );
		else
			FWrite(CropStr(CSpace(PtrStr(FColumns^[I].Name), FColumns^[I].Width), TW) );
		end;
	end;
end;

procedure TListView.DrawItem(var AItem : PListItem; AState : integer); 
var
	I, TW, MW, X, Y, B : integer;
	P : PListItem;
begin
	TextAttr := GetItemAttr(AItem, AItem, 0, AState);
	ClrScr; 
{	if Not Assigned(AItem) then begin
	end else }
	if FColumnCount = 0 then begin
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
			TextAttr := GetItemAttr(AItem, P, 0, AState);
			FWrite(FNoColumnPad + CropStr(P^.GetNameID, TW));
			P := P^.Next;
		end;
		TextAttr := GetItemAttr(AItem, AItem, 0, AState);
		FWrite(Space(MW));
	end else begin
		MW := Width;
		if FVScroll.GetVisible and (Not FScrollEdge)  then Dec(MW);
		I := 0;
		P := AItem;
		while (I < FColumnCount) and (Assigned(P)) do begin
			TW := FColumns^[I].Width;
			if TW > MW then TW := MW;
			Dec(MW, TW);
			TextAttr := GetItemAttr(AItem, P, I, AState);	
			case FColumns^[I].Align of
				AlignLeft : FWrite(CropStr(RSpace(P^.GetNameID, FColumns^[I].Width), TW) );
				AlignRight : FWrite(CropStr(LSpace(P^.GetNameID, FColumns^[I].Width), TW) );
			else
				FWrite(CropStr(CSpace(P^.GetNameID, FColumns^[I].Width), TW) );
			end;
			inc(I);
			if P = AItem then 
				P := P^.First
			else
				P := P^.Next;
		end;
	end;
end;

function TListView.GetItemAttr(var AItem, ASubItem : PListItem; AColumn, AState : integer) : integer; 
begin
	if (FColumnCount = 0) or (not Assigned(AItem)) then 
		case AState of
			1 : GetItemAttr := FNAttr.Hover;
			2 : GetItemAttr := FNAttr.Click;
			3 : GetItemAttr := FNAttr.Focused;
			-1 : GetItemAttr := FNAttr.Disabled;
		else
			GetItemAttr := FNAttr.Normal; 
		end
	else
		case AState of
			1 : GetItemAttr := FColumns^[AColumn].Colors.Hover;
			2 : GetItemAttr := FColumns^[AColumn].Colors.Click;
			3 : GetItemAttr := FColumns^[AColumn].Colors.Focused;
			-1 : GetItemAttr := FColumns^[AColumn].Colors.Disabled;
		else
			GetItemAttr := FColumns^[AColumn].Colors.Normal; 
		end;
end;

procedure TListView.TriggerHook;
begin
	if (FCommand = cmNone) and FLD then
		PutCommand(cmListViewDouble, @Self)
	else
		inherited TriggerHook;
	FLD := False;
end;

procedure TListView.CheckLocal(var AEvent : TEvent); 
begin
	if AEvent.What = evCommand then
{	if (AEvent.InfoPtr = @FVScroll) or (AEvent.InfoPtr = @Self) or
	(Application^.FFocused = @Self)  then }
		case AEvent.Command of
			cmEnter, cmReturn : begin
				ClearEvent(AEvent);
				FLD := True;
				TriggerHook;
			end;
			cmUp : begin
				ClearEvent(AEvent);
				Up(True);
				PutCommand(cmListViewChange, @Self);
			end;
			cmDown : begin
				ClearEvent(AEvent);
				Down(True);
				PutCommand(cmListViewChange, @Self);
			end;
			cmPageUp : begin
				ClearEvent(AEvent);
				PageUp;
				PutCommand(cmListViewChange, @Self);
			end;
			cmPageDown : begin
				ClearEvent(AEvent);
				PageDown;
				PutCommand(cmListViewChange, @Self);
			end;
			cmTextStart, cmLineStart : begin
				ClearEvent(AEvent);
				Top;
				PutCommand(cmListViewChange, @Self);
			end;
			cmTextEnd, cmLineEnd :  begin
				ClearEvent(AEvent);
				Bottom;
				PutCommand(cmListViewChange, @Self);
			end;
			cmScrollBar : if (AEvent.InfoPtr = @FVScroll) then  begin
				ClearEvent(AEvent);
				if FVScroll.GetValue - 1 <> FIndex then begin
					SetIndex(FVScroll.GetValue - 1);
					PutCommand(cmListViewChange, @Self);
				end;
			end;
		end;
	inherited CheckLocal(AEvent);
end;

procedure TListView.CheckMouse(var AEvent : TEvent); 
var
	Hold : TEvent;
	J, L, MW : integer;
	P : PListItem;
	Ref : boolean;
begin
	Ref := false;
	Hold := AEvent;
	inherited CheckMouse(AEvent);
	if not GetVisible then exit;
	if not GetShown then exit;
	if (Not FMouseOver) or (FVScroll.GetVisible and (Not FScrollEdge) and 
	(Hold.Where.X - FWindow.Left > Width - FMargin.Left - 2  )) then begin
		if FLL <> -1 then begin
			FLL := -1;
			Refresh;
		end;
		FLD := False;
		exit;
	end;
	if Hold.What = evMouseDown then begin
		FLD := Hold.Double;
	end;
	L := Hold.Where.Y - FWindow.Top + 1;
	MW := Width;
	if FVScroll.GetVisible and (Not FScrollEdge)  then Dec(MW);
	if Hold.Where.X - FWindow.Left + 1 > MW then exit;
	if Hold.What = evMouseUp then begin
		P := FTop;
		J := L;
		if FColumnCount > 0 then Dec(J);
		while assigned(P) and (J > 1) do begin
			P := P^.Next;
			Dec(J);
		end;
		if P <> FCurrent then begin
			Ref := True;
			FCurrent := P;	
			if Assigned(P) then begin
				FVScroll.SetValue(P^.GetIndex + 1);
				FIndex := P^.GetIndex;
			end else begin
				FVScroll.SetValue(0);
				FIndex := -1;
			end;
		end;
			
	end;
	if (L <> FLL) or (Ref) then begin
		FLL := L;
		Refresh;
	end;	
end;

procedure TListView.Up( ARefresh : boolean ); 
var
	Flag : boolean;
	LI : integer;
begin
	Flag := Not Assigned(FCurrent);
	LI := FIndex;
	CheckMarkers(True);
	if FIndex <> LI then Refresh;
	if Not Assigned(FCurrent) then exit;
	if Not Assigned(FCurrent^.Prior) then exit;
	if Not (Flag and Assigned(FCurrent)) then begin
		if FCurrent = FTop then 
			FTop := FTop^.Prior;
		FCurrent := FCurrent^.Prior;
		Dec(FIndex);
	end;
	if ARefresh then begin
		Refresh;
		FVScroll.SetValue(FIndex + 1); 
	end;
end;

procedure TListView.Down( ARefresh : boolean );
var
	P : PListItem;
	C : integer;
	MH : integer;
	Flag : boolean;
	LI : integer;
begin	
	Flag := Not Assigned(FCurrent);
	LI := FIndex;
	CheckMarkers(True);
	if FIndex <> LI then Refresh;
	if Not Assigned(FCurrent) then exit;
	if Not Assigned(FCurrent^.Next) then exit;

	if Not (Flag and Assigned(FCurrent)) then begin
		FCurrent := FCurrent^.Next;
		Inc(FIndex);
		C := 0;
		P := FTop;
		while (P <> FCurrent) do begin
			P := P^.Next;
			Inc(C);
		end;
		MH := Height;
		{if FHScroll.GetVisible then Dec(MH); }
		if FColumnCount > 0 then Dec(MH);

		if C >= MH then 
			FTop := FTop^.Next;
	end;
	if ARefresh then begin
		Refresh;
		FVScroll.SetValue(FIndex + 1); 
	end;
end;

procedure TListView.PageDown;
var
	MH : integer;
begin
	MH := Height;
	Dec(MH);
	{if FHScroll.GetVisible then Dec(MH); }
	if FColumnCount > 0 then Dec(MH);
	While MH > 0 do begin
		Down(True);
		Dec(MH);
	end;
end;

procedure TListView.PageUp; 
var
	I, MH : integer;
begin
	MH := Height;
	Dec(MH);
	{if FHScroll.GetVisible then Dec(MH);}
	if FColumnCount > 0 then Dec(MH);
	While MH > 0 do begin
		Up(True);
		Dec(MH);
	end;
end;

procedure TListView.Bottom;
begin
	SetIndex(FItems.Count - 1);
end;

procedure TListView.Top; 
begin
	SetIndex(0);
end;

procedure TListView.SetIndex(AValue : integer); 
begin
	if AValue = FIndex then exit;
	CheckMarkers(True);
	if Not Assigned(FCurrent) then exit;
	While ( AValue < FIndex ) and ( FIndex > 0 ) and Assigned(FCurrent^.Prior) do Up(False);
	While ( AValue > FIndex ) and ( FIndex < FItems.Count - 1 ) and Assigned(FCurrent^.Next) do Down(False);
	Refresh;
	FVScroll.SetValue(FIndex + 1); 
end;

function TListView.GetIndex : integer;
begin
	GetIndex := FIndex;
end;

procedure TListView.ScrollTo(AValue : String);
begin
	CheckMarkers(True);
	if Not FItems.Sorted then exit;
	if Not Assigned(FCurrent) then exit;
	while Assigned(FCurrent^.Prior) and (FCurrent^.GetNameID > AValue) do 
		Up(True);
	while Assigned(FCurrent^.Next) and (FCurrent^.GetNameID < AValue) do 
		Down(True);
	Refresh;	
end;

procedure TListView.SetScrollEdge(AValue : boolean);
begin
	if FScrollEdge = AValue then exit;
	FScrollEdge := AValue;
	AdjustBars;
	Update;
end;

function TListView.GetScrollEdge : boolean;
begin
	GetScrollEdge := FScrollEdge;
end;

function TListView.AddItem( AValue : String) : PListItem; 
var
	Item : PListItem;
begin
	Item := New(PListItem, Create(AValue));
	FItems.Add(Item);
	if not FUpdating then begin
		AdjustSize;
		Update;
	end;
	AddItem := Item;
end;

procedure TListView.SetSorted(AValue : boolean);
begin
	FItems.Sorted := AValue;
end;

function TListView.GetCurrent : PListItem;
begin
	GetCurrent := FCurrent;
end;

procedure TListView.ClearItems;
begin
	FTop := nil;
	FCurrent := nil;
	FItems.Clear;
	FIndex := -1;
	FVScroll.SetMin(1);
	FVScroll.SetMax(1);
	FVScroll.SetValue(1);
	AdjustSize;
	Update;
end;

procedure TListView.SetAllowEmpty(AValue : boolean);
begin
	if AValue = FAllowNull then exit;
	FAllowNull := AValue;
	if (Not FAllowNull) and (FCurrent = nil) then begin
		Bottom;
	end;
end;

function TListView.GetAllowEmpty : boolean;
begin
	GetAllowEmpty := FAllowNull;
end;

procedure TListView.EndUpdate;
begin
	if not FUpdating then exit;
	CheckMarkers(not FAllowNull);
	inherited EndUpdate;
end;

procedure TListView.DeleteItem;
var
	TP, TN : PListItem;
begin
	TP := FTop^.Prior;
	TN := FTop^.Next;
	if AItem = FCurrent then begin
		if Assigned(FCurrent^.Next) then begin 
			FCurrent := FCurrent^.Next;
			{Inc(FIndex); Will be wrong because it is being moved up one }
		end else if Assigned(FCurrent^.Prior) then begin
			FCurrent := FCurrent^.Prior;
			Dec(FIndex);
			if Assigned(TP) then 
				FTop := TP;
		end else 
			ClearItems;
	end;
	if FItems.Count > 0 then begin
		FItems.Remove(AItem);
		if FTop = AItem then begin
			if Assigned(TP) then
				FTop := TP
			else
				FTop := TN;
		end;
		Dispose(AItem, Destroy);
	end;
	FVScroll.SetValue(FIndex); 
	AdjustSize;
	Update;
end;

procedure TListView.AdjustSize; 
begin
	inherited AdjustSize;
	AdjustBars;
end;

{$ENDIF}
