{ Copyright 2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ETOOLS.DEF}
program Eternity_Resource_File_Toolkit; { X Series }

uses QObjects, QStrings, QErrors, QDos, QFiles, QResrc,
    QClass;

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'Eternity Resource File Utility';
	AppID = 'QRESFILE';

const
  	class_TResource : TObjectClass = 'TFile';

type
    PResource = ^TResource;
    TResource = object (TObject)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
    public
      constructor Create;
      destructor  Destroy; virtual;
    end;

function TResource.ObjectClass(AName : String) : String;
begin
    if (AName = '') or (AName = class_TResource) then
        ObjectClass := class_TResource
    else
        ObjectClass := inherited ObjectClass(AName);
end;

constructor TResource.Create;
begin
    inherited Create('');
end;

destructor  TResource.Destroy;
begin
    inherited Destroy;
end;

procedure ShowHelp;
begin
    WriteLn('usage: ', GetExeBase, ' [options]');

end;

begin
	if ParamCount = 0 then begin
	    ShowHelp;
	    Halt(0);
	end;
end.
