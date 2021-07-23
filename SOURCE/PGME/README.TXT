### Program Manager Eternity Download Readme 

** PGME-YEAR-MONTH-DAY.zip File **

	This is the pre-compiled release of the PGME Installer. Simply download,
	extract and execute the INSTALL.EXE file. It will perform the needed 
	steps to install or upgrade to the latest version of PGME. 
		
		It will create an installation directory or detect the previous one.
		Also, it creates a launcher batch file called PGM.BAT and
		will place it in a directory listed your path environment 
		variable. You should really use the PGM.BAT launcher to start
		PGME. 
		
		Please note that during installation the installer will overwrite
		all pre-existing PGME files, with one exception. Only when 
		a .CFG file does not exist, it is written to disk. Also, any files
		that may exist in the PGME target directory, that are not part
		of PGME are left untouched. This includes any additional themes,
		languages, keymaps, fonts, menus and etc... 
		
		Remember, the ETERNITY.MNU file comes with PGME and is overwritten
		on every install. Do not modify that menu file or you will loss any 
		changes when you upgrade PGME. If you don't want it, delete it from 
		the command line. It is marked as not writable to help prevent losing
		menu entries. 
		
** PGME-YEAR-MONTH-DAY.src.zip **

		This is the PGME Source code for the specified version.
		Please note if you wish to build PGME from source, you will need
		the http://quickcrt.sourceforge.net units. Oh, and Turbo Pascal 7.0.
		
** DEFAULTS-YEAR-MONTH-DAY.zip **

		You don't really need this file. It contains the default values 
		for Eternity, Font Designer, Installer and anything else that may 
		get added to the PGME package. They are created at build time and
		contain every customizable setting for Themes, Language and Keymap
		files. These are built into the programs. You would only want These
		for developing custom themes and such. They should really not be
		used except as a reference. 