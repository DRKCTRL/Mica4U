@echo off
cls
setlocal enabledelayedexpansion

:: Enable ANSI color support
reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f >nul 2>&1

:: Define ANSI color codes using PowerShell
for /f "delims=" %%a in ('powershell -Command "[char]27"') do set "ESC=%%a"
set "RESET=%ESC%[0m"
set "BOLD=%ESC%[1m"
set "RED=%ESC%[91m"
set "GREEN=%ESC%[92m"
set "YELLOW=%ESC%[93m"
set "BLUE=%ESC%[94m"
set "MAGENTA=%ESC%[95m"
set "CYAN=%ESC%[96m"

:: Parse command line arguments
set "VERBOSE=false"
set "BUILD_X86=true"
set "BUILD_X64=true"

:parse_args
if "%1"=="" goto end_parse_args
if /i "%1"=="-v" set "VERBOSE=true" & shift & goto parse_args
if /i "%1"=="--verbose" set "VERBOSE=true" & shift & goto parse_args
if /i "%1"=="--x86" set "BUILD_X86=true" & set "BUILD_X64=false" & shift & goto parse_args
if /i "%1"=="--x64" set "BUILD_X86=false" & set "BUILD_X64=true" & shift & goto parse_args
if /i "%1"=="--all" set "BUILD_X86=true" & set "BUILD_X64=true" & shift & goto parse_args
shift
goto parse_args
:end_parse_args

:: Build Configuration
set "BUILD_VERSION=1.6.9"
set "LOG_FILE=build.log"
set "BUILD_DIR=compiled"
set "OUTPUT_DIR=%BUILD_DIR%\output"
set "DIST_DIR=%BUILD_DIR%\dist"
set "BUILD_TEMP=%BUILD_DIR%\build"
set "INSTALLER_DIR=%BUILD_DIR%\installer"

echo %BOLD%%BLUE%Starting Mica4U build process v%BUILD_VERSION%...%RESET%
echo %BOLD%%BLUE%----------------------------------------%RESET%

if "%BUILD_X86%"=="true" if "%BUILD_X64%"=="true" (
    echo %BOLD%%CYAN%Building for both x86 and x64 architectures%RESET%
) else if "%BUILD_X86%"=="true" (
    echo %BOLD%%CYAN%Building for x86 architecture only%RESET%
) else if "%BUILD_X64%"=="true" (
    echo %BOLD%%CYAN%Building for x64 architecture only%RESET%
)

:: Initialize logging
echo [%date% %time%] Starting build process (Version %BUILD_VERSION%) > "%CD%\%LOG_FILE%"

echo %BOLD%%YELLOW%[1/6]%RESET% %CYAN%Updating version information...%RESET%
call :update_version
if errorlevel 1 (
    echo %BOLD%%RED%ERROR: Version update failed! Check %LOG_FILE% for details.%RESET%
    exit /b 1
)

echo %BOLD%%YELLOW%[2/6]%RESET% %CYAN%Cleaning previous build files...%RESET%
call :cleanup
if errorlevel 1 (
    echo %BOLD%%RED%ERROR: Cleanup failed! Check %LOG_FILE% for details.%RESET%
    exit /b 1
)

set "STEPS=6"
if "%BUILD_X86%"=="true" if "%BUILD_X64%"=="true" set "STEPS=7"

if "%BUILD_X86%"=="true" (
    echo %BOLD%%YELLOW%[3/%STEPS%]%RESET% %CYAN%Building x86 application...%RESET%
    call :build_arch x86
    if errorlevel 1 (
        echo %BOLD%%RED%ERROR: x86 build failed! Check %LOG_FILE% for details.%RESET%
        exit /b 1
    )
)

if "%BUILD_X64%"=="true" (
    if "%BUILD_X86%"=="true" (
        echo %BOLD%%YELLOW%[4/%STEPS%]%RESET% %CYAN%Building x64 application...%RESET%
    ) else (
        echo %BOLD%%YELLOW%[3/%STEPS%]%RESET% %CYAN%Building x64 application...%RESET%
    )
    call :build_arch x64
    if errorlevel 1 (
        echo %BOLD%%RED%ERROR: x64 build failed! Check %LOG_FILE% for details.%RESET%
        exit /b 1
    )
)

