{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Nothing_Tool; { X Series }

{uses QObjects;}

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'Eternity Nothing Utility';
	AppID = 'QNOTHING';

	
begin
	if ParamCount = 0 then begin
		WriteLn(AppTitle);
		WriteLn;
		WriteLn('This program does nothing at all and never will.');
		WriteLn('To hide this message, just give it parameters.');
	end;
end.
