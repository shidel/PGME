{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ..\QUICKCRT\TEMPLATE.DEF}
{$I ETERNITY.DEF}
unit ECache;

interface

uses QCrt, QErrors, QStrings, QDos, QFiles, QClass, QObjects, QControls, QFmtStr,
	EConst;

{$DEFINE INTERFACE}

{$I TCacheItems.inc}
{$I TMenuCache.inc}

{$UNDEF INTERFACE}

implementation

{$DEFINE IMPLEMENTATION}

{$I TCacheItems.inc}
{$I TMenuCache.inc}

{$UNDEF IMPLEMENTATION}

end.
