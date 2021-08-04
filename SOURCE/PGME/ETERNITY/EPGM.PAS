{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

unit EPGM;

interface

uses QCrt, QErrors, QStrings, QDos, QFiles, QClass, QObjects, QControls, QFmtStr, PGM_Call,
	QFileDlg, QTimer;

{$DEFINE INTERFACE}

{$I TPGME.inc}

{$UNDEF INTERFACE}

implementation

uses EConst, ECache, EDialogs;

{$DEFINE IMPLEMENTATION}

{$I TPGME.inc}

{$UNDEF IMPLEMENTATION}

end.
