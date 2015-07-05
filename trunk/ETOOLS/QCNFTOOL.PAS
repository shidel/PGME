{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Config_File_Toolkit; { X Series }

uses QObjects, QStrings, QErrors, QDos, QFiles, QCrt;

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
	flList		= 14;
	flFile		= 15;
	flHelp		= 16;
	flComment	= 17;
	flStop		= 18;
	flPause		= 19;
	
	NoFileName : Str12 = 'NONAME.CFG';
	Switch 	   : Str1 = '/';
	MaxOptions = 22;
	Options : array[0..MaxOptions] of TOption = (
		( Flag:flNone;    SPM:False; Short:''; Long:''; Desc:''),
		( Flag:flVerbose; SPM:True;  Short:'V'; Long:'VERBOSE'; 
			Desc:'+/- set verbose level.'),
		( Flag:flHelp; SPM:False;  Short:'H'; Long:'HELP'; 
			Desc:'Display this help screen.'),
		( Flag:flComment; SPM:False;  Short:'#'; Long:'COMMENT'; 
			Desc:'Mostly only useful in files for commenting.'),
		( Flag:flNone;    SPM:False; Short:''; Long:''; Desc:''),
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
			Desc:'Print selection to display.'),
		( Flag:flList; SPM:False; Short:'L'; Long:'LIST';  
			Desc:'List sections, keys or values.'),		
		( Flag:flNone;    SPM:False; Short:''; Long:''; Desc:''),
		( Flag:flFile; SPM:False; Short:'F'; Long:'FILE';  
			Desc:'Process FILENAME line command line options.'),
		( Flag:flPause; SPM:False; Short:''; Long:'PAUSE';  
			Desc:'Pause processing and wait for keypress.'),		
		( Flag:flStop; SPM:False; Short:''; Long:'STOP';  
			Desc:'Stop processing and quit.')		
	);
		

var
	Config : TConfigFile;
	Export : TFile;
	OptFile : TFile;
	Verbose : boolean;
	FlagValue : integer;
	StateValue : boolean;
	Section : string;
	Key : String;
	CommentFlag : boolean;
		
procedure DoAPause;
begin
	Write('press a key to continue');
	pause;
	GotoXY(1,WhereY);
	ClrEol;
end;

procedure Help(Term : boolean);
var
	I : integer;
	M1, M2 : integer;
	S : String;
	
	procedure WriteHelp(Line : String);
	const 
		Count : integer = 0;
	begin
		Inc(Count);
		if Count = Hi(WindMax) then begin
			DoAPause;
			Count := 1;
		end;
		WriteLn(Line);
	end;
	
begin
	WriteHelp('Usage: ' + AppID + ' FILE [options...]');
	WriteHelp('');
	WriteHelp(AppTitle + ' (' + AppID + ') is for manipulating PGME');
	WriteHelp('text based data files. These files are similar to windows INI files and');
	WriteHelp('may also be edited using a simple text editor.');
	M1 := 0;
	M2 := 0;
	for I := 0 to MaxOptions do begin
		if Length(Options[I].Short) > M1 then 
			M1 := Length(Options[I].Short);
		if Length(Options[I].Long) > M2 then 
			M2 := Length(Options[I].Long);
	end;
	if M1 < 4 then M1 := 4;
	S := ' ';
	for I := 0 to MaxOptions do 
		if Options[I].Flag <> flNone then begin
			S := LSpace(Options[I].Short, M1 + 1);
			if (Options[I].Short <> '') and (Options[I].Long <> '') then
				S := S + ', '
			else 
				S := S + '  ';
			S := S + RSpace(Options[I].Long, M2);
			S := S + ' ' + Options[I].Desc;
			WriteHelp(S);
		end else if S <> '' then begin
			WriteHelp('');
			S := '';
		end;
	if Term then Halt(0);
end;

procedure Abort(Code : integer);
begin
	Config.Destroy;
	Export.Destroy;
	OptFile.Destroy;
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
	CommentFlag := False;
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

procedure ListKey(AKey : String);
var
	P : PListItem;
begin
	if AKey <> '' then begin
		P := Config.FindKey(AKey);
		if (not Assigned(P)) and Verbose then
			WriteLn('Key ', AKey, ' not present.')
		else begin
			P := P^.First;
			while Assigned(P) do begin
				WriteLn(P^.GetNameId);
				P := P^.Next;							
			end;
		end;
	end;
end;

procedure ListSection(ASection : String);
var
	P : PListItem;
begin
	Config.OpenSection(ASection);
	if Not Assigned(Config.FSectionKey) then begin
		if Verbose then WriteLn('Section ', ASection, ' not present.')
	end else begin
		P := Config.FSectionKey^.First;
		while Assigned(P) do begin
			WriteLn(P^.GetNameID);
			P := P^.Next;
		end;
	end;
	Config.OpenSection(Section);
end;

procedure ListAll;
var
	P : PListItem;
begin
	P := Config.FList.First;
	if Not Assigned(P) then
		WriteLn('File ', Config.FFileName, ' is empty.')
	else begin
		while Assigned(P) do begin
			WriteLn('[', P^.GetNameID, ']');
			P := P^.Next;
		end;
	end;
end;

procedure SetExportName;
var
	F : String;
begin
	F := Config.FFilename;
	if F = '' then F := NoFileName;
	if (Export.GetName <> F) and Verbose then
		WriteLn('Export Filename is ', F);	
	Export.Assign(F);
end;

procedure ExportLn(S : String);
var
	E : integer;