set "STEP=5"
if "%BUILD_X86%"=="false" set /a "STEP-=1"

echo %BOLD%%YELLOW%[%STEP%/%STEPS%]%RESET% %CYAN%Creating portable version(s)...%RESET%
call :create_portable
if errorlevel 1 (
    echo %BOLD%%RED%ERROR: Portable version creation failed! Check %LOG_FILE% for details.%RESET%
    exit /b 1
)

set /a "STEP+=1"
echo %BOLD%%YELLOW%[%STEP%/%STEPS%]%RESET% %CYAN%Creating installer(s)...%RESET%
call :create_installer
if errorlevel 1 (
    echo %BOLD%%RED%ERROR: Installer creation failed! Check %LOG_FILE% for details.%RESET%
    exit /b 1
)

set /a "STEP+=1"
echo %BOLD%%YELLOW%[%STEP%/%STEPS%]%RESET% %CYAN%Performing final cleanup...%RESET%
call :final_cleanup
if errorlevel 1 (
    echo %BOLD%%RED%ERROR: Final cleanup failed! Check %LOG_FILE% for details.%RESET%
    exit /b 1
)

echo.
echo %BOLD%%GREEN%Build completed successfully!%RESET%
echo %BOLD%%BLUE%----------------------------------------%RESET%
echo %CYAN%The portable version(s) can be found in: %OUTPUT_DIR%%RESET%
if exist "%OUTPUT_DIR%\Mica4U_Setup_x86.exe" (
    echo %CYAN%The x86 installer can be found in: %OUTPUT_DIR%\Mica4U_Setup_x86.exe%RESET%
)
if exist "%OUTPUT_DIR%\Mica4U_Setup_x64.exe" (
    echo %CYAN%The x64 installer can be found in: %OUTPUT_DIR%\Mica4U_Setup_x64.exe%RESET%
)
echo.
pause
exit /b 0

:cleanup
echo [%date% %time%] Cleaning up previous build... >> "%CD%\%LOG_FILE%"
if "%VERBOSE%"=="true" (
    echo %MAGENTA%[LOG]%RESET% %CYAN%Cleaning up previous build...%RESET%
)
for %%d in ("%OUTPUT_DIR%" "%DIST_DIR%" "%BUILD_TEMP%" "%INSTALLER_DIR%") do (
    if exist "%%~d" (
        if "%VERBOSE%"=="true" echo %MAGENTA%[LOG]%RESET% %CYAN%Removing directory: %%~d%RESET%
        rmdir /s /q "%%~d" 2>> "%CD%\%LOG_FILE%"
        if errorlevel 1 (
            echo [%date% %time%] Failed to remove directory: %%~d >> "%CD%\%LOG_FILE%"
            exit /b 1
        )
    )
)
exit /b 0

:build_arch
set "ARCH=%~1"
echo [%date% %time%] Building %ARCH% application... >> "%CD%\%LOG_FILE%"
cd "%BUILD_DIR%"

:: Create architecture-specific output directories
if not exist "dist_%ARCH%" mkdir "dist_%ARCH%" 2>> "%CD%\..\%LOG_FILE%"
if not exist "build_%ARCH%" mkdir "build_%ARCH%" 2>> "%CD%\..\%LOG_FILE%"

:: Set environment variable for PyInstaller to use
set "TARGET_ARCH=%ARCH%"

if "%VERBOSE%"=="true" (
    echo %MAGENTA%[LOG]%RESET% %CYAN%Running PyInstaller for %ARCH%...%RESET%
    :: Use PowerShell to capture output and redirect to both console and log file
    powershell -Command "pyinstaller Mica4U.spec --clean --workpath='build_%ARCH%' --distpath='dist_%ARCH%' 2>&1 | ForEach-Object { Add-Content -Path '%CD%\..\%LOG_FILE%' -Value $_; Write-Host $_ }"
) else (
    pyinstaller Mica4U.spec --clean --workpath="build_%ARCH%" --distpath="dist_%ARCH%" 2>> "%CD%\..\%LOG_FILE%"
)

