{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgExecOpt --- }

{$IFDEF INTERFACE}
const
	class_TDlgExecOpt = 'TDlgExecOpt';

type 
	PDlgExecOpt = ^TDlgExecOpt;
	TDlgExecOpt = object (TDialog)
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
function TDlgExecOpt.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgExecOpt) then 
		ObjectClass := class_TDlgExecOpt
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgExecOpt.Create;
var
	B, T : TBounds;
begin
	inherited Create(AParent, AName);
	FStyle := bxSingle;	
	FAutoEndModal := True;
	Bounds(1,1,70,10, B);
	
	SetBounds(B);					

	Bounds(B.Width div 2 - 7, B.Height - 4, 14, 1, T);
	TextAttr := $2E;
	FOK := New(PButton, Create(@Self, 'BUTTON.OK'));
	with FOK^ do begin
		SetBounds(T);
		SetAnchors(abRight or abBottom);
		SetCaption('OK');
		SetCommand(cmOk);
	end;

end;

destructor TDlgExecOpt.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgExecOpt.BeforeShow;
begin
	inherited BeforeShow;
end;

procedure TDlgExecOpt.CheckLocal(var AEvent : TEvent);
var
	PDlg : PDialog;
begin
	inherited CheckLocal(AEvent);
	if AEvent.What = evCommand then 
		case AEvent.Command of
			cmAdd : begin
				PDlg := PDialog(PGME.GetDlgFindExec);
				if PDlg^.ShowModal = mrOK then begin
				end;
			end;
		end;
end;

procedure TDlgExecOpt.SetData(ADlg : PDialog);
begin
	SetTitle('');
	if Assigned(ADlg) then begin
		SetTitle(PDlgProg(ADlg)^.FEditTitle^.GetCaption);
	end;
end;

{$ENDIF}