begin
	SetExportName;
	if FileExists(Config.FFileName) then begin
		Export.Reset;
		Export.SeekEOF;
	end else
		Export.Rewrite;
	Export.WriteLn(S);
	E := Export.IOResult;
	if E <> 0 then Halt(E);
	Export.Close;
end;

procedure ExportKey(AKey : String);
var
	P : PListItem;
begin
	if AKey <> '' then begin
		P := Config.FindKey(AKey);
		if (not Assigned(P)) then begin
			if Verbose then 
				WriteLn('Export Key ', AKey, ' not present.')
		end else if (P^.First = nil) then 
			ExportLn(AKey + '=')
		else begin
			P := P^.First;
			ExportLn(Space(5) + AKey + '=' + P^.GetNameId);
			P := P^.Next;
			while Assigned(P) do begin
				ExportLn(Space(10) + '+=' + P^.GetNameID);
				P := P^.Next;							
			end;
		end;
	end;
end;

procedure ExportSection(ASection : String);
var
	P : PListItem;
begin
	Config.OpenSection(ASection);
	if Not Assigned(Config.FSectionKey) then begin
		WriteLn('Export Section ', ASection, ' not present.')
	end else begin
		ExportLn('[' + ASection + ']');
		P := Config.FSectionKey^.First;
		while Assigned(P) do begin
			ExportKey(P^.GetNameID);
			P := P^.Next;
		end;
	end;
	Config.OpenSection(Section);
end;

procedure ExportAll;
var
	P : PListItem;
begin
	P := Config.FList.First;
	if Not Assigned(P) then begin
		WriteLn('Export no data.')
	end else begin
		while Assigned(P) do begin
			ExportSection(P^.GetNameID);
			P := P^.Next;
		end;
	end;
end;

procedure ParseFile(FileName : String); forward;

procedure Option(Opt : String);
var
	P : PListItem;
	S : String;
	TempFlag, State : boolean;
begin
	{if Trim(Opt) = '' then exit; }
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
		end else if FlagValue = flComment then begin
			if Verbose then begin
				if Not CommentFlag then begin
					WriteLn;
					WriteLn('Comment: ', Opt);
				end else 
					WriteLn('         ', Opt);
				CommentFlag := True;
			end;
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
		end else if FlagValue = flFile then begin
			InitVars;
			ParseFile(Opt);
			InitVars;
		end else if (FlagValue = flMerge) or (FlagValue = flNone) then begin 
			S := Config.FFileName;
			if S = '' then begin
				S := UCase(Opt);
				Config.FFileName := S;
			end;
			Opt := UCase(Opt);
			if FileExists(Opt) then begin
				Config.FFileName := UCase(Opt);
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
			
			if FlagValue <> flComment then
				CommentFlag := False;
				
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
			end else if FlagValue = flHelp then begin
				FlagValue := flNone;
				if Verbose then begin
					WriteLn('Print help text.');
				end;
				Help(False);
			end else if (FlagValue = flList) then begin
					FlagValue := flNone;
				if Key <> '' then 
					ListKey(Key)
				else if Section <> '' then 
					ListSection(Section)
				else 
					ListAll;
			end else if (FlagValue = flExport) then begin
					FlagValue := flNone;
				if Key <> '' then 
					ExportKey(Key)
				else if Section <> '' then 
					ExportSection(Section)
				else 
					ExportAll;
			end else if (FlagValue = flVerbose) then begin
				FlagValue := flNone;
				WriteLn('Verbose ', BoolStr(StateValue));
				Verbose := StateValue;
			end else if (FlagValue = flPause) then begin
				FlagValue := flNone;
				DoAPause;
			end else if (FlagValue = flStop) then begin
				FlagValue := flNone;
				if Verbose then Write('***** ');
				WriteLn('Stop command issued.');
				Abort(0);
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

var
	OptHold : String;
	
procedure OptionFile(Opt : String; Last : boolean);
begin
	OptHold := Trim(OptHold + ' ' + Opt);
	if Last then 
		Option(OptHold)
	else if OptHold <> '' then begin
		if (OptHold[1] <> '"') then begin
				Option(OptHold);
				OptHold := '';		
		end else if ((Length(OptHold) > 1) and (OptHold[Length(OptHold)] = '"')) then
			begin
				Option(Copy(OptHold, 2, Length(OptHold) - 2));
				OptHold := '';
			end;
	end;
end;

procedure ParseFile(FileName : String); 
var
	E : integer;
	Line, Opt : String;
begin
	OptHold := '';
	if Verbose then 
		WriteLn('Parse options from ', FileName);
	OptFile.Assign(FileName);
	OptFile.Reset;
	E := OptFile.IOResult;
	if E = 0 then begin
		Opt := '';
		while not OptFile.EOF do begin
			OptFile.ReadLn(Line, Sizeof(Line) - 1);
			E := OptFile.IOResult;
			if E <> 0 then begin
				OptFile.Close;
				Abort(E);
			end;
			while Line <> '' do begin
				Opt := Trim(PullStr(' ', Line));
				if Opt <> '' then  OptionFile(Opt, False);
			end;
		end;
		if OptHold <> '' then OptionFile('', True);
		OptFile.Close;
	end else begin
		OptFile.Close;
		Abort(E);
	end;
end;

procedure Parse;
var
	S : String;
	I : integer;
begin
	Verbose := False;
	Config.Create('');
	Export.Create;
	OptFile.Create;
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
	if Config.GetModified and Verbose then 
		Writeln('Warning there changes to not saved.');
	Config.Destroy;
	Export.Destroy;
	OptFile.Destroy;
end;

begin
	TurnMouseOff;
	DirectVideo := False;
	ErrorClrScr := False;
	if ParamCount = 0 then 
		Help(True)
	else
		Parse;
end.
