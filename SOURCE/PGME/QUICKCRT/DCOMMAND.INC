{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

const
	DefaultMappedCommandsMax = 36;
	DefaultMappedCommands : array[0..DefaultMappedCommandsMax] of Str55 = (
		'None=$0000',
		'Break=$0063:M+$0104', 			                        { Ctrl+C }
		'Quit=$0071:M+$0008,$0071:M+$0208',	                    { Alt+Q }
		'Help=$3B00:M+$0000,$0068:M+$0208,$0068:M+$0008',	    { F1  & Alt+H }
		'Close=$0077:M+$0008,$0077:M+$0208',	                { Alt+W }
        'Tab=$0009:M+$0000', 			                        { TAB }
        'Untab=$0009:M+$0002,$0009:M+$0001',	                { Shift-TAB }
        'Backspace=$0008:M+$0000',		                        { BACKSPACE }
        'Delete=$0008:C+$0104,$5300:M+$0000',	                { Ctrl+BACKSPACE }
        'Stop=$007A:C+$0104',			                        { Ctrl+Z }
        'Enter=$000D:M+$0000',			                        { Enter }
        'Return=$000D:C+$0004',			                        { Ctrl+Enter }
        'Escape=$001B:M+$0000',			                        { Escape }
        'Insert=$5200:M+$0000',
		'Left=$4B00:M+$0000',
		'Right=$4D00:M+$0000',
		'Up=$4800:M+$0000',
		'Down=$5000:M+$0000',
		'Before=$4800::M+$0001,$4800:M+$0002',
		'After=$5000:M+$0001,$5000:M+$0002',
		'Previous=$4B00:M+$0001,$4B00:M+$0002',                 { Shift + Left }
		'Next=$4D00:M+$0001,$4D00:M+$0002',		                { Shift + Right }
		'LineStart=$4B00:M+$0008,$4D00:M+$0208,$4700:M+$0000',  { Alt+Left & Home }
		'LineEnd=$4D00:M+$0008,$4B00:M+$0208,$4F00:M+$0000',	{ Alt+Right & End }
		'PageUp=$4900:M+$0000',
		'PageDown=$5100:M+$0000',
		'GroupPrevious=$4900:M+$0001,$4900:M+$0002',            { Shift+PgUp }
		'GroupNext=$5100:M+$0001,$5100:M+$0002',
		'TextStart=$4900:M+$0008,$4900:M+$0208',	            { Alt+PgUp  }
		'TextEnd=$5100:M+$0008,$5100:M+$0208',

		'Cut=$0078:M+$0008,$0078:M+$0208',			            { Alt+X }
		'Copy=$0063:M+$0008,$0063:M+$0208',			            { Alt+C }
		'Paste=$0076:M+$0008,$0076:M+$0208',		            { Alt+V }
		'Find=$0066:M+$0008,$0066:M+$0208', 		            { Alt+F }

		'CreateItem=$004E:M+$0008,$004E:M+$0208',               { Alt+N }
		'DeleteItem=$0044:M+$0008,$0044:M+$0208',               { Alt+D }
		'EditItem=$0065:M+$0008,$0065:M+$0208'		            { Alt+E }

	);

const
	DefaultCommandNamesMax = 77;
	DefaultCommandNames : array[0..DefaultCommandNamesMax] of Str25 = (
		{ System/Environment Commands }
		'None=$0000',
		'ClearedEvent=$0001',
		'MakeSysReq=$0001',
		'BreakSysReq=$0003',
		'PrintScreen=$0004',
		'Break=$0005',
		'Quit=$0006',
		'Help=$0007',
		'Close=$0008',
		'Timer=$0009',
		'Clock=$000A',
		'Speaker=$000B',
		'Search=$000C',
		{ Keyboard Editing Commands }
		'Tab=$0100',
		'Untab=$0101',
		'Backspace=$0102',
		'Delete=$0103',
		'Stop=$0104',
		'Enter=$0105',
		'Return=$0106',
		'Escape=$0107',
		'Insert=$0108',
		'Clear=$0109',
		'Add=$010A',
		'Modify=$010B',
		'Remove=$010C',
		{ Keyboard Navigation Commands }
		'Left=$0110',
		'Right=$0111',
		'Up=$0112',
		'Down=$0113',
		'Previous=$0114',
		'Next=$0115',
		'LineStart=$0116',
		'LineEnd=$0117',
		'PageUp=$0118',
		'PageDown=$0119',
		'GroupPrevious=$011A',
		'GroupNext=$011B',
		'TextStart=$011C',
		'TextEnd=$011D',
		'Before=$011E',
		'After=$011F',

		'Cut=$0200',
		'Copy=$0201',
		'Paste=$0202',
		'Find=$0203',
		'FindAgain=$0204',
		'Replace=$0205',
		'ReplaceAll=$0206',

		{ Special Commands }
		'CharMap=$0300',
		'MakeDir=$0301',
		'NewFile=$0302',
		'OpenFile=$0303',
		'SaveFile=$0304',
		'SaveFileAs=$0305',
		'CloseFile=$0306',
		'InfoOnFile=$0307',
		'ExecuteFile=$0308',
		'EditFile=$0309',

		'About=$0310',

		{ Modal Commands }
		'OK=$0400',
		'Cancel=$0401',
		'Abort=$0402',
		'Retry=$0403',

		{ TUI Commands }
		'ScrollBar=$0500',
		'Toggle=$0501',
		'Checkbox=$0502',
		'RadioButton=$0503',
		'ListView=$0504',
		'ListViewDouble=$0505',
		'ListViewChange=$0506',
		'DoDropList=$0507',
		'DropList=$0508',
		'Edit=$0509',

		{ General TUI Item Commands }
		'CreateItem=$0600',
		'DeleteItem=$0601',
		'EditItem=$0602',

		{ User Commands }
		'User=$1000'
	);
