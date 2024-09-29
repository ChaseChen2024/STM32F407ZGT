@ECHO ON
@REM 编译脚本
setlocal enabledelayedexpansion
set PRO_SELECT=PROJECT_BOOTLOADER
set PRO_PATH=%CD%
set BOOT_PATH=Component\bootloader\STM32F407ZGT-BOOTLOADER
set APP_PATH=%PRO_PATH%
echo %PRO_SELECT%
echo %PRO_PATH%
echo %BOOT_PATH%
echo %APP_PATH%

@REM 检查当前路径
echo Current directory: %CD%
if %PRO_PATH% == %CD% (
	echo %CD% is correct directory
) else (
	echo %CD% directory mismatch
	goto directory_error
)

if %PRO_SELECT% == PROJECT_BOOTLOADER ( goto bulid_bootloader_project ) else ( goto bulid_application_project )

:bulid_bootloader_project
@REM 打开buildloader 版本路径
CD %BOOT_PATH%
echo Bootloader directory: %CD%
set BOOTLOADER_PATH=%PRO_PATH%\%BOOT_PATH%
if %BOOTLOADER_PATH% == %CD% (
	echo %CD% is correct directory
) else (
	echo bootloader %CD% directory mismatch
	goto directory_error
)

goto make_build

:bulid_application_project
CD %APP_PATH%
echo application directory: %CD%
if %APP_PATH% == %CD% (
	echo %CD% is correct directory
) else (
	echo application: %CD% directory mismatch
	goto directory_error
)

:make_build

@REM make clean
@REM if %errorlevel% == 0 (
@REM 	echo make clean successfully
@REM ) else (
@REM 	echo make clean failed
@REM 	goto make_clean_failed
@REM )

make
if %errorlevel% == 0 (
	echo make successfully
	goto make_successfully
) else (
	echo make failed
	goto make_failed
)
:directory_error
:make_clean_failed
:make_failed
@ECHO OFF
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                    ########    ###     ####  ##                   ##
echo     ##                    ##         ## ##     ##   ##                   ##
echo     ##                    ##        ##   ##    ##   ##                   ##
echo     ##                    ######   ##     ##   ##   ##                   ##
echo     ##                    ##       #########   ##   ##                   ##
echo     ##                    ##       ##     ##   ##   ##                   ##
echo     ##                    ##       ##     ##  ####  ########             ##
echo     ##                                                                   ##
echo     #######################################################################

goto:end


:make_successfully
@ECHO OFF
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                 ########     ###     ######   ######              ##
echo     ##                 ##     ##   ## ##   ##    ## ##    ##             ##
echo     ##                 ##     ##  ##   ##  ##       ##                   ##
echo     ##                 ########  ##     ##  ######   ######              ##
echo     ##                 ##        #########       ##       ##             ##
echo     ##                 ##        ##     ## ##    ## ##    ##             ##
echo     ##                 ##        ##     ##  ######   ######              ##
echo     ##                                                                   ##
echo     #######################################################################

if %PRO_SELECT% == PROJECT_BOOTLOADER% ( 
	set PRO_SELECT=PROJECT_APPLICATION
	echo PRO_SELECT=!PRO_SELECT!
	echo ################# build bootloader completed #################
	echo ################## start build application ##################
	goto bulid_application_project
) else ( 
	echo ###################### build all completed ######################
	set COPY_PATH=%BOOTLOADER_PATH%\Build\QT201-BOOT.bin
	echo copy !COPY_PATH! !APP_PATH!
	MD !APP_PATH!\Release
	COPY /y !COPY_PATH! !APP_PATH!\Release\bootloader.bin
	COPY /y !APP_PATH!\Build\QT201.bin !APP_PATH!\Release\application.bin
	COPY /y !APP_PATH!\Build\QT201.* !APP_PATH!\Release
	@REM REN !APP_PATH!\Release\QT201.bin !APP_PATH!\Release\application.bin 
)



goto:end


:end

echo ###################### build end ######################