:: Check if dist_ARCH directory contains Mica4U.exe, which indicates success
if not exist "dist_%ARCH%\Mica4U.exe" (
    echo [%date% %time%] PyInstaller build failed for %ARCH%! >> "%CD%\..\%LOG_FILE%"
    exit /b 1
)

:: Log success
echo [%date% %time%] PyInstaller build succeeded for %ARCH%! >> "%CD%\..\%LOG_FILE%"

:: Copy files to main dist directory
if not exist "dist" mkdir "dist" 2>> "%CD%\..\%LOG_FILE%"
copy "dist_%ARCH%\Mica4U.exe" "dist\Mica4U_%ARCH%.exe" >nul 2>> "%CD%\..\%LOG_FILE%"

cd ..
exit /b 0

:create_portable
echo [%date% %time%] Creating portable version(s)... >> "%CD%\%LOG_FILE%"
if "%VERBOSE%"=="true" (
    echo %MAGENTA%[LOG]%RESET% %CYAN%Creating portable version directories...%RESET%
)
if not exist "%OUTPUT_DIR%\portable" mkdir "%OUTPUT_DIR%\portable" 2>> "%CD%\%LOG_FILE%"

:: Process x86 if enabled
if "%BUILD_X86%"=="true" (
    if not exist "%OUTPUT_DIR%\portable\Mica4U_x86" mkdir "%OUTPUT_DIR%\portable\Mica4U_x86" 2>> "%CD%\%LOG_FILE%"
    
    :: Create portable.ini file
    echo [Settings] > "%OUTPUT_DIR%\portable\Mica4U_x86\portable.ini"
    echo Version=%BUILD_VERSION% >> "%OUTPUT_DIR%\portable\Mica4U_x86\portable.ini"
    echo Portable=true >> "%OUTPUT_DIR%\portable\Mica4U_x86\portable.ini"
    echo Architecture=x86 >> "%OUTPUT_DIR%\portable\Mica4U_x86\portable.ini"
    
    :: Copy files
    copy "%BUILD_DIR%\dist\Mica4U_x86.exe" "%OUTPUT_DIR%\portable\Mica4U_x86\Mica4U.exe" >nul 2>> "%CD%\%LOG_FILE%"
    copy "README.md" "%OUTPUT_DIR%\portable\Mica4U_x86\" >nul 2>> "%CD%\%LOG_FILE%"
    copy "LICENSE" "%OUTPUT_DIR%\portable\Mica4U_x86\" >nul 2>> "%CD%\%LOG_FILE%"
    copy "ExplorerBlurMica.dll" "%OUTPUT_DIR%\portable\Mica4U_x86\" >nul 2>> "%CD%\%LOG_FILE%"
    copy "initialise.cmd" "%OUTPUT_DIR%\portable\Mica4U_x86\" >nul 2>> "%CD%\%LOG_FILE%"
    
    :: Create x86 zip
    if "%VERBOSE%"=="true" echo %MAGENTA%[LOG]%RESET% %CYAN%Creating x86 portable zip archive...%RESET%
    where 7z >nul 2>&1
    if errorlevel 1 (
        powershell -Command "Add-Type -Assembly 'System.IO.Compression.FileSystem'; [System.IO.Compression.ZipFile]::CreateFromDirectory('%OUTPUT_DIR%\portable\Mica4U_x86', '%OUTPUT_DIR%\Mica4U_portable_x86.zip')" 2>> "%CD%\%LOG_FILE%"
    ) else (
        cd "%OUTPUT_DIR%\portable\Mica4U_x86"
        7z a -tzip "..\..\Mica4U_portable_x86.zip" "*" -mx9 -r >nul 2>> "%CD%\%LOG_FILE%"
        cd ..\..\..\..
    )
)

