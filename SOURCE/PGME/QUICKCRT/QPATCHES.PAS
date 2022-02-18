{ Copyright 2022 Jerome Shidel }


(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

unit QPatches; { QuickCRT Patches, version 8.40 }
{$I QCrt.def}
{$F+,O-}

interface

implementation

{	uses QCrt, QDos, QStrings; }

begin
 {   if (UCase(GetEnv('LANG')) <> 'EN') and (UCase(GetEnv('LANG')) <> '') then
    	RestoreInt15Patch; }
end.
