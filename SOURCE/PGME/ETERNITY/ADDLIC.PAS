{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$M 20480,0,0}
program Release_License_Detector_Calculator;

uses QCrt, QFiles, QStrings;

procedure SetLicense(License : integer);
var
	F : Text;
begin
	Assign(F, 'LICENSE.INC');
	Rewrite(F);
	WriteLn(F, '{ This file is automatically generated by make install. DO NOT EDIT IT! }');
	WriteLn(F, '');
	WriteLn(F, 'const');
	WriteLn(F, #9'SoftwareLicense : String =');
	case License of
		2 : begin
        	TextColor(LightGreen);
			Write('GPLv2.0 License');
			TextColor(LightGray);
			WriteLn;
			WriteLn(F, #9#9#$27 + 'This project and related files are subject to the terms of the' + #$27 + '#13 +');
			WriteLn(F, #9#9#$27 + 'GNU General Public License, version 2.0. If a copy of the GPL' + #$27 + '#13 +');
			WriteLn(F, #9#9#$27 + 'was not distributed with this file, You can obtain one at:' + #$27 + '#13 +');
			WriteLn(F, #9#9#$27 + 'http://www.gnu.org/licenses/gpl-2.0.txt.' + #$27 + ';');
		end;
		1000 : begin
        	TextColor(LightMagenta);
			Write('Commercial License');
			TextColor(LightGray);
			WriteLn;
			Delay(2000);
			WriteLn(F, #9#9#$27 + 'This project and related files are subject to the terms ' + #$27 + '#13 +');
			WriteLn(F, #9#9#$27 + 'specified in the included LICENSE.TXT file. If a copy of that' + #$27 + '#13 +');
			WriteLn(F, #9#9#$27 + 'file was not distributed with this file, You can obtain one by ' + #$27 + '#13 +');
			WriteLn(F, #9#9#$27 + 'contacting the distributor of this program' + #$27 + ';');
		end;
	else
        TextColor(LightRed);
		Write('Unlicensed');
        TextColor(LightGray);
        WriteLn;
        Delay(5000);
		WriteLn(F, #9#9#$27 + 'This project and related files are subject to either the terms' + #$27 + '#13 +');
		WriteLn(F, #9#9#$27 + 'specified in the included LICENSE.TXT file or the GNU GPLv2.0.'  + #$27 + '#13 +');
		WriteLn(F, #9#9#$27 + 'Neither file was present during the creation of this program,' + #$27 + '#13 +');
		WriteLn(F, #9#9#$27 + 'making it an invalid and possibly illegal copy.' + #$27 + ';');
	end;
	Close(F);
	Delay(1000);
	halt(0);
end;

procedure Detect(Filename: String; License : integer);
var
	F : TFile;
	E : integer;
	S : String;
	GPL : Boolean;
	Ver : integer;
	M : word;
	I : integer;
	function Has ( SubStr : String ) : boolean;
	begin
	    Has := Pos(UCase(SubStr), S) > 0;
	end;
begin
    GPL := False;
    Ver := 0;
    M := FileMode;
    FileMode := 0;
    F.Create;
	F.Assign(Filename);
	F.Reset;
	E := F.IOResult;
	I := 0;
	While (Not F.EOF) and (E = 0) do begin
	    Inc(I);
	    F.ReadLn(S, Sizeof(S) - 1);
	    if I > 5 then begin
	        F.Close;
	        F.Destroy;
	        Break;
	    end;
	    E := F.IOResult;
	    S := ReplaceStr(S, #9, ' ', True);
	    S := ReplaceStr(S, '  ', ' ', True);
	    S := UCase(Trim(S));
	    if Has('GNU GENERAL PUBLIC LICENSE') then
	        GPL := True
	    else if GPL then begin
	        if Has('Version 2') then
	            Ver := 2
	        else
	           Break;
	    end else
	        Break;
    end;
	F.Close;
	F.Destroy;
	FileMode := M;
	if (E = 0) then begin
	    if GPL and (Ver = 2) then
    	    SetLicense(2)
	    else
	        SetLicense(License);
	end;
end;


begin
	{ Calculate EXE Size }
	Detect('..\gpl-2.0.txt', 2);    { GPL License }
	Detect('..\gpl-20.txt', 2);     { GPL License }
	Detect('..\gpl20.txt', 2);  	{ GPL License }
	Detect('..\license.txt', 1000); { Commercial Release }
	SetLicense(0); 			  	    { Unlicensed }

end.
