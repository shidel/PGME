{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ Excludes A-Z, a-z, and 0-9, Because we assume that they are their own names. }
const
	DefaultKeyNamesMax = 48;
	DefaultKeyNames : array[0..DefaultKeyNamesMax] of String[25] = (
  		'$001B=Escape',
    	'$4700=Home',
    	'$8500=F11',
    	'$8600=F12',
		'$4800=Up', 
		'$4900=Page Up', 
		'$4B00=Left',
		'$4D00=Right',
		'$4F00=End', 
		'$5000=Down', 
		'$5100=Page Down', 
		'$5200=Insert',
		'$5300=Delete',
		'$000D=Enter/Return', 
		'$0009=Tab', 
		'$0020=Space',
		'$0008=BackSpace',
		'$002D=Minus',
		'$005F=Underscore',
		'$003D=Equals',
		'$002B=Plus',
		'$0021=Exclamation',
		'$0040=At',
		'$0023=Pound',
		'$0024=Dollar',
		'$0025=Percent',
		'$005E=Caret',
		'$0026=Ampersand',
		'$002A=Star',
		'$0028=Open Parentheses',
		'$0029=Close Parentheses',
		'$005B=Open Bracket',
		'$005D=Close Bracket',
		'$007B=Open Brace',
		'$007D=Close Brace',
		'$005C=Forward Slash',
		'$007C=Pipe',
		'$003A=Colon',
		'$003B=Semicolon',
		'$0027=Single Quote',
		'$0022=Double Quote',
		'$002C=Comma',
		'$002E=Period',
		'$0060=Quote',
		'$007E=Tilda',
		'$002F=Backslash',
		'$003C=Less Than',
		'$003E=Greater Than',
		'$003F=Question Mark'
	);
	
function KeyAsStr(AValue : TKeyValue; Flags : word) : String;
var
	S : String;
begin
	S := '$' + ZPad(HexStr(AValue.KeyCode),4) + ':';
	if Flags = $0001 then 
		S := S + 'C'
	else if Flags = $0002 then 
		S := S + 'M'
	else
		S := S + 'A';
	KeyAsStr := S + '+$'+ZPad(HexStr(AValue.ShiftCode),4);
end;

procedure StrAsKey(Str : String; var AValue : TKeyValue; var Flags : word );
var
	I : integer;
	Scan, Flag : String;
begin
	Flag := '';
	Scan := '$0000';
	I := Pos(':', Str);
	if I > 0 then begin
		Flag := Ucase(Str[I + 1]); { I don't care if user messes it up }
		Delete(Str, I, 2);
	end;
	I := Pos('+', Str);
	if I > 0 then begin
		Scan := Copy(Str, I + 1, Length(Str)); { I don't care if user messes it up }
		Delete(Str, I, Length(Scan) + 1 );
	end;
	AValue.KeyCode := StrInt(Str);
	AValue.ShiftCode := StrInt(Scan);
	if Flag = 'C' then 
		Flags := $0001
	else if Flag = 'M' then 
		Flags := $0002
	else
		Flags := $0000;
end;
