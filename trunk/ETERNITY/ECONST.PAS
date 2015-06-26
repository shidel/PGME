{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit EConst;

interface

	uses  QCrt;

{$I LICENSE.INC}
{$I ../VERSION.INC}

const
	cmExecute 	= cmExecuteFile;
	cmNextMenu  = cmAfter;
	cmPrevMenu  = cmBefore;
	
	cmProgAdd 	= $1100;
	cmProgEdit	= cmProgAdd + 1;
	cmProgDel	= cmProgEdit + 1;
	
	cmMenuAdd 	= cmProgDel + 1;
	cmMenuEdit	= cmMenuAdd + 1;
	cmMenuDel	= cmMenuEdit + 1;
	
	cmPGMOpts   = cmMenuDel + 1;
	cmPGMCfg	= cmPGMOpts + 1;
	
	cmBrowseTheme = cmPGMCfg + 1;
	cmBrowseFont = cmBrowseTheme + 1;
	cmBrowseESaver = cmBrowseFont + 1;
	cmBrowseLang = cmBrowseESaver + 1;

	cmBrowseExec = cmBrowseLang + 1;

	cmProgMove = cmBrowseExec + 1;
		
	ftMenu	= 'MENU';

implementation

end.
