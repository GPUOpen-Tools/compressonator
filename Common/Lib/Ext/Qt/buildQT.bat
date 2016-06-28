REM make sure you do a "subst Q: c:\amdtools\main" (replacing the directory name with the correct one for your machine)
REM run this batch file from a VS2010 command prompt

Q:

set QTBASEDIR=q:\common\lib\ext\qt\4.7.4
set QTLIBDIR=%QTBASEDIR%\lib
set QTBINDIR=%QTBASEDIR%\bin
set QTINCLUDEDIR=%QTBASEDIR%\include
set QTBUILDDIR=%QTBASEDIR%\Build\Windows
set QTBUILDPLATFORMDIR=%QTBUILDDIR%\x86

set QSCINTILLABASEDIR=q:\common\lib\ext\qscintilla\2.5.1
set QSCINTILLABUILDDIR=%QSCINTILLABASEDIR%\Build\Windows
set QSCINTILLABUILDPLATFORMDIR=%QSCINTILLABUILDDIR%\x86

GOTO :DOIT

cd %QSCINTILLABASEDIR%
attrib -r /s *.*
del /s *.pdb

cd %QTBASEDIR%
attrib -r /s *.*
del /s *.pdb

if not exist %QTBUILDDIR%. (mkdir %QTBUILDDIR%)
if not exist %QTBUILDPLATFORMDIR%. (mkdir %QTBUILDPLATFORMDIR%)
if not exist %QSCINTILLABUILDDIR%. (mkdir %QSCINTILLABUILDDIR%)
if not exist %QSCINTILLABUILDPLATFORMDIR%. (mkdir %QSCINTILLABUILDPLATFORMDIR%)

REM move the Linux qmake out of the way because configure.exe will try to run the Linux version rather than the Windows version (because it does a CreateProcess call on "qmake", not "qmake.exe")
ren bin\qmake qmake.linux.hold

:DOIT

REM *************************************************************
REM Build debug static
REM *************************************************************
cd %QTBASEDIR%
copy mkspecs\win32-msvc2010\qmake.static.conf mkspecs\win32-msvc2010\qmake.conf
del %QTLIBDIR%\*.lib
del %QTLIBDIR%\*.dll
del %QTLIBDIR%\*.pdb

set STATICORSHARED=static
set STATICORDYNAMIC=static
set DEBUGORRELEASE=debug

if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include)

REM removed the following from the next configure command:  -headerdir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\include -libdir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\lib -bindir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\bin
configure.exe -qt-sql-sqlite -no-qt3support -no-dbus -no-phonon -no-phonon-backend -webkit -%DEBUGORRELEASE% -opensource -%STATICORSHARED% -qtlibinfix AmdDt474 -platform win32-msvc2010 -confirm-license
nmake
nmake install
copy  %QTBINDIR%\*.exe %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin
copy  %QTLIBDIR%\*.lib %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib
xcopy /s %QTINCLUDEDIR% %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include

if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%)
if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%)
if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib)

cd %QSCINTILLABASEDIR%\Qt4
copy qscintilla.pro.static qscintilla.pro
%QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin\qmake.exe qscintilla.pro
nmake
copy %DEBUGORRELEASE%\*.lib %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib

GOTO :END

REM *************************************************************
REM Build release static
REM *************************************************************
cd %QTBASEDIR%
del %QTLIBDIR%\*.lib
del %QTLIBDIR%\*.dll
del %QTLIBDIR%\*.pdb

set DEBUGORRELEASE=release

if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include)

REM removed the following from the next configure command:  -headerdir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\include -libdir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\lib -bindir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\bin
configure.exe -qt-sql-sqlite -no-qt3support -no-dbus -no-phonon -no-phonon-backend -webkit -%DEBUGORRELEASE% -opensource -%STATICORSHARED% -qtlibinfix AmdDt474 -platform win32-msvc2010 -confirm-license
nmake 
nmake install
copy  %QTBINDIR%\*.exe %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin
copy  %QTLIBDIR%\*.lib %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib
xcopy /s %QTINCLUDEDIR% %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include

if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%)
if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%)
if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib)

cd %QSCINTILLABASEDIR%\Qt4
%QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin\qmake.exe qscintilla.pro
nmake
copy %DEBUGORRELEASE%\*.lib %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib

REM *************************************************************
REM Build debug dynamic
REM *************************************************************
cd %QTBASEDIR%
copy mkspecs\win32-msvc2010\qmake.dynamic.conf mkspecs\win32-msvc2010\qmake.conf
del %QTLIBDIR%\*.lib
del %QTLIBDIR%\*.dll
del %QTLIBDIR%\*.pdb

set STATICORSHARED=shared
set STATICORDYNAMIC=dynamic
set DEBUGORRELEASE=debug

if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include)

REM removed the following from the next configure command:  -headerdir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\include -libdir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\lib -bindir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\bin
configure.exe -qt-sql-sqlite -no-qt3support -no-dbus -no-phonon -no-phonon-backend -webkit -%DEBUGORRELEASE% -opensource -%STATICORSHARED% -qtlibinfix AmdDt474 -platform win32-msvc2010 -confirm-license
nmake 
nmake install
copy  %QTBINDIR%\*.exe %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin
copy  %QTLIBDIR%\*.lib %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib
copy  %QTLIBDIR%\*.dll %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib
copy  %QTLIBDIR%\*.pdb %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib
xcopy /s %QTINCLUDEDIR% %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include

if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%)
if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%)
if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib)

cd %QSCINTILLABASEDIR%\Qt4
copy qscintilla.pro.dynamic qscintilla.pro
%QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin\qmake.exe qscintilla.pro
nmake
copy %DEBUGORRELEASE%\*.lib %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib

REM GOTO :END

REM *************************************************************
REM Build release dynamic
REM *************************************************************
cd %QTBASEDIR%
del %QTLIBDIR%\*.lib
del %QTLIBDIR%\*.dll
del %QTLIBDIR%\*.pdb

set DEBUGORRELEASE=release

if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib)
if not exist %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include. (mkdir %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include)

REM removed the following from the next configure command:  -headerdir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\include -libdir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\lib -bindir q:\common\lib\ext\qt\4.7.4\build\win32static\debug\bin
configure.exe -qt-sql-sqlite -no-qt3support -no-dbus -no-phonon -no-phonon-backend -webkit -%DEBUGORRELEASE% -opensource -%STATICORSHARED% -qtlibinfix AmdDt474 -platform win32-msvc2010 -confirm-license
nmake 
nmake install
copy  %QTBINDIR%\*.exe %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin
copy  %QTLIBDIR%\*.lib %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib
copy  %QTLIBDIR%\*.dll %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib
copy  %QTLIBDIR%\*.pdb %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib
xcopy /s %QTINCLUDEDIR% %QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\include

if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%)
if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%)
if not exist %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib. (mkdir %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib)

cd %QSCINTILLABASEDIR%\Qt4
%QTBUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\bin\qmake.exe qscintilla.pro
nmake
copy %DEBUGORRELEASE%\*.lib %QSCINTILLABUILDPLATFORMDIR%\%STATICORDYNAMIC%\%DEBUGORRELEASE%\lib

:END

REM move some stuff back into place
ren bin\qmake.linux.hold qmake

