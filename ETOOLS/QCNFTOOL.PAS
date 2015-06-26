{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Config_File_Toolkit; { X Series }

uses QObjects, QStrings, QErrors, QDos;

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'Eternity Configuration File Toolkit';
	AppID = 'QCNFTOOL';
	
	flMerge 	= 0;
	flOutName   = 1;
	flMax   	= 10;
	
var
	Config : TConfigFile;
	Verbose : boolean;
	Flags  : array[flMerge..flMax] of boolean;
	
procedure Help;
begin
	WriteLn('Usage: ', AppID, ' FILE [options...]');
	WriteLn;
	WriteLn(AppTitle, ' (', AppID, ') is for manipulating PGME');
	Writeln('text based data files. These files are similar to windows INI files and');
	WriteLn('may also be edited using a simple text editor.');
	WriteLn;
	WriteLn('	M, MERGE	Merge FILENAME another file into the current');
	WriteLn('	C, CLEAR	Purge all data.');
	WriteLn('	V, VERBOSE	Be verbose about doing stuff.');
	WriteLn('	O, OUTPUT	Set Output FILENAME.');	
	WriteLn('	S, SAVE		Write changes to disk now.');
	Halt(0);
end;

procedure Abort(Code : integer);
begin
	Halt(Code);
end;

procedure ClearFlags;
begin
	FillChar(Flags, Sizeof(Flags), False);
end;

function Flag(AFlag : integer) : boolean;
begin
	Flag := Flags[AFlag];
end;

procedure Option(Opt : String);
var
	S : String;
	F : boolean;
begin
	if Trim(Opt) = '' then exit;
	if (Opt[1] = '/') then begin
		if (Length(Opt) > 1) then begin
			Delete(Opt, 1, 1);
			Opt := UCase(Opt);
			if (Opt = 'M') or (Opt = 'MERGE') then begin
				Flags[flMerge] := True
			end else if (Opt = 'C') or (Opt = 'CLEAR') then begin
				if Verbose then WriteLn('Clearing data.');
				Config.Clear;
			end else if (Opt = 'O') or (Opt = 'OUTPUT') then begin
				Flags[flOutName] := True
			end else if (Opt = 'S') or (Opt = 'SAVE') then begin
				if Verbose then WriteLn('Saving ', Config.GetFileName);
				if not Config.Write then 
					Abort(erDisk_Write_Error);
			end else if (Opt = 'V') or (Opt = 'VERBOSE') then begin
				WriteLn('Verbose ON');
				Verbose := True;
			end;
		end else 
			Abort(erInvalid_Function_Number);
	end else begin
		if Flag(flOutname) then begin
			Opt := UCase(Opt);
			if Verbose then WriteLn('Set output to ', Opt);
			Config.FFileName := Opt;
		end else 
{		if Flag(flMerge) then } begin 
			S := Config.GetFileName;
			Opt := UCase(Opt);
			if FileExists(Opt) then begin
				Config.FFileName := Opt;
				if Verbose then WriteLn('Reading ', Opt);
				F := Config.FList.Count <> 0 ;
				if not Config.Read then
					Abort(erDisk_Read_Error);
				Config.FFileName := S;
				if F then
					Config.FModified := True;
			end else begin
				WriteLn('File not found: ', Opt);
			end;
		end;
		ClearFlags;
	end;
end;

procedure Parse;
var
	I : integer;
begin
	Verbose := False;
	Config.Create('');
	Config.FAutoClear := False;
	Config.FAutoMerge := False;
	ClearFlags;
	Flags[flMerge] := True;
	
	for I := 1 to ParamCount do begin
		Option(ParamStr(I));
	end;
	if Config.GetModified and (Config.GetFileName <> '') then begin
		if Verbose then
			WriteLn('Saving ', Config.GetFileName);
		if not Config.Write then 
			Abort(erDisk_Write_Error);
	end;
	Config.Destroy;
end;

begin
	ErrorClrScr := False;
	if ParamCount = 0 then 
		Help
	else
		Parse;
end.
