{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ETOOLS.DEF}
program Eternity_Font_Designer; { X Series }

uses QCrt, QClass, QErrors, QStrings, QDos, QFiles, QObjects, QControls;

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'Empty Tool Application Template';
	AppID = 'ETOOL';
	class_TETool : TObjectClass = 'TEternityTool';

type
	TETool = object (TApplication)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		procedure BeforeShow; virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
	end;

{ --- Global Variables --- }
var
	ETool : TETool;

{ --- TETool --- }
function TETool.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TETool) then
		ObjectClass := class_TETool
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TETool.Create;
begin
	inherited Create(nil, AppID);
end;

destructor TETool.Destroy;
begin
	inherited Destroy;
end;

procedure TETool.BeforeShow;
begin
	inherited BeforeShow;
	GotoXY(1, WhereY);
	TextAttr := $07;
	ClrEOL;
end;

begin
	Write('Starting ' + AppTitle + '...');
	ETool.Create;
	ETool.Execute;
	ETool.Destroy;
	TextAttr := $08;
	GotoXY(1,WhereY);
	Write('Goodbye.');
	ClrEol;
	TextAttr := $07;
	WriteLn;
end.
