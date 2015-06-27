{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Config_File_Toolkit; { X Series }

uses QObjects, QStrings, QErrors, QDos;

{$I ..\ETERNITY\LICENSE.INC}

type
	TOption = record
		Flag : integer;
		SPM : Boolean;
		Short : Str2;
		Long  : Str10;
		Desc : Str80;
	end;

const
	AppTitle = 'Eternity Configuration File Toolkit';
	AppID = 'QCNFTOOL';
	
	flNone		= 0;
	flMerge 	= 1;
	flClear		= 2;
	flVerbose	= 3;
	flOutName   = 4;
	flSave		= 5;
	
	NoFileName : Str12 = 'NONAME.CFG';
	Switch 	   : Str1 = '/';
	MaxOptions = 5;
	Options : array[0..MaxOptions] of TOption = (
		( Flag:flNone;    SPM:False; Short:''; Long:''; Desc:''),
		( Flag:flMerge;   SPM:False; Short:'M'; Long:'MERGE';   
			Desc:'Merge FILENAME another file into the current.'),
		( Flag:flClear;   SPM:False; Short:'C'; Long:'CLEAR';   
			Desc:'Purge all data'),
		( Flag:flVerbose; SPM:True;  Short:'V'; Long:'VERBOSE'; 
			Desc:'Be verbose about doing stuff.'),
		( Flag:flOutName; SPM:False; Short:'O'; Long:'OUTPUT';  
			Desc:'Set Output FILENAME.'),
		( FLag:flSave; 	  SPM:False; Short:'S'; Long:'SAVE'; 	
			Desc:'Write changes to disk now.')
	);
		

var
	Config : TConfigFile;
	Verbose : boolean;
	FlagValue : integer;
	StateValue : boolean;
	
procedure Help;
var
	I : integer;
	M1, M2 : integer;
begin
	WriteLn('Usage: ', AppID, ' FILE [options...]');
	WriteLn;
	WriteLn(AppTitle, ' (', AppID, ') is for manipulating PGME');
	Writeln('text based data files. These files are similar to windows INI files and');
	WriteLn('may also be edited using a simple text editor.');
	WriteLn;
	M1 := 0;
	M2 := 0;
	for I := 0 to MaxOptions do begin
		if Length(Options[I].Short) > M1 then 
			M1 := Length(Options[I].Short);
		if Length(Options[I].Long) > M2 then 
			M2 := Length(Options[I].Long);
	end;
	for I := 0 to MaxOptions do 
		if Options[I].Flag <> flNone then begin
			Write(LSpace(Options[I].Short, M1));
			if (Options[I].Short <> '') and (Options[I].Long <> '') then
				Write(', ')
			else 
				Write('  ');
			Write(RSpace(Options[I].Long, M2));
			Write(' ', Options[I].Desc);
		end;
	Halt(0);
end;

procedure Abort(Code : integer);
begin
	Halt(Code);
end;

procedure TestSwitches(Opt:String);
var
	I : integer;
	F : boolean;
begin
	Delete(Opt, 1, 1);
	Opt := UCase(Opt);
	F := False;
	FlagValue := flNone;
	StateValue := True;	
	if Opt[Length(Opt)] = '+' then begin
		F := True;
		Delete(Opt, Length(Opt), 1);
	end;
	if Opt[Length(Opt)] = '-' then begin
		F := True;
		StateValue := False;
		Delete(Opt, Length(Opt), 1);
	end;
	for I := 0 to MaxOptions do 
		if (Opt = Options[I].Short) or (Opt = Options[I].Long) then begin
			FlagValue := Options[I].Flag;
			if F and not Options[I].SPM then 
				Abort(erInvalid_Function_Number);
			Break;
		end;
end;

procedure Option(Opt : String);
var
	S : String;
	TempFlag, State : boolean;
begin
	if Trim(Opt) = '' then exit;
	if (FlagValue <> flNone) or ((Length(Opt) > 1) and (Opt[1] <> Switch)) then begin
		if FlagValue = flOutname then begin
			Opt := UCase(Opt);
			if Verbose then WriteLn('Set output to ', Opt);
			Config.FFileName := Opt;
		end else if (FlagValue = flMerge) or (FlagValue = flNone) then begin 
			S := Config.GetFileName;
			Opt := UCase(Opt);
			if FileExists(Opt) then begin
				Config.FFileName := Opt;
				if Verbose then WriteLn('Reading ', Opt);
				TempFlag := Config.FList.Count <> 0 ;
				if not Config.Read then
					Abort(erDisk_Read_Error);
				Config.FFileName := S;
				if TempFlag then
					Config.FModified := True;
			end else begin
				WriteLn('File not found: ', Opt);
			end;
		end else 
			Abort(erInvalid_Function_Number);
		FlagValue := flNone;
	end else if (Opt[1] = Switch) then begin
			TestSwitches(Opt);
			if FlagValue = flNone then 
				Abort(erInvalid_Function_Number);
			
			if (FlagValue = flClear) then begin
				FlagValue := flNone;
				if Verbose then WriteLn('Clearing data.');
				Config.Clear;
			end else if (FlagValue = flVerbose) then begin
				FlagValue := flNone;
				WriteLn('Verbose ', BoolStr(StateValue));
				Verbose := StateValue;
			end else if (FlagValue = flSave) then begin
				if Verbose then WriteLn('Saving ', Config.GetFileName);
				if not Config.Write then 
					Abort(erDisk_Write_Error);
			end;
	end else begin
		Abort(erInvalid_Function_Number);
	end;
end;

procedure Parse;
var
	I : integer;
begin
	Verbose := False;
	Config.Create('');
	Config.Clear;
	Config.FFileName := NoFileName;
	Config.FAutoClear := False;
	Config.FAutoMerge := False;
	FlagValue := flNone;
	StateValue := True;
	
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
