@echo off 
rem compress every file
set root=%cd%
set input=.\deps\NPLRuntime\ParaWorld\bin32\cef3\*.*
set output=.\deps\NPLRuntime\ParaWorld\bin32\cef3_publish\

rmdir %output%  /s /q
mkdir %output%

xcopy /s /y %input%  %output%

pushd %output%
	for /f "delims=" %%f in ('dir /b /a') do CALL :MakeZip %%f
popd


EXIT /B %ERRORLEVEL%

:MakeZip
	call %root%/7z.exe a %1.zip %1
	del /s /q %1
	rd /s /q %1
	echo %1
EXIT /B 0