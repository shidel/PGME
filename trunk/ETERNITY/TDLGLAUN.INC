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
			FPath : PDirDropList;
			FDirs : PDirList;
    		FOK, FAdd : PButton;	
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
	FStyle := bxSingle;
{	FMargin.Left := 0;
	FMargin.Top := 0;
	FMargin.Right:= 0;
	FMargin.Bottom := 0; }
	
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
		Bounds(B.Left + B.Width - 20, B.Top + B.Height - 5, 14, 1, T);
		SetBounds(T);
		SetAnchors(abRight or abTop);
		SetCaption('Done');
		SetCommand(cmOK);
	end;

	Bounds(3,3, B.Width - 10, 10, T);
	TextAttr := $1E;
	FPath := New(PDirDropList, Create(@Self, 'PATH'));
	with FPath^ do begin
		SetBounds(B);
		SetAnchors(abRight or abTop or ableft);
	end;
	
	Bounds(T.Left, T.Top + 2, T.Width, Height - T.Top - 4, T);
	TextAttr := $1E;
	FDirs := New(PDirList, Create(@Self, 'DIRECTORY'));
	with FDirs^ do begin
		SetBounds(T);
		SetAnchors(abAll);
		FFilter := '';
	end;

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
