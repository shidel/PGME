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
			FProc	: PListView;
    		FOK, FAdd, FClear, FModify, FRemove : PButton;	
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
		procedure UpdateButtons; virtual;
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
var
	B, T : TBounds;
begin
	inherited Create(AParent, AName);
	SetHelpText('This is "Execution Process" editor.');
	AddHelpText('');
	AddHelpText('It provides the basic functionality to add, edit and remove items ');
	AddHelpText('from a programs launch process.');
	FStyle := bxSingle;	
	FAutoEndModal := True;
	Bounds(1,1,64,15, B);
	
	if AParent^.Height > 18 then begin
		Inc(B.Height, 2);
	end;
	SetBounds(B);					

	TextAttr := $70;
	with PLabel(New(PLabel, Create(@Self, 'LABEL')))^ do begin
		Bounds(3, 2, 40, 1, T);
		SetBounds(T);
		SetAnchors(abRight or abTop);
		SetCaption('Execution Process:');
		FHAlign := AlignLeft;
	end;
	
	TextAttr := $17;
	FProc := New(PListView, Create(@Self, 'PROCLIST'));
	with FProc^ do begin
		SetHelpText('The "Process List" shows the current launch process.');
		Inc(T.Top, 2);
		T.Height := B.Height - T.Top - 3;
		SetBounds(T);
	end;

	Bounds(B.Left + B.Width - 20, 2, 14, 1, T);
	Inc(T.Top, 2);
	TextAttr := $2E;
	FAdd := New(PButton, Create(@Self, 'BUTTON.ADD'));
	with FAdd^ do begin
		SetHelpText('5;Use the "%0" button to add another executable to the process.');
		SetBounds(T);
		SetAnchors(abRight or abTop);
		SetCaption('Add');
		SetCommand(cmAdd);
	end;
	Inc(T.Top, 2);
	TextAttr := $3E;
	FModify := New(PButton, Create(@Self, 'BUTTON.OPTIONS'));
	with FModify^ do begin
		SetHelpText('5;Use the "%0" button to modify an executables command line parameters.');
		SetBounds(T);
		SetAnchors(abRight or abTop);
		SetCaption('Options');
		SetCommand(cmModify);
	end;
	Inc(T.Top, 2);
	TextAttr := $4E;
	FRemove := New(PButton, Create(@Self, 'BUTTON.REMOVE'));
	with FRemove^ do begin
		SetHelpText('5;Use the "%0" button to remove a line from the process.');
		SetBounds(T);
		SetAnchors(abRight or abTop);
		SetCaption('Remove');
		SetCommand(cmRemove);
	end;
	Inc(T.Top, 2);
	TextAttr := $4E;
	FClear := New(PButton, Create(@Self, 'BUTTON.CLEAR'));
	with FClear^ do begin
		SetHelpText('5;Use the "%0" button to clear the entire process and start over.');
		SetBounds(T);
		SetAnchors(abRight or abTop);
		SetCaption('Clear');
		SetCommand(cmClear);
	end;

	T.Top := B.Height - 4;
	TextAttr := $2E;
	FOK := New(PButton, Create(@Self, 'BUTTON.OK'));
	with FOK^ do begin
		SetHelpText('5;Use the "%0" button accept all changes.');
		SetBounds(T);
		SetAnchors(abRight or abBottom);
		SetCaption('Done');
		SetCommand(cmOk);
	end;

end;

destructor TDlgLaunch.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgLaunch.BeforeShow;
begin
	inherited BeforeShow;
end;

procedure TDlgLaunch.UpdateButtons;
begin
	FRemove^.SetEnabled(Assigned(FProc^.FCurrent));
	FModify^.SetEnabled(Assigned(FProc^.FCurrent));
	FClear^.SetEnabled(FProc^.FItems.Count > 0);
end;

procedure TDlgLaunch.CheckLocal(var AEvent : TEvent);
var
	PDlg, POpt : PDialog;
	R, R2 : integer;
begin
	inherited CheckLocal(AEvent);
	if AEvent.What = evCommand then 
		case AEvent.Command of
			cmAdd : begin
				POpt := nil;
				PDlg := PDialog(PGME.GetDlgFindExec);
				R := PDlg^.ShowModal;
				R2 := mrCancel;
				if R = mrOK then begin
					POpt := PDialog(PGME.GetDlgOptExec);
					PDlgExecOpt(POpt)^.SetData(PDlg);
					R2 := POpt^.ShowModal;
				end;
				
			end;
		end;
end;

procedure TDlgLaunch.SetData(ADlg : PDialog);
begin
	SetTitle('');
	if Assigned(ADlg) then begin
		SetTitle(PDlgProg(ADlg)^.FEditTitle^.GetCaption);
	end;
	UpdateButtons;
end;

{$ENDIF}
