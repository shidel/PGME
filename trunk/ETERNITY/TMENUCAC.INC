{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TMenuCache --- }

{$IFDEF INTERFACE}
const
	class_TMenuCache = 'TMenuCache';
	
type
	PMenuCache = ^TMenuCache;
	TMenuCache = object(TPersistent)
    public { protected }
    		FNoMenuStr,
    		FNoNameStr,
    		FNoTitleStr,
    		FNoDescStr : String;
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create;
		destructor Destroy; virtual;
		procedure ApplyLanguage(AConfig : PConfigFile); virtual;
		procedure AddDir(ADir : String); virtual;
		procedure AddPaths(APaths : String); virtual;
		procedure Update; virtual;

		{ Results based on current active menu }
		function FileName : String; virtual; { Menu Filename }
		function Title : String; virtual;	 { Menu Short Title }
		function Description : String; virtual;	{ Menu Long Title }
		function Writable : boolean; virtual; { Is Menu writable } 
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TMenuCache.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TMenuCache) then 
		ObjectClass := class_TMenuCache
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TMenuCache.Create;
begin
	inherited Create('MENUCACHE');
	FNoMenuStr := '';
	FNoNameStr := '';
	FNoTitleStr := '';
	FNoDescStr := '';
end;

destructor TMenuCache.Destroy;
begin
	inherited Destroy;
end;

procedure TMenuCache.ApplyLanguage;
begin
	FNoMenuStr := AConfig^.GetValue('MENU.EMPTY', '');
	FNoNameStr := AConfig^.GetValue('MENU.UNNAMED', 'no name');
	FNoTitleStr := AConfig^.GetValue('MENU.UNTITLED', 'Untitled Menu');
	FNoDescStr := AConfig^.GetValue('MENU.UNDESCRIBED', 'Untitled Menu');
end;

procedure TMenuCache.AddDir;
begin
end;

procedure TMenuCache.AddPaths;
begin
end;

procedure TMenuCache.Update;
begin
end;

function TMenuCache.FileName;
begin
	FileName := '';
end;

function TMenuCache.Title;
begin
	Title := FNoTitleStr;
end;

function TMenuCache.Description;
begin
	Description := FNoDescStr;	
end;

function TMenuCache.Writable;
begin
	Writable := False;
end;
{$ENDIF}
