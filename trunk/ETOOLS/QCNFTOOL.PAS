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
	flPrint    	= 2;
	flVerbose	= 3;
	flOutName   = 4;
	flSave		= 5;
	flAutoMerge = 6;
	flSection   = 7;
	flKey		= 8;
	flValue		= 9;
	flUnSelect  = 10;
	flDelete	= 11;
	flExport    = 12;
	flAppend 	= 13;
	
	NoFileName : Str12 = 'NONAME.CFG';
	Switch 	   : Str1 = '/';
	MaxOptions = 14;
	Options : array[0..MaxOptions] of TOption = (
		( Flag:flNone;    SPM:False; Short:''; Long:''; Desc:''),
		( Flag:flVerbose; SPM:True;  Short:'V'; Long:'VERBOSE'; 
			Desc:'+/- set verbose level.'),
		( Flag:flMerge;   SPM:False; Short:'R'; Long:'READ';   
			Desc:'Read/merge FILENAME into the current data.'),
		( FLag:flSave; 	  SPM:False; Short:'W'; Long:'WRITE'; 	
			Desc:'Write changes to output file now.'),
		( Flag:flOutName; SPM:False; Short:'O'; Long:'OUTPUT';  
			Desc:'Set output FILENAME.'),
		( Flag:flAutoMerge; SPM:False; Short:'M'; Long:'AUTOMERGE';  
			Desc:'+/- set auto merging state.'),
		( Flag:flNone;    SPM:False; Short:''; Long:''; Desc:''),
		( Flag:flSection; SPM:False; Short:'S'; Long:'SECTION';  
			Desc:'Select section NAME.'),
		( Flag:flKey; SPM:False; Short:'K'; Long:'KEY';  
			Desc:'Select key NAME.'),
		( Flag:flValue; SPM:False; Short:'D'; Long:'DATA';  
			Desc:'Set DATA for key.'),
		( Flag:flAppend; SPM:False; Short:'A'; Long:'APPEND';  
			Desc:'Append DATA to key.'),
		( Flag:flUnSelect; SPM:False; Short:'U'; Long:'UNSELECT';  
			Desc:'Unselect section and key'),
		( Flag:flExport; SPM:False; Short:'X'; Long:'EXPORT';  
			Desc:'Export selection.'),
		( Flag:flDelete; SPM:False; Short:'E'; Long:'ERASE';  
			Desc:'Erase selection.'),
		( Flag:flPrint; SPM:False; Short:'P'; Long:'PRINT';  
			Desc:'Print selection to display.')
			
	);
		

var
	Config : TConfigFile;
	Verbose : boolean;
	FlagValue : integer;
	StateValue : boolean;
	Section : string;
	Key : String;
		
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
	M1 := 0;
	M2 := 0;
	for I := 0 to MaxOptions do begin
		if Length(Options[I].Short) > M1 then 
			M1 := Length(Options[I].Short);
		if Length(Options[I].Long) > M2 then 
			M2 := Length(Options[I].Long);
	end;
	if M1 < 4 then M1 := 4;
	for I := 0 to MaxOptions do 
		if Options[I].Flag <> flNone then begin
			Write(LSpace(Options[I].Short, M1 + 1));
			if (Options[I].Short <> '') and (Options[I].Long <> '') then
				Write(', ')
			else 
				Write('  ');
			Write(RSpace(Options[I].Long, M2));
			WriteLn(' ', Options[I].Desc);
		end else
			WriteLn;
	Halt(0);
end;

procedure Abort(Code : integer);
begin
	Config.Destroy;
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

procedure InitVars;
begin
	FlagValue := flNone;
	StateValue := True;
	Section := '';
	Key := '';
end;

procedure Save;
begin
	if (Config.FFileName = '') then 
		Config.FFileName := NoFileName;
	if Verbose then
			WriteLn('Writing ', Config.GetFileName);
	if not Config.Write then 
		Abort(erDisk_Write_Error);
end;

procedure PrintKey(AKey : String);
var
	P : PListItem;
begin
	if AKey <> '' then begin
		P := Config.FindKey(AKey);
		if (not Assigned(P)) then
			WriteLn('Key ', AKey, ' not present.')
		else if (P^.First = nil) then 
			WriteLn(AKey, '=')
		else begin
			P := P^.First;
			WriteLn(Space(5), AKey, '=', P^.GetNameId);
			P := P^.Next;
			while Assigned(P) do begin
				WriteLn(Space(10), '+=', P^.GetNameID);
				P := P^.Next;							
			end;
		end;
	end;
end;

procedure PrintSection(ASection : String);
var
	P : PListItem;
begin
	Config.OpenSection(ASection);
	if Not Assigned(Config.FSectionKey) then
		WriteLn('Section ', ASection, ' not present.')
	else begin
		WriteLn('[', ASection, ']');
		P := Config.FSectionKey^.First;
		while Assigned(P) do begin
			PrintKey(P^.GetNameID);
			P := P^.Next;
		end;
	end;
	Config.OpenSection(Section);
