{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ ---THelp --- }

{$IFDEF INTERFACE}
const
	class_THelp = 'THelp';

type
	PHelp = ^THelp;
	THelp = object(TWindow)
    public { protected }
        FBlankLines : integer;
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FText : TTextView;
		procedure SetCurrentBounds(AValue : TBounds); virtual;
		procedure AddHelpFrom(AControl : PCOntrol);
	public
		constructor Create(AParent : PControl; AName : String);

		procedure ShowHelp(AValue : PControl); virtual;
		procedure BuildHelp; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
const
	HelpSection : PControl = nil;

procedure FormatHelpTitle (AVisual : PControl; var AValue : String); far;
var
	D : TFormatData;
	P : PControl;
begin
	D.ID := fmtString;
	D.StringValue := '';
	P := HelpSection;
	while Assigned(P) and (D.StringValue = '') do begin
		if P^.ClassOf(class_TApplication) then
			D.StringValue := Trim(PApplication(P)^.FInfo.Title)
		else if P^.ClassOf(class_TWindow) then
			D.StringValue := Trim(PWindow(P)^.FTitle.GetCaption);
		P := PControl(P^.Parent);
	end;
	if D.StringValue = '' then D.StringValue := 'Program';
	AValue := FormatStr(AValue, D, 1);
end;

function THelp.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_THelp) then
		ObjectClass := class_THelp
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor THelp.Create;
var
	B : TBounds;
	C : integer;
begin
	C := TextAttr;
	TextAttr := $07;
	inherited Create(AParent, AName);
	FTitle.SetCaption('0; Help on "%0" ');
	FTitle.SetTextAttr($74);
	FTitle.OnFormat := FormatHelpTitle;
    FBlankLines := 0;
	Bounds(1, 1, 70, Hi(ScreenMax) - 1,B);
	SetBounds(B);

	FStyle := bxSingle;
	SetAnchors(abTop or abBottom);

	FAutoEndModal := True;
	TextAttr := C or $08;
	FVisible := False;
	FText.Create(@Self, 'TEXT');
	FText.SetFreeOnDestroy(False);
	FText.SetAnchors(alClient);
	FText.SetScrollEdge(True);
	FText.Sorted := False;
	HelpSection := nil;
	TextAttr := C;
end;

procedure THelp.SetCurrentBounds(AValue : TBounds);
begin
	if (AValue.Height > FText.FItems.Count + 3) then begin
		AValue.Height := FText.FItems.Count + 3;
		if AValue.Height < 5 then AValue.Height := 5;
	end;
	if Assigned(Parent) then begin
		AValue.Left := (PControl(Parent)^.Width div 2) - (AValue.Width div 2) + 1;
		AValue.Top := (PControl(Parent)^.Height div 2) - (AValue.Height div 2) + 1;
	end else begin
		AValue.Left := (Lo(ScreenMax) div 2) - (AValue.Width div 2) + 1;
		AValue.Top := (Hi(ScreenMax)  div 2) - (AValue.Height div 2) + 1;
	end;
	inherited SetCurrentBounds(AValue);
end;

procedure THelp.ShowHelp(AValue : PControl);
begin
	HelpSection := AValue;
	if (Not Assigned(HelpSection)) or (HelpSection = @Self) then begin
		HelpSection := Application;
		Hide;
	end;
	StartDrawing;
	BuildHelp;
	FinishDrawing;
	SetCurrentBounds(FRBounds);
	ShowModal;
	FText.ClearItems;
	HelpSection := nil;
end;

procedure THelp.AddHelpFrom(AControl : PControl);
var
	CommandMap : PCodeMap;
	CommandCount : word;
	L : PListItem;
	D : array[0..6] of TFormatData;
	P : PControl;
	S, T : String;
	W : integer;
begin
	GetCommandMap(CommandMap, CommandCount);
	L := AControl^.FHelpList.First;
	if Assigned(L) then begin
        Inc(FBlankLines);
        if FBlankLines < 2 then
            FText.AddItem('');
    end;
	D[0].ID := fmtString;
	D[0].StringValue := AControl^.GetNameID;
	D[1].ID := fmtString;
	D[1].StringValue := AControl^.GetClassID;
	D[2].ID := fmtString;
	D[2].StringValue := Application^.GetCommandName(AControl^.FCommand);
	D[3].ID := fmtString;
	D[3].StringValue := Application^.GetCommandName(AControl^.FRespond);
	D[4].ID := fmtString;
	D[4].StringValue := Application^.GetKeyName(AControl^.FKeyValue);
	D[5].ID := fmtString;
	if AControl^.ClassOf(class_TLabel) then
		D[5].StringValue := Trim(PLabel(AControl)^.GetCaption)
	else
		D[5].StringValue := '';
	if D[5].StringValue = '' then
		D[5].StringValue := WCase(D[0].StringValue);
	S := '';
	while Assigned(L) or (S <> '') do begin
		if Assigned(L) then
			T := FormatStr(L^.GetNameID, D, 4)
		else
			T := '';
		if T = '' then begin
			if S <> '' then begin
				FText.AddItem(S);
				S := '';
			end;
		end;

		if S <> '' then
			S := S + ' ' + T
		else
			S := T;

		if (S = '') then begin
		    Inc(FBlankLines);
		    if FBlankLines < 2 then
        		FText.AddItem('');
        end else
            FBlankLines := 0;

		while Length(S) > FText.Width do begin
			W := FText.Width;
			while (W > 0) and (S[W] <> ' ') do dec(W);
			if W = 0 then W := FText.Width;
			T := Copy(S, 1, W);
			S := LTrim(Copy(S, Length(T) + 1, Length(S)));
			FText.AddItem(T);
		end;

		if Assigned(L) then
			L := L^.Next;
	end;
	P := PControl(AControl^.First);
	while assigned(P) do begin
		if P^.FShown then AddHelpFrom(P);
		P := PControl(P^.Next);
	end;
end;

procedure THelp.BuildHelp;
begin
	FText.ClearItems;
	FBlankLines := 0;
	AddHelpFrom(HelpSection);
end;
{$ENDIF}
