{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgLaunch --- }

{$IFDEF INTERFACE}
const
	class_TDlgLaunch = 'TDlgLaunch';

type 
	PDlgLaunch = ^TDlgLaunch;
	TDlgLaunch = object (TDlgLaunchFwd)
    public { protected }	
    		FOK : PButton;
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure SetData(ADlg : PDialog); virtual;
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgLaunch.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgLaunch) then 
		ObjectClass := class_TDlgLaunch
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgLaunch.Create;
const
	ColorLabel = $70;
	ColorEdit = $1E;
	ColorFile = $71;
	ColorOK = $2E;
	ColorToggle = $0F;
	ColorAccept = $74;
	ColorLaunch = $3E;
var
	B, T : TBounds;
begin
	{ TextAttr := $3E; }
	inherited Create(AParent, AName);
	FAutoEndModal := True;
	Bounds(1,1,55,15, B);
	{ SetBackGround($3); }
	if AParent^.Height > 18 then begin
		Inc(B.Height, 2);
	end;
	SetBounds(B);					

	TextAttr := ColorOK;
	FOK := New(PButton, Create(@Self, 'BUTTON.OK'));
	with PButton(FOK)^ do begin
		Bounds(B.Left + B.Width - 20, B.Top + B.Height - 5, 14, 1, B);
		SetBounds(B);
		SetAnchors(abRight or abTop);
		SetCaption('Done');
		SetCommand(cmOK);
	end;

{	Bounds(3,2,47,1, B);		
	TextAttr := ColorLabel;
	with PLabel(New(PLabel, Create(@Self, 'TITLE.LABEL')))^ do begin
		SetBounds(B);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		SetCaption('Program Title:');
	end;
}
end;

destructor TDlgLaunch.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgLaunch.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
{	if AEvent.What = evCommand then 
		case AEvent.Command of
		end;}
end;

procedure TDlgLaunch.BeforeShow;
begin
	inherited BeforeShow;
end;

procedure TDlgLaunch.SetData(ADlg : PDialog);
begin
	SetTitle('');
	if Assigned(ADlg) then begin
		SetTitle(PDlgProg(ADlg)^.FEditTitle^.GetCaption);
	end;
end;

{$ENDIF}
