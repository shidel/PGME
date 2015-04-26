{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Font_Loader; { X Series }

uses QCrt, QErrors, QStrings, QObjects;

{$I ..\ETERNITY\LICENSE.INC}

const
	class_TFontLoader = 'TFontLoader';
	
	AppTitle = 'Eternity Font Loader';
	AppID = 'ELOADFNT';

		
procedure Execute;
var
	Font : TFont;
begin
	Font.Create('FONT');
	if ParamStr(1) = '' then 
		WriteLn('Please Specify a QuickCrt font file to load.')
	else begin
		Font.SetFileName(ParamStr(1));
		if Font.GetFileName = '' then 
			WriteLn('Invalid or corrupt font file.')
		else begin
			UserFontSize := Font.GetHeight;
			UserFontPtr := Font.FFontPtr;
			TextMode(CO80 or FontUser);
		end;		
	end;
	
	Font.Destroy;
end;
		 
begin
	Execute; 
end.
