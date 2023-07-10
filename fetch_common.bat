set CurrDir=%CD%
for %%* in (.) do set CurrDirName=%%~nx*

REM --------------------------------------------------------
REM Get Common folder content: works only for Git repo
REM --------------------------------------------------------
python %CurrDir%\build\fetch_dependencies.py

