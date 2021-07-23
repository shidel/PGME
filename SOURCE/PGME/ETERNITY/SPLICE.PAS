{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$M 50000,0,655350}
program ConfigFile_Splicer;

uses QCrt, QObjects;

var
	C : TConfigFile;
	I : integer;

begin
	TurnMouseOff;
	if ParamCount < 0 then exit;
	C.Create('CONFIG');
	C.FAutoClear := False;
	C.FAutoMerge := False;
	for I := 1 to ParamCount do	begin
		WriteLn('Reading ', ParamStr(I));
		C.SetFileName(ParamStr(I));
		C.Read;
	end;
	C.SetFileName(ParamStr(1));
	WriteLn('Writing ', ParamStr(1));
	C.Write;
	C.Destroy;
end.
