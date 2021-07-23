{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TTDirDropList --- }

{$IFDEF INTERFACE}
const
	class_TDirDropList = 'TDirDropList';

type
	PDirDropList = ^TDirDropList;
	TDirDropList = object(TDropList)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
{--- TDirDropList ---}
function TDirDropList.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDirDropList) then 
		ObjectClass := class_TDirDropList
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDirDropList.Create(AParent : PControl; AName : String);
var
	I : Integer;
	S : String;
begin
	inherited Create(AParent, AName);
	SetSorted(True);
	for I := 1 to 26 do begin
		if IsValidDrive(I) then begin
			if IsRemovable(I) or IsCDROM(I) or IsRemote(I) then begin		
			end else begin	
				GetDir(I, S);
				AddItem(Dir(S));
			end;
		end;
	end;
	GetDir(0, S);
	FEdit.SetCaption(Dir(S));
end;
{$ENDIF}
