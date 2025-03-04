ECHO Starting X11 server ...

REM Check if VCXSRV_EXE_PATH environment variable is set
IF NOT DEFINED VCXSRV_EXE_PATH (
    SET VCXSRV_EXE_PATH=C:\Program Files\VcXsrv\vcxsrv.exe
    ECHO Environment variable VCXSRV_EXE_PATH missing. Defaulting to %VCXSRV_EXE_PATH%
)

start /b "" "%VCXSRV_EXE_PATH%" :0 -ac -multiwindow -from localhost -silent-dup-error