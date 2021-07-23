{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TKeyStroke --- }

{$IFDEF INTERFACE}
const
	class_TKeyStroke = 'TKeyStroke';
	
type
	PKeyStroke = ^TKeyStroke;
	TKeyStroke = object (TControl)
	private
		FLastCode : TEvent;
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public  { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		
	public
		constructor Create(AParent : PControl; AName : String);
		procedure Draw; virtual;		
	end;
	{$ENDIF}
	
{$IFDEF IMPLEMENTATION}

function TKeyStroke.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TKeyStroke) then 
		ObjectClass := class_TKeyStroke
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TKeyStroke.Create(AParent : PControl; AName : String);
var
	Temp : TBounds;
begin
	inherited Create(AParent, AName);
	SetVisible(False);
	Bounds(1, 1, 32, 2, Temp);
	SetBounds(Temp);
	FillChar(FLastCode, 0, Sizeof(FLastCode));
end;

procedure TKeyStroke.Draw;
begin
	inherited Draw;
	if FLastCode.What = evKeyboard then begin
		GotoXY(1,1);
		FWriteLn(' ScanCode: ' + ZPad(BinStr(FLastCode.ShiftCode), 16));
		FWrite ('  KeyCode: ' );
		if FLastCode.Original <> 0 then
			FWrite  (ZPad(HexStr(FLastCode.Original), 4) + '->');
		FWrite  (ZPad(HexStr(FLastCode.KeyCode), 4) + '+' + ZPad(HexStr(FLastCode.ShiftCode),4));
	end else if FLastCode.What = evCommand then begin
		FWriteLn(' Command: ' + Application^.GetCommandName(FLastCode.Command) );
	end;
end;
		
procedure TKeyStroke.CheckLocal(var AEvent : TEvent); 
begin	
	if not GetEnabled then exit;
	FLastCode := AEvent;
	if GetShown then Update;
	inherited CheckLocal(AEvent);
end;

{$ENDIF}
