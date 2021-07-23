{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TMessageBox --- }

{$IFDEF INTERFACE}
const
	class_TMessageBox = 'TMessageBox';
	
type
	PMessageBox = ^TMessageBox;
	TMessageBox = object(TDialog)
	private
		FMsgPtr : PListItem;
		FColors	: integer;
		FFmtData : Pointer;
		FFmtCount : Integer;
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure SetMessage(AValue : String); virtual;
		procedure AppendMessage(AValue : String); virtual;
		procedure SetFormatData(const AData; ACount : byte); virtual;
		procedure Draw; virtual;
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TMessageBox.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TMessageBox) then 
		ObjectClass := class_TMessageBox
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TMessageBox.Create;
{var
	M : TMargins; }
begin
	inherited Create(AParent, AName);
	FMsgPtr := nil;
	FColors := TextAttr;
	FFmtCount := 0;
	FFmtData := nil;
end;

destructor TMessageBox.Destroy;
begin
	if Assigned(FFmtData) then begin
		FreeMem(FFmtData, FFmtCount * Sizeof(TFormatData));
		FFmtData := nil;
		FFmtCount := 0;
	end;
	if Assigned(FMsgPtr) then begin
		Dispose(FMsgPtr, Destroy);
		FMsgPtr := nil;
	end;
	inherited Destroy;
end;

procedure TMessageBox.SetFormatData(const AData; ACount : byte); 
begin
	if Assigned(FFmtData) then begin
		FreeMem(FFmtData, FFmtCount * Sizeof(TFormatData));
		FFmtData := nil;
		FFmtCount := 0;
	end;
	FFmtCount := ACount;
	if MaxAvail < FFmtCount * Sizeof(TFormatData) then halt(8); 
	GetMem(FFmtData, FFmtCount * Sizeof(TFormatData));
	Move(AData, FFmtData^, FFmtCount * Sizeof(TFormatData));
end;


procedure TMessageBox.AppendMessage(AValue : String);
var
	I : integer;
	S : String;
begin
	if AValue <> '' then begin
		if Not Assigned(FMsgPtr) then begin
			if MaxAvail < Sizeof(TListItem) then Halt(8);
			FMsgPtr := New(PListItem, Create(''));
			FMsgPtr^.Sorted := False;
		end;
		while (AValue <> '') do begin
			I := Pos(#13, AValue);
			if I > 0 then begin
				S := Copy(AValue, 1, I - 1);
				AValue := Copy(AValue, I + 1, Length(AValue));
			end	else begin
				S := AValue;
				AValue := '';
			end;
			if MaxAvail < SizeOf(TListItem) + Length(S) + 1 then Halt(8);
			FMsgPtr^.Add(New(PListItem, Create(S)));
		end;
	end;
	Refresh;
end;

procedure TMessageBox.SetMessage(AValue : String);
begin
	if Assigned(FMsgPtr) then begin
		Dispose(FMsgPtr, Destroy);
		FMsgPtr := nil;
	end;
	AppendMessage(AValue);
end;

procedure TMessageBox.Draw;
var
	P : PListItem;
begin
	inherited Draw;
	if Assigned(FMsgPtr) then begin
		P := FMsgPtr^.First;
		while Assigned(P) do begin
			if Assigned(FFmtData) then
				FWriteLn(Space(2) + FormatStr(P^.GetNameID, FFmtData^, FFmtCount)) { FFmtCount is integer not byte, but i don't care }
			else
				FWriteLn(Space(2) + P^.GetNameID);
			P := P^.Next;
		end;
	end;
end;

procedure TMessageBox.ApplyLanguage( AConfig : PConfigFile );
var
	P, M : PListItem;
	S : String;
begin
	inherited ApplyLanguage(AConfig);
	P := AConfig^.FindKey(GetPathID + '.MESSAGE');
	if Assigned(P) then begin
		SetMessage('');
		P := P^.First;
		while Assigned(P) do begin
			AppendMessage(P^.GetNameId);
			P := P^.Next;
		end;
	{$IFDEF TEMPLATES}
	end else begin
		AConfig^.SetValue(GetPathID + '.MESSAGE', '');
		P := AConfig^.FindKey(GetPathID + '.MESSAGE');
		if Assigned(P) and Assigned(FMsgPtr) then begin
			P^.Clear;
			M := FMsgPtr^.First;
			while Assigned(M) do begin
				S := M^.GetNameID;
				if MaxAvail < Sizeof(TListItem) + Length(S) + 1 then Halt(8);
				P^.Add(New(PListItem, Create(S)));
				M := M^.Next;
			end;
		end;
	{$ENDIF}
	end;
end;

procedure TMessageBox.ApplyTheme( AConfig : PConfigFile );
var
	Temp : Integer;
begin
	inherited ApplyTheme(AConfig);
	AConfig^.GetInts('MSGCOLOR', FColors, SizeOf(FColors) div SizeOf(Integer));
end;

{$ENDIF}
