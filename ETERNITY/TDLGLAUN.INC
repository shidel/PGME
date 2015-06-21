{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgLaunch --- }

{$IFDEF INTERFACE}
const
	class_TDlgLaunch = 'TDlgLaunch';
	class_TLaunchItem = 'TLaunchItem';

type
	PLaunchItem = ^TLaunchItem;
	TLaunchItem = object(TListItem)
	public
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		FPath : String;
		FExact, FLock, FNOpt : boolean;
		FOptions : String;
		constructor Create(AName : String);
	end;

type 
	PDlgLaunch = ^TDlgLaunch;
	TDlgLaunch = object (TDlgLaunchFwd)
    public { protected }
    		FStartDir : String;
			FProc	: PListView;
			FDlg : PDialog;
    		FOK, FAdd, FClear, FModify, FDos, FRemove : PButton;	
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
		procedure UpdateButtons; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure SetData(ADlg : PDialog); virtual;
		procedure ClearData; virtual;
		procedure AddData(ADlg : PDialog); virtual;
		procedure AddDefaults; virtual;
		procedure AddCurrent; virtual;
		procedure RemoveData; virtual;
		function AddSingle(AFilename : String; DosCmd : boolean) : PLaunchItem; virtual;
	end;   
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TLaunchItem.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TLaunchItem) then 
		ObjectClass := class_TLaunchItem
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TLaunchItem.Create;
begin
	inherited Create(AName);
	FLock := False;
	FNOpt := False;
	FExact := False;
	FPath := '';
	FOptions := '';
end;

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
	FDlg := nil;
	SetHelpText('This is "Execution Process" editor.');
	AddHelpText('');
	AddHelpText('It provides the basic functionality to add, edit and remove items ');
	AddHelpText('from a programs launch process.');
	FStyle := bxSingle;	
	FAutoEndModal := True;
	Bounds(1,1,65,15, B);
	
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
	
	TextAttr := $1E;
	FProc := New(PListView, Create(@Self, 'PROCLIST'));
	with FProc^ do begin
		SetHelpText('The "Process List" shows the current launch process.');
		Inc(T.Top, 2);
		T.Height := B.Height - T.Top - 3;
		SetBounds(T);
		SetColumns(2);
		FColumns^[0].Width := 13;
		FColumns^[0].Name := StrPtr('COMMAND');
		FColumns^[1].Width := T.Width - FColumns^[0].Width;
		FColumns^[1].Name := StrPtr('OPTIONS');
		FColumns^[1].Colors.Normal := (FColumns^[1].Colors.Normal and $F0) or $07;
	end;

	Bounds(B.Left + B.Width - 21, 2, 15, 1, T);
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
	TextAttr := $2E;
	FDos := New(PButton, Create(@Self, 'BUTTON.COMMAND'));
	with FDos^ do begin
		SetHelpText('5;Use the "%0" button to manually add a dos command.');
		SetBounds(T);
		SetAnchors(abRight or abTop);
		SetCaption('DOS Command');
		SetCommand(cmEdit);
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
	if Assigned(FProc^.FCurrent) and (PLaunchItem(FProc^.FCurrent)^.FLock = false) then begin
		FRemove^.SetEnabled(True);
		if (PLaunchItem(FProc^.FCurrent)^.FNOpt = false) then
			FModify^.SetEnabled(True)
		else
			FModify^.SetEnabled(False);
	end else begin
		FRemove^.SetEnabled(False);
		FModify^.SetEnabled(False);
	end;
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
			cmListView : if AEvent.InfoPtr = FProc then begin
				ClearEvent(AEvent);
				UpdateButtons;
			end;
			cmEdit : if AEvent.InfoPtr = FDos then begin
				PDlg := PDialog(PGME.GetDlgDosCOmmand);
				PDlgDosCommand(PDlg)^.FCmdLine^.SetCaption('');
				R := PDlg^.ShowModal;
				if R = mrOK then 
					AddData(PDlg);		
			end;
			cmAdd : begin
				ClearEvent(AEvent);
{				POpt := nil; }
				PDlg := PDialog(PGME.GetDlgFindExec);
				if Assigned(FProc^.FItems.Last) then
					POpenDialog(PDlg)^.SetDir(PLaunchItem(FProc^.FItems.Last)^.FPath);
				R := PDlg^.ShowModal;
				if R = mrOK then AddData(PDlg);
{				R2 := mrCancel;
				if R = mrOK then begin
					POpt := PDialog(PGME.GetDlgOptExec);
					PDlgExecOpt(POpt)^.SetData(PDlg);
					R2 := POpt^.ShowModal;
				end; }
			end;
			cmListViewDouble, cmModify : if FModify^.GetEnabled and ((AEvent.Command = cmModify) or 
			(AEvent.InfoPtr = FProc)) then begin
				ClearEvent(AEvent);
				if Assigned(FProc^.FCurrent) then begin
					POpt := PDialog(PGME.GetDlgOptExec);
					PDlgExecOpt(POpt)^.SetData(@Self);
					R := POpt^.ShowModal;
					if R = mrOK then begin
						PLaunchItem(FProc^.FCurrent)^.Clear;
						PLaunchItem(FProc^.FCurrent)^.FOptions := 
							PDlgExecOpt(POpt)^.FCmdLine^.GetCaption;
						FProc^.FCurrent^.Add(New(PListItem, Create(
							PLaunchItem(FProc^.FCurrent)^.FOptions)));
					end;
				end;
			end; 
			cmClear : ClearData;
			cmRemove : RemoveData;			
				
		end;
end;

procedure TDlgLaunch.AddDefaults;
var
	P : PLaunchItem;
