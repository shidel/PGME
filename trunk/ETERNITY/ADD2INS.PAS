{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$M 50000,0,655350}
program Install_Exe_Size_Calculator;

uses QStrings, QDos, QObjects;

{$I INSFILE.INC}

const
	WorkFile : String = 'INSTALL.TMP';
	WorkPath : String = '';
	Count	: LongInt = 0;
	Total	: LongInt = 0;
	INSSize	: LongInt = 0;

var
	SRec : TSearchRec;
	Exclude : TListItem;

procedure Append(FileName : String; IGroup : word);
var
	Buf : array[0..255] of byte;
	FI, FO : File;
	SI : LongInt;
	C : Integer;
	R : TInsFIleHeader;
	N : TSearchRec;
	S : String;
begin
	if Exclude.FindId(FileName) <> nil then exit;
	Exclude.Add(New(PListItem, Create(FileName)));
	Assign(FO, WorkPath + WorkFile);
	Reset(FO, 1);
	Assign(FI, WorkPath + FileName);
	Reset(FI, 1);
	if FileSize(FO) > 0 then begin
		Seek( FO, FileSize(FO) );
		{ Put Header Data }
		WriteLn('Appending ' + FileName, ' (+', FileSize(FI), ')' );
		with R do begin
			Group := IGroup;
			Name := FileName;
			Size := FileSize(FI);
			Time := SRec.Time;
			Attr := SRec.Attr;
		end;
		{ Total := Total + Sizeof(R); }
		BlockWrite(FO, R, Sizeof(R));
	end else
		WriteLn('Creating ' + WorkFile + ' from ' + FileName, ' (', FileSize(FI), ')'  );
	Inc(Count);
	repeat
		BlockRead(FI, Buf, Sizeof(Buf), C);
		if C <> 0 then BlockWrite(FO, Buf, C);
		Total := Total + C;
	until C = 0;
	if Count = 1 then begin
		INSSize := Total;
		S := InstallHeader;
{		WriteLn(FileSize(FO)); }
		BlockWrite(FO, S, Length(S) + 1);
	end;
	Close(FI);
	Close(FO);
end;

procedure AppendAll(Search : String; IGroup : word);
begin
	FindFirst(WorkPath + Search, faAnyFile, SRec);
	while DosError = 0 do begin
		Append(SRec.Name, IGroup);
		FindNext(SRec);
	end;
end;

procedure InitFile;
var
	FO : File;
begin
	if ParamStr(1) <> '' then
		WorkPath := ParamStr(1) + '\';
	Assign(FO, WorkPath + WorkFile);
	ReWrite(FO, 1);
	Close(FO);
end;

procedure ExcludeFile(FileName : String);
begin
	Exclude.Add(New(PListItem, Create(FileName)));
end;

procedure ExcludeList;
begin
	Exclude.Create('ROOT');
	ExcludeFile('ADD2INS.EXE');
	ExcludeFile('ADDLIC.EXE');
	ExcludeFile('SPLICE.EXE');
	ExcludeFile('EXESIZE.EXE');
	ExcludeFile('INSTALL.TMP');
	ExcludeFile('TEMPLATE.THM');
	ExcludeFile('TEMPLATE.LNG');
	ExcludeFile('TEMPLATE.KMP');
end;
	
begin
	ExcludeList;
	InitFile;
	Append('INSTALL.EXE', igInstall );
	AppendAll('INSTALL.*', igInstall );
	AppendAll('PGME.*', igEternity );
	AppendAll('LICENSE.TXT', igEternity );
	AppendAll('ETERNITY.BIN', igEternity );
	AppendAll('ETERNITY.DAT', igEternity ); { Force rescanning of menus }
	AppendAll('ETERNITY' + EMenuExt, igEternity ); { Add Eternity Menu }
{	AppendAll('TEMPLATE.*', igSupplement ); }
	AppendAll('*' + ConfigExt, igConfig );
	AppendAll('*' + FontExt, igFonts);
	AppendAll('*' + KeymapExt, igKeymaps);
	AppendAll('*' + LanguageExt, igLanguages);
	AppendAll('*' + ThemeExt, igEternity);  
	AppendAll('*' + InfoExt, igSupplement);
	AppendAll('*' + ESaverExt, igSavers);
	AppendAll('*.EXE', igTools);
	AppendAll('*.TXT', igTextFiles );
	WriteLn(Count, ' files, ', INSSize, ' installer, ', Total - INSSize, ' data, ', Total, ' total bytes.');
	Exclude.Destroy;
end.
