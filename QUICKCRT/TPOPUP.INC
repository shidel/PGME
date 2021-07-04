{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TPopUp --- }

{$IFDEF INTERFACE}
const
	class_TPopUp = 'TPopUp';
	
type
	PPopUp = ^TPopUp;
	TPopUp = object(TWindow)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TPopUp.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TPopUp) then 
		ObjectClass := class_TPopUp
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TPopUp.Create;
begin
	inherited Create(AParent, AName);
	FAllowOutside := True;
	SetBorder(True);
	SetStyle(bxSingle);
	FCloseBtn.SetVisible(False);
	SetShadow(True);
	FAutoEndModal := True;
	FUseCaption := False;
	FVisible := False;
end;

procedure TPopUp.CheckLocal(var AEvent : TEvent); 
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmQuit : begin
				ClearEvent(AEvent); 
				FStopModal := True;
				PutCommand(cmQuit, nil);
			end;
		else
			{ Really Kludgy test to see if a child generated an event.
			  if so, repost and close }
			if Assigned(AEvent.InfoPtr) and (PControl(AEvent.InfoPtr)^.Parent = @Self) then begin
				PutEvent(AEvent);
				ClearEvent(AEvent);
				FStopModal := True;
			end;
		end;
	inherited CheckLocal(AEvent);
end;
{$ENDIF}
