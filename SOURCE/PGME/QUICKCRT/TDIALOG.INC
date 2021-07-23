{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TDialog --- }

{$IFDEF INTERFACE}
const
	class_TDialog = 'TDialog';
	
type
	PDialog = ^TDialog;
	TDialog = object(TWindow)
	private 
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { Protected }
		procedure SetCurrentBounds(AValue : TBounds); virtual;
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDialog.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDialog) then 
		ObjectClass := class_TDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDialog.Create;
var
	T : integer;
begin
	T := TextAttr;
	TextAttr := $70;
	inherited Create(AParent, AName);
	TextAttr := T;
	FTitle.SetTextColor(4);
	FVisible := False;
	FAnchors := alNone;
	SetHelpText('0,1; %0 is a simple dialog box.');
end;

procedure TDialog.SetCurrentBounds(AValue : TBounds);
begin
	if Assigned(Parent) then begin
		AValue.Left := (PControl(Parent)^.Width div 2) - (AValue.Width div 2) + 1;
		AValue.Top := (PControl(Parent)^.Height div 2) - (AValue.Height div 2) + 1;
	end else begin
		AValue.Left := (Lo(ScreenMax) div 2) - (AValue.Width div 2) + 1;
		AValue.Top := (Hi(ScreenMax)  div 2) - (AValue.Height div 2) + 1;
	end; 
	inherited SetCurrentBounds(AValue);
end;

procedure TDialog.CheckLocal(var AEvent : TEvent); 
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmQuit : begin
				ClearEvent(AEvent);
				Hide;
				FResult := mrCancel;
				PutCommand(cmQuit, @Self);
			end;
			cmOK : begin
				ClearEvent(AEvent);
				Hide;
				FResult := mrOK;
			end;
			cmCancel : begin
				ClearEvent(AEvent);
				Hide;
				FResult := mrCancel;
			end;
			cmAbort : begin
				ClearEvent(AEvent);
				Hide;
				FResult := mrAbort;
			end;
			cmRetry : begin
				ClearEvent(AEvent);
				Hide;
				FResult := mrRetry;
			end;
		end;
	inherited CheckLocal(AEvent);
end;
{$ENDIF}
