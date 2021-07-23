{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TFileDialog --- }

{$IFDEF INTERFACE}
const
	class_TFileDialog = 'TFileDialog';
	
type
	PFileDialog = ^TFileDialog;
	TFileDialog = object (TPathDialog)	
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }	
		FFilter : TDropList;
		FFile : TEdit;
		FMustExist : boolean;
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure AddFilter(AValue : String); virtual;
		procedure UpdateButtons; virtual;
		procedure UpdateDir; virtual;
		function GetFileName : String; virtual;
		procedure SetFileName (AValue : String); virtual;
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
	end;

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TFileDialog.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TFileDialog) then 
		ObjectClass := class_TFileDialog
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TFileDialog.Create;
var
	B : TBounds;
begin
	inherited Create(AParent, AName);
	SetTitle('File Dialog');
	Bounds(1,1,60,20,B);
	SetBounds(B);	
	FStyle := bxSingle;
	FMargin.Left := 0;
	FMargin.Top := 0;
	FMargin.Right:= 0;
	FMargin.Bottom := 0;
	FMustExist := True;
	FBOk.SetEnabled(False);

	Bounds(3,Height - 3, Width - 20, 1, B);

	TextAttr := $1B;
	FFile.Create(@Self, 'FILENAME');
	with FFile do begin
		SetFreeOnDestroy(False); 
		SetBounds(B);
		SetAnchors(abRight or abBottom or abLeft);
	end;

	TextAttr := $1A;
	FFilter.Create(@Self, 'FILTER');
	with FFilter do begin
		SetSorted(False);
		SetFreeOnDestroy(False); 
		Dec(B.Top, 2);
		B.Height := 8;
		SetBounds(B);
		SetAnchors(abRight or abBottom or abLeft);
		SetReadOnly(True);
	end;

	with FDirs do begin
		B := FCBounds;
		Dec(B.Height, 4);
		SetBounds(B);
		SetColumns(6);
		FColumns^[0].Width := 1;
		FColumns^[0].Name := StrPtr('FILE NAME');
		FColumns^[1].Width := 13;
		FColumns^[1].Name := StrPtr('SIZE');
		FColumns^[1].Align := AlignRight;
		FColumns^[2].Width := 0; {6} 
		FColumns^[2].Name := StrPtr('ATTR');
		FColumns^[2].Align := AlignCenter;
		FColumns^[3].Width := 0;  {8}
		FColumns^[3].Name := StrPtr('DATE');
		FColumns^[3].Align := AlignRight;
		FColumns^[4].Width := 0; {8}
		FColumns^[4].Name := StrPtr('TIME');
		FColumns^[4].Align := AlignRight;
		FColumns^[5].Width := 9;
		FColumns^[5].Name := StrPtr('WHEN');
		FColumns^[5].Align := AlignRight;
	end;
end;

destructor TFileDialog.Destroy;
begin
	inherited Destroy;
end;

procedure TFileDialog.AddFilter(AValue : String); 
begin
	FFilter.AddItem(AValue);
	if FFilter.GetCaption = '' then
		FFilter.SetFromList;
end;

procedure TFileDialog.CheckLocal(var AEvent : TEvent); 
var
	H : TEvent;
begin
	H := AEvent;
		
	inherited CheckLocal(AEvent); { Default Behavior if good, just need to add stuff } 

	if H.What = evCommand then
		case H.Command of
			cmEdit, cmDropList :  begin 
				if (H.InfoPtr = @FFilter) then begin
					ClearEvent(AEvent);
					UpdateDir;
				end else if (H.InfoPtr = @FFile) then begin
					ClearEvent(AEvent);
					if (Pos('*', FFile.GetCaption) > 0) or (Pos('?', FFile.GetCaption) > 0) then begin
						FFilter.SetCaption('Other Files (' + UCase(Trim(FFile.GetCaption)) + ')');
						FFile.SetCaption('');
						UpdateDir;
						UpdateButtons;
					end else
						UpdateButtons;
				end;
			end;
		cmListView, cmListViewDouble : if (H.InfoPtr = @FDirs) then begin
				if Assigned(FDirs.FCurrent) and FileExists(GetDir + FDirs.FCurrent^.GetNameID) then 
					FFile.SetCaption(FDirs.FCurrent^.GetNameID)
				else
					FFile.SetCaption('');
				ClearEvent(AEvent);
				UpdateButtons;
			end;
		end;
end;

procedure TFileDialog.UpdateButtons;
begin
	FBMkDir.SetEnabled(not Exists(FPath.GetCaption));	
	FBOk.SetEnabled(
		DirExists(FPath.FEdit.GetCaption) and (FFile.GetCaption <> '') and (
			(
				(not FMustExist) 
{				and (Not DirExists(Dir(FPath.FEdit.GetCaption) + FFile.GetCaption)) }
			) or (			
				FileExists(Dir(FPath.FEdit.GetCaption) + FFile.GetCaption)				
			)
		)
	);
end;

procedure TFileDialog.UpdateDir;
begin
	FDirs.FFilter := FFilter.GetCaption;
	StartDrawing;
	inherited UpdateDir;
	FinishDrawing;
end;

function TFileDialog.GetFileName : String;
begin
	if FileExists(Dir(FPath.FEdit.GetCaption) + FFile.GetCaption) or (not FMustExist) then
		GetFileName := Dir(FPath.FEdit.GetCaption) + FFile.GetCaption
	else
		GetFileName := '';
end;

procedure TFileDialog.SetFileName(AValue : String);
begin
	if (Pos('*', AValue) > 0) or (Pos('?', AValue) > 0) then begin
		FFilter.SetCaption('Other Files (' + UCase(Trim(AValue)) + ')');
		FFile.SetCaption('');
		UpdateDir;
		UpdateButtons;
	end else begin
		FFile.SetCaption(AValue);
		UpdateButtons;
	end;
end;

procedure TFileDialog.ApplyLanguage( AConfig : PConfigFile );
var
	P, M : PListItem;
	S : String;
begin
	inherited ApplyLanguage(AConfig);
	P := AConfig^.FindKey(GetPathID + '.FILTERS');
	if Assigned(P) then begin
		FFilter.FList.ClearItems;
		P := P^.First;
		while Assigned(P) do begin
			AddFilter(P^.GetNameId);
			P := P^.Next;
		end;
		FFilter.SetFromList;
	{$IFDEF TEMPLATES}
	end else begin
		AConfig^.SetValue(GetPathID + '.FILTERS', '');
		P := AConfig^.FindKey(GetPathID + '.FILTERS');
		if Assigned(P) and Assigned(FFilter.FList.First) then begin
			P^.Clear;
			M := FFilter.FList.FList.FItems.First;
			while Assigned(M) do begin
				S := M^.GetNameID;
				if MaxAvail < Sizeof(TListItem) + Length(S) + 1 then Halt(8);
				P^.Add(New(PListItem, Create(S)));
				M := M^.Next;
			end;
		end;
	{$ENDIF}
	end;
end;

{$ENDIF}
