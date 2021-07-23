{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Keymapping_Tool; { X Series }

uses QCrt, QStrings, QObjects;

{$I ..\ETERNITY\LICENSE.INC}
{$I ..\QUICKCRT\DCOMMAND.INC}
{$I ..\QUICKCRT\DKEYNAME.INC}
{$I ..\QUICKCRT\DKEYMAPS.INC}

const
	AppTitle = 'Eternity Key Mapping Utility';
	AppID = 'QKEYS';

var
	Event : TEvent;
	I : integer;
	
begin
	InitQCrtMaps;
	WriteLn('Simple ' + AppTitle);
	WriteLn('Press the "Escape" key to exit.');
	WriteLn;
	repeat
		ClearEvent(Event);
		while Event.What = evNothing do GetEvent(Event);
		Write('Event: ');
		if Event.What = evNothing then
  			WriteLn('Null')
  		else if Event.What and evMouse <> 0 then begin
  			Write('Mouse ');
  			if Event.What and evMouseDown = evMouseDown then
  				Write('[PRESS] ');
  			if Event.What and evMouseup = evMouseUp then
  				Write('[RELEASE] ');
  			if Event.What and evMouseMove = evMouseMove then
  				Write('[MOVE] ');
  			if Event.What and evMouseAuto = evMouseAuto then
  				Write('[AUTO] ');
			if Event.Double then 
				Write('[DOUBLE] ');
			WriteLn(Event.Where.X, ':', Event.Where.Y, ' (', BinStr(Event.Buttons), ')');  			
  		end else if Event.What = evKeyboard then begin
  			Write('Keyboard ');
  			Write('$' + ZPad(HexStr(Event.Keycode), 4), '+$', ZPad(HexStr(Event.ShiftCode),4), ' ');
  			Write('[');
  			if Event.Original <> 0 then 
  				Write('ORIGINAL:', ZPad(HexStr(Event.Original),4), ' ');
  			Write('Scancode:$', HexStr(Event.Scancode), ', ');
  			FWrite('CHAR:"' + Event.CharCode + '"');
			for I := 0 to DefaultKeyNamesMax do
				if Pos('$' + ZPad(HexStr(Event.Keycode),4), DefaultKeyNames[I]) = 1 then begin
					Write(' (', Copy(DefaultKeyNames[I], 7, Length(DefaultKeyNames[I]) ), ')');
					Break;
				end;
  			WriteLn(']');
  		end else if Event.What and evMessage <> 0 then begin
  			if Event.What and evMessage = evCommand then
  				Write('Command')
  			else if Event.What and evMessage = evBroadcast then
  				Write('Broadcast')
  			else if Event.What and evMessage = evSystem then
  				Write('System')
  			else
  				Write('Other Message');
  			Write(': Value=$', ZPad(HexStr(Event.Command),4));
  			Write(' Data:', ZPad(HexStr(Event.InfoLong),8));  
			for I := 0 to DefaultCommandNamesMax do
				if Pos('=$' + ZPad(HexStr(Event.Command),4), DefaultCommandNames[I]) > 0 then begin
					Write(' [cm', Copy(DefaultCommandNames[I], 1, Pos('=', DefaultCommandNames[I]) -1 ), ']');
					Break;
				end;
			WriteLn;	
  		end;
(* 
    evCommand   = $0100; { Command event }
    evBroadcast = $0200; { Broadcast event }
    evSystem    = $0400; { System Event }
    evMessage   = $FF00; { Message (command, broadcast, or user-defined) event }
*)					
	until ((Event.What = evKeyboard) and (Event.CharCode = Chr(kbEscape))) or
		((Event.What = evCommand) and (Event.Command = cmEscape));
end.
