{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

const 
	InstallHeader : String = 'INSTALL_DATA';
	
	igInstall	= $FFFF; 	{ Installer Only Group, Never Installed }
	igEternity	= $0000; 	{ PGM Eternity Required Files }
	igSupplement= $0001; 	{ PGM Eternity Supplemental Files }
	igFonts		= $0002;	{ Add-on Fonts }
	igThemes	= $0003;	{ Add-on Themes }
	igLanguages	= $0004;	{ Add-on Languages }
	igKeymaps	= $0005;	{ Add-on Keymaps }
	igTools		= $0006; 	{ Add-on Utilities }
	igSavers	= $0007;	{ Add-on Screen Savers }
	igTextFiles = $0008;	{ Help Docs and other text files }
	igConfig    = $0009;	{ Not replaced if exists }
	
type
	TInsFileHeader = record
		Group 	: word;
		Name 	: String[12];
		Size 	: LongInt;
		Time 	: LongInt;
		Attr 	: byte;
	end;
