{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ESAVER.DEF}
program Eternity_Screen_Saver; { X Series }

uses QCrt, QScrnSvr;

{$I ..\ETERNITY\LICENSE.INC}

type
	TESaver = object (TScreenSaver)
	end;

var
	ESaver : TESaver;

begin
    IdleMethod := imAny;
	ESaver.Create;
	ESaver.Execute;
	ESaver.Destroy;
end.