end;

procedure PrintAll;
var
	P : PListItem;
begin
	P := Config.FList.First;
	if Not Assigned(P) then
		WriteLn('File ', Config.FFileName, ' is empty.')
	else begin
		while Assigned(P) do begin
			PrintSection(P^.GetNameID);
			P := P^.Next;
		end;
	end;
end;

procedure Option(Opt : String);
var
	P : PListItem;
	S : String;
	TempFlag, State : boolean;
begin
	if Trim(Opt) = '' then exit;
	if (FlagValue <> flNone) or ((Length(Opt) > 1) and (Opt[1] <> Switch)) then begin
		if FlagValue = flOutname then begin
			Opt := UCase(Opt);
			if Verbose then WriteLn('Set output to ', Opt);
			Config.FFileName := Opt;
		end else if FlagValue = flSection then begin
			Key := '';
			Section := UCase(Opt);
			if Verbose then begin
				if Config.FList.FindID(Section) = nil then 
					WriteLn('Create section "', Section, '"')
				else
					WriteLn('Select section "', Section, '"');
			end;
			Config.OpenSection(Section);
		end else if FlagValue = flKey then begin
			Key := UCase(Opt);
			if Verbose then begin
				WriteLn('Select key "', Key, '"');
			end;
		end else if FlagValue = flValue then begin
			if Key = '' then 
				Abort(erParamError);
			Config.SetValue(Key, Opt);
			if Verbose then
				WriteLn('Setting data "', Opt, '"')
		end else if FlagValue = flAppend then begin
			if Key = '' then 
				Abort(erParamError);
			P := Config.FindKey(Key);
			if Assigned(P) then 
				P^.Add(New(PListItem, Create(Opt)))
			else
				Config.SetValue(Key, Opt);
			if Verbose then
				WriteLn('Appending "', Opt, '"');
			Config.FModified := True;
		end else if (FlagValue = flMerge) or (FlagValue = flNone) then begin 
			S := Config.FFileName;
			if S = '' then begin
				S := Opt;
				Config.FFileName := S;
			end;
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
			Abort(erParamError);
		FlagValue := flNone;
	end else if (Opt[1] = Switch) then begin
			TestSwitches(Opt);
			if FlagValue = flNone then 
				Abort(erParamError);
			
			if (FlagValue = flDelete) then begin
				FlagValue := flNone;
				if Key <> '' then begin
					Config.DeleteKey(Key);
					if Verbose then WriteLn('Removing key "', Key, '"');
					Key := '';
				end else if Section <> '' then begin
					Config.DeleteSection;
					if Verbose then WriteLn('Removing section "', Section, '"');
					Section := '';
					InitVars;
				end else begin
					if Verbose then WriteLn('Removing all data.');
					Config.Clear;
					InitVars;
				end;
			end else if (FlagValue = flPrint) then begin
					FlagValue := flNone;
				if Key <> '' then 
					PrintKey(Key)
				else if Section <> '' then 
					PrintSection(Section)
				else 
					PrintAll;
			end else if (FlagValue = flVerbose) then begin
				FlagValue := flNone;
				WriteLn('Verbose ', BoolStr(StateValue));
				Verbose := StateValue;
			end else if (FlagValue = flUnSelect) then begin
				InitVars;
				FlagValue := flNone;
				if Verbose then WriteLn('Unselecting section and key');
			end else if (FlagValue = flAutoMerge) then begin
				FlagValue := flNone;
				if Verbose then WriteLn('Automerge ', BoolStr(StateValue));
				Config.FAutoMerge := StateValue;
			end else if (FlagValue = flSave) then begin
				FlagValue := flNone;
				Save;
			end;
	end else begin
		Abort(erParamError);
	end;
end;

procedure Parse;
var
	S : String;
	I : integer;
begin
	Verbose := False;
	Config.Create('');
	Config.Clear;
	Config.FFileName := '';
	Config.FAutoClear := False;
	Config.FAutoMerge := False;
	InitVars;
	I := 0;
	S := '';
	repeat
		Inc(I);
		S := Trim(S + ' ' + ParamStr(I));
		if (S[1] = '"') then begin
			if S[Length(S)] = '"' then begin
				Option(Copy(S, 2, Length(S) - 2));
				S := '';
			end;
		end else begin
			Option(S);
			S := '';
		end;
	until (I = ParamCount);
	if (S <> '') or (FlagValue <> flNone) then 
		Abort(erParamError);		
	if Config.GetModified then Save;
	Config.Destroy;
end;

begin
	ErrorClrScr := False;
	if ParamCount = 0 then 
		Help
	else
		Parse;
end.