:: Process x64 if enabled
if "%BUILD_X64%"=="true" (
    if not exist "%OUTPUT_DIR%\portable\Mica4U_x64" mkdir "%OUTPUT_DIR%\portable\Mica4U_x64" 2>> "%CD%\%LOG_FILE%"
    
    :: Create portable.ini file
    echo [Settings] > "%OUTPUT_DIR%\portable\Mica4U_x64\portable.ini"
    echo Version=%BUILD_VERSION% >> "%OUTPUT_DIR%\portable\Mica4U_x64\portable.ini"
    echo Portable=true >> "%OUTPUT_DIR%\portable\Mica4U_x64\portable.ini"
    echo Architecture=x64 >> "%OUTPUT_DIR%\portable\Mica4U_x64\portable.ini"
    
    :: Copy files
    copy "%BUILD_DIR%\dist\Mica4U_x64.exe" "%OUTPUT_DIR%\portable\Mica4U_x64\Mica4U.exe" >nul 2>> "%CD%\%LOG_FILE%"
    copy "README.md" "%OUTPUT_DIR%\portable\Mica4U_x64\" >nul 2>> "%CD%\%LOG_FILE%"
    copy "LICENSE" "%OUTPUT_DIR%\portable\Mica4U_x64\" >nul 2>> "%CD%\%LOG_FILE%"
    copy "ExplorerBlurMica.dll" "%OUTPUT_DIR%\portable\Mica4U_x64\" >nul 2>> "%CD%\%LOG_FILE%"
    copy "initialise.cmd" "%OUTPUT_DIR%\portable\Mica4U_x64\" >nul 2>> "%CD%\%LOG_FILE%"
    
    :: Create x64 zip
    if "%VERBOSE%"=="true" echo %MAGENTA%[LOG]%RESET% %CYAN%Creating x64 portable zip archive...%RESET%
    where 7z >nul 2>&1
    if errorlevel 1 (
        powershell -Command "Add-Type -Assembly 'System.IO.Compression.FileSystem'; [System.IO.Compression.ZipFile]::CreateFromDirectory('%OUTPUT_DIR%\portable\Mica4U_x64', '%OUTPUT_DIR%\Mica4U_portable_x64.zip')" 2>> "%CD%\%LOG_FILE%"
    ) else (
        cd "%OUTPUT_DIR%\portable\Mica4U_x64"
        7z a -tzip "..\..\Mica4U_portable_x64.zip" "*" -mx9 -r >nul 2>> "%CD%\%LOG_FILE%"
        cd ..\..\..\..
    )
)

if "%VERBOSE%"=="true" (
    echo %MAGENTA%[LOG]%RESET% %CYAN%Cleaning up temporary portable files...%RESET%
)
rmdir /s /q "%OUTPUT_DIR%\portable" 2>> "%CD%\%LOG_FILE%"
exit /b 0

:create_installer
echo [%DATE% %TIME%] Creating installer(s)... >> "%CD%\%LOG_FILE%"

if "%BUILD_X86%"=="true" (
    if "%VERBOSE%"=="true" (
        echo %MAGENTA%[LOG]%RESET% %CYAN%Creating x86 installer using Inno Setup...%RESET%
        copy "%BUILD_DIR%\dist\Mica4U_x86.exe" "%BUILD_DIR%\dist\Mica4U.exe" >nul 2>&1
        iscc "%BUILD_DIR%\installer.iss" /O"%OUTPUT_DIR%" /F"Mica4U_Setup_x86" /DArch="x86"
    ) else (
        copy "%BUILD_DIR%\dist\Mica4U_x86.exe" "%BUILD_DIR%\dist\Mica4U.exe" >nul 2>&1
        iscc "%BUILD_DIR%\installer.iss" /O"%OUTPUT_DIR%" /F"Mica4U_Setup_x86" /DArch="x86" >nul 2>> "%CD%\%LOG_FILE%"
    )
    
    if errorlevel 1 (
        echo %BOLD%%RED%[%DATE% %TIME%]%RESET% Error creating x86 installer.
        echo [%DATE% %TIME%] Error creating x86 installer. >> "%CD%\%LOG_FILE%"
        exit /b 1
    )
)