begin
{	P := New(PLaunchItem, Create('@ECHO OFF'));
	P^.FLock := True;
	with FProc^ do begin
		FItems.Add(P);
	end;	 }
	
	P := AddSingle(Copy(FStartDir, 1, 2), True);
	P^.FLock := True;
	P := AddSingle('CD ' + Copy(FStartDir, 3, Length(FStartDir)), True);
	P^.FLock := True;
end;

procedure TDlgLaunch.AddCurrent;
var
	P : PListItem;
begin
	with PDlgProg(FDlg)^ do begin
		if FCommands.Count = 0 then 
			AddSingle(FFile, False)
		else begin
			P := FCommands.First;
			while Assigned(P) do begin
				AddSingle(P^.GetNameID, False);
				P := P^.Next;
			end;
		end;
	end; 
end;

procedure TDlgLaunch.SetData(ADlg : PDialog);
begin
	FDlg := ADlg;
	SetTitle('');
	if Assigned(ADlg) then begin
		SetTitle(PDlgProg(ADlg)^.FEditTitle^.GetCaption);
	end;
	FStartDir := '';
	Data := ADlg^.Data;
	FProc^.ClearItems;
	if Assigned(Data) then begin
		with PCacheItem(PListItem(Data)^.Data)^ do
			FStartDir := PCacheDir(Parent^.Parent)^.GetNameID;
	end	else begin
		with PGME do 
			if Assigned(FMenus) and Assigned(FMenus^.FCurrent) then 
				with PCacheMenu(FMenus^.FCurrent^.Data)^ do 
					FStartDir := PCacheDir(Parent)^.GetNameID; 
	end;
	AddDefaults;
	if Assigned(Data) then AddCurrent;
	UpdateButtons;
end;

function TDlgLaunch.AddSingle(AFilename : String; DosCmd : boolean) : PLaunchItem;
var
	FName, FPath, FOpts, LPath : String;
	P : PLaunchItem;
begin
	if Assigned(FProc^.FItems.Last) then 
		LPath := PLaunchItem(FProc^.FItems.Last)^.FPath
	else
		LPath := '';
		
	FName := UCase(Trim(PullStr(' ', AFilename)));
	if ((Length(FName) = 2) and (FName[2] = ':')) or (FName = 'CD') then 
		DosCmd := True;
		
	if DosCmd then begin
		FPath := LPath;
		FOpts := AFilename;
	end else begin
		FPath := PathOf(FName);
		FName := Copy(FName, Length(FPath) + 1, Length(FName));
		FOpts := Trim(AFilename);
	end;
	
	if FPath = '' then begin
		if Assigned(FProc^.FItems.Last) then 
			FPath := PLaunchItem(FProc^.FItems.Last)^.FPath
		else
			FPath := FStartDir;
	end;
	
	FPath := Dir(FPath);

	if (Length(FName) = 2) and (FName[2] = ':') then begin
		LPath := FName;
		FPath := FName;
	end else begin
		if LPath[1] <> FPath[1] then begin
			P := New(PLaunchItem, Create(Copy(FPath, 1, 2)));
			P^.FLock := False;
			P^.FNOpt := True; 
			P^.FPath := Copy(FPath, 1, 2);
			FProc^.FItems.Add(P);
		end;
	end;
	
	if FName = 'CD' then begin
		FOpts := Trim(UCase(FOpts));
		FPath := Dir(Copy(FPath, 1, 2) + FOpts);
		FOpts := Copy(FPath, 3, Length(FPath));
		LPath := FPath;
	end else begin
		if LPath <> FPath then begin
			{P := New(PLaunchItem, Create('CD'));
			P^.FLock := False;
			P^.FNOpt := True; 
			P^.FPath := FPath;
			P^.FOptions := Copy(FPath, 3, Length(FPath));
			P^.Add(New(PListItem, Create(Copy(FPath, 3, Length(FPath)))));
			FProc^.FItems.Add(P); }
			Self.AddSingle('CD ' + Copy(FPath, 3, Length(FPath)), False);
		end;
	end;
	P := New(PLaunchItem, Create(FName));
	P^.FLock := False;
	P^.FOptions := FOpts;
	P^.FPath := FPath;	
	P^.FExact := DosCmd;
	if (Length(FName) = 2) and (FName[2] = ':') then begin
		P^.FNOpt := True;
	end;
	if FName = 'CD' then
		P^.FNOpt := True;
	
	if FOpts <> '' then
		P^.Add(New(PListItem, Create(FOpts)));
	with FProc^ do begin
		FItems.Add(P);
		AdjustSize;
		Update;
		Bottom;
	end;
	AddSingle := P;
end;

procedure TDlgLaunch.AddData;
begin
	StartDrawing;
	if ADlg^.ClassOf(class_TOpenDialog) then
		AddSingle(POpenDialog(ADlg)^.GetFileName, False)
	else
	if ADlg^.ClassOf(class_TDlgDosCommand) then
		AddSingle(PDlgDosCommand(ADlg)^.FCmdLine^.GetCaption, True);
	UpdateButtons;
	FinishDrawing;
end;

procedure TDlgLaunch.RemoveData;
begin
	if Not Assigned(FProc^.FCurrent) then exit;
	if PLaunchItem(FProc^.FCurrent)^.FLock then exit;
	StartDrawing;
	FProc^.DeleteItem(FProc^.FCurrent);
	UpdateButtons;
	FinishDrawing;
end;

procedure TDlgLaunch.ClearData;
begin
	StartDrawing;
	FProc^.ClearItems;
	AddDefaults;
	UpdateButtons;
	FinishDrawing;
end;

{$ENDIF}
