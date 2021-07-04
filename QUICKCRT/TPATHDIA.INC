{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{--- TPathDialog ---}

{$IFDEF INTERFACE}
const
	class_TPathDialog = 'TPathDialog';
	
type
	PPathDialog = ^TPathDialog;
	TPathDialog = object (TDialog)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FBOk, 
		FBCancel, 
		FBMkDir : TButton;
		FPath : TDirDropList;
		FDirs : TDirList;
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure SetDir(AValue : String); virtual;
		function GetDir : String; virtual;
		procedure UpdateButtons; virtual;
		procedure UpdateDir; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TPathDialog.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TPathDialog) then 
		ObjectClass := class_TPathDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TPathDialog.Create;
var
	B : TBounds;
begin
	inherited Create(AParent, AName);
	SetTitle('Select Directory');
	Bounds(1, 1, 42, Hi(ScreenMax) - 1,B);
	SetBounds(B);	
	
	FStyle := bxSingle;
	FMargin.Left := 0;
	FMargin.Top := 0;
	FMargin.Right:= 0;
	FMargin.Bottom := 0;
	
	Bounds(Width - 15,Height - 3, 12, 1, B);
	SetAnchors(abTop or abBottom);
	
	TextAttr := $2E;
	FBCancel.Create(@Self, 'BUTTON.Cancel');
	with FBCancel do begin
		SetFreeOnDestroy(False); 
		SetBounds(B);
		SetAnchors(abRight or abBottom);
		SetCaption('Cancel');
		SetCommand(cmCancel);
	end;
	TextAttr := $2E;
	FBOK.Create(@Self, 'BUTTON.OK');
	with FBOk do begin
		SetFreeOnDestroy(False); 
		Dec(B.Top, 2);
		SetBounds(B);
		SetAnchors(abRight or abBottom);
		SetCaption('Ok');
		SetCommand(cmOk);
	end;
	TextAttr := $4E;
	FBMkDir.Create(@Self, 'BUTTON.MAKEDIR');
	with FBMkDir do begin
		SetFreeOnDestroy(False); 
		B.Top := 3;
		SetBounds(B);
		SetAnchors(abRight or abTop);
		SetCaption('Make Dir');
		SetEnabled(False);
		SetCommand(cmMakeDir);
	end;
	
	Bounds(3,3, B.Left - 5, 10, B);
	TextAttr := $1E;
	FPath.Create(@Self, 'PATH');
	with FPath do begin
		SetFreeOnDestroy(False); 
		SetBounds(B);
		SetAnchors(abRight or abTop or ableft);
	end;
	
	Bounds(B.Left, B.Top + 2, B.Width, Height - B.Top - 4, B);
	TextAttr := $1E;
	FDirs.Create(@Self, 'DIRECTORY');
	with FDirs do begin
		SetFreeOnDestroy(False); 
		SetBounds(B);
		SetAnchors(abAll);
		FFilter := '';
	end;

	SetHelpText('This is a standard directory selection dialog box.');
	AddHelpText('');
	AddHelpText('You may use the dropdown path list to select one of the systems current');
	AddHelpText('active directories. Or, you may type in one for direct access. Also,' );
	AddHelpText('recently entered paths are stored into the list for quick selection.');
	AddHelpText('');
	AddHelpText('When a specified directory does not exist, the Make Dir button will' );
	AddHelpText('become available. ' );
end;

destructor TPathDialog.Destroy;
begin
	FDirs.Destroy;
	FPath.Destroy;
	FBMkDir.Destroy;
	FBOk.Destroy;
	FBCancel.Destroy;
	inherited Destroy;
end;

procedure TPathDialog.SetDir(AValue : String);
var
	P : PListItem;
begin
	AValue := Dir(trim(AValue));
	StartDrawing;
	FDirs.BeginUpdate;
	FPath.FEdit.SetCaption(AValue);
	FPath.AddItem(AValue);
	FPath.Update;
	FDirs.SetDir(AValue);
	FDirs.EndUpdate;
	UpdateButtons;
	FinishDrawing;
end;

procedure TPathDialog.UpdateDir;
begin
	FDirs.SetDir(FPath.FEdit.GetCaption);
end;

function TPathDialog.GetDir : String; 
begin
	GetDir := FDirs.GetDir;
end;

procedure TPathDialog.CheckLocal(var AEvent : TEvent); 
var
	S : String;
	I : integer;
begin
	if AEvent.What = evCommand then
		case AEvent.Command of
			cmMakeDir : begin
				ClearEvent(AEvent);
				MakeDir(FPath.FEdit.GetCaption);
				PutCommand(cmEdit, @FPath);
			end;
			cmEdit, cmDropList : if AEvent.InfoPtr = @FPath then begin
				ClearEvent(AEvent);
				StartDrawing;
				S := Dir(FPath.FEdit.GetCaption);
				SetDir(S);
				UpdateButtons;
				FinishDrawing;
			end;
			cmListView, cmListViewDouble : if (AEvent.InfoPtr = @FDirs) then begin
				S := GetDir;
				if Assigned(FDirs.FCurrent)  then begin
					if FDirs.FCurrent^.GetNameID = '..' then begin
						I := LastPos('\', Copy(S, 1, Length(S) - 1));
						if I > 3 then Dec(I);
						S := Copy(S, 1, I);
					end else
						S := S + FDirs.FCurrent^.GetNameID;
				end;
				if DirExists(S) then begin
					if FPath.FEdit.GetCaption <> Dir(S) then begin					
						FPath.FEdit.SetCaption(Dir(S));
						FPath.Update;
					end;
					if  (AEvent.Command = cmListViewDouble) then begin
						StartDrawing;
						FDirs.SetDir(S);
						FinishDrawing;
					end;
					UpdateButtons;
				end else begin
					FPath.FEdit.SetCaption(GetDir);
					FPath.Update;
				end;
				ClearEvent(AEvent);
			end;
		end;
	inherited CheckLocal(AEvent);
end;

procedure TPathDialog.UpdateButtons;
begin
	FBMkDir.SetEnabled(not Exists(FPath.GetCaption));
	FBOk.SetEnabled(FDirs.FValid);	
end;

procedure TPathDialog.BeforeShow;
begin
	inherited BeforeShow;
	UpdateDir;
end;
{$ENDIF}
