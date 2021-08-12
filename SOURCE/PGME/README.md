Copyright 1992-2021, Jerome Shidel.

### Program Manager Eternity

This version of **_Program Manager Eternity_** and related files are released under the GNU GPL v2.0.

Simple Instructions for compiling PGME from archive sources.

	First you must have Turbo Pascal 7.0.

	Then either download the latest archives of PGME and QuickCRT.

	Extract the PGME archive. Then Extract the QuickCrt archive inside the PGME directory.

	then from a dos prommpt run make in the pgme directory.

	If all goes well, an Install directory will be created with the installer.

Instructions on compiling from svn sources.

	You still need turbo pascal 7.0.

	svn checkout svn://svn.code.sf.net/p/pgme/svn/trunk pgme
	(or from github at https://github.com/shidel/PGME)

	then from a dos prommpt run make in the pgme directory.

A QuickCRT snapshot is now included in the main PGME repository tree. This
snapshot is not kept in sync with the main version of QuickCRT. Rather
it should be considered a fork of that project. At present there are a couple
"living" forks of QuickCRT. There is this branch that was derived from
v8.4 and built-out with a object oriented application framework. Then there
is the previous 8.4 version without the OOP framework available at:

    svn checkout svn://svn.code.sf.net/p/quickcrt/svn/trunk pgme/quickcrt

Then there is the newer version 9.x without an OOP framework. But was being
migrated to support assembly only projects as well as Pascal. It is at

    https://github.com/LoopZ/QCrt

None of those living versions are not actually "better" than any of the others.
They are just different from each other with different purposes.