if "%BUILD_X64%"=="true" (
    if "%VERBOSE%"=="true" (
        echo %MAGENTA%[LOG]%RESET% %CYAN%Creating x64 installer using Inno Setup...%RESET%
        copy "%BUILD_DIR%\dist\Mica4U_x64.exe" "%BUILD_DIR%\dist\Mica4U.exe" >nul 2>&1
        iscc "%BUILD_DIR%\installer.iss" /O"%OUTPUT_DIR%" /F"Mica4U_Setup_x64" /DArch="x64"
    ) else (
        copy "%BUILD_DIR%\dist\Mica4U_x64.exe" "%BUILD_DIR%\dist\Mica4U.exe" >nul 2>&1
        iscc "%BUILD_DIR%\installer.iss" /O"%OUTPUT_DIR%" /F"Mica4U_Setup_x64" /DArch="x64" >nul 2>> "%CD%\%LOG_FILE%"
    )
    
    if errorlevel 1 (
        echo %BOLD%%RED%[%DATE% %TIME%]%RESET% Error creating x64 installer.
        echo [%DATE% %TIME%] Error creating x64 installer. >> "%CD%\%LOG_FILE%"
        exit /b 1
    )
)

echo [%DATE% %TIME%] Installer(s) created successfully. >> "%CD%\%LOG_FILE%"
goto :eof

:final_cleanup
echo [%date% %time%] Cleaning up temporary files... >> "%CD%\%LOG_FILE%"
if "%VERBOSE%"=="true" (
    echo %MAGENTA%[LOG]%RESET% %CYAN%Cleaning up temporary build files...%RESET%
)
for %%d in ("%DIST_DIR%" "%BUILD_TEMP%" "%INSTALLER_DIR%") do (
    if exist "%%~d" (
        if "%VERBOSE%"=="true" echo %MAGENTA%[LOG]%RESET% %CYAN%Removing directory: %%~d%RESET%
        rmdir /s /q "%%~d" 2>> "%CD%\%LOG_FILE%"
        if errorlevel 1 (
            echo [%date% %time%] Failed to remove directory: %%~d >> "%CD%\%LOG_FILE%"
            exit /b 1
        )
    )
)

for %%d in ("%BUILD_DIR%\dist_x86" "%BUILD_DIR%\dist_x64" "%BUILD_DIR%\build_x86" "%BUILD_DIR%\build_x64") do (
    if exist "%%~d" (
        if "%VERBOSE%"=="true" echo %MAGENTA%[LOG]%RESET% %CYAN%Removing directory: %%~d%RESET%
        rmdir /s /q "%%~d" 2>> "%CD%\%LOG_FILE%"
    )
)
exit /b 0

:update_version
echo [%date% %time%] Updating version in main.py... >> "%CD%\%LOG_FILE%"
if "%VERBOSE%"=="true" (
    echo %MAGENTA%[LOG]%RESET% %CYAN%Updating version in main.py...%RESET%
)
powershell -Command "(Get-Content 'main.py') -replace 'VERSION = \"[0-9]+\.[0-9]+\.[0-9]+\"', 'VERSION = \"%BUILD_VERSION%\"' | Set-Content 'main.py'"
if errorlevel 1 (
    echo [%date% %time%] Failed to update version in main.py >> "%CD%\%LOG_FILE%"
    exit /b 1
)

if "%VERBOSE%"=="true" (
    echo %MAGENTA%[LOG]%RESET% %CYAN%Updating version in installer.iss...%RESET%
)
echo [%date% %time%] Updating version in installer.iss... >> "%CD%\%LOG_FILE%"
powershell -Command "(Get-Content 'compiled\installer.iss') -replace '#define MyAppVersion \"[0-9]+\.[0-9]+\.[0-9]+\"', '#define MyAppVersion \"%BUILD_VERSION%\"' | Set-Content 'compiled\installer.iss'"
if errorlevel 1 (
    echo [%date% %time%] Failed to update version in installer.iss >> "%CD%\%LOG_FILE%"
    exit /b 1
)
exit /b 0 