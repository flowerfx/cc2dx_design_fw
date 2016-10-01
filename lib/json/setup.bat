@echo off

set configDir=%~dp0..\jsoncpp_config

set FORCECOPY=0

:: Get parameter on command line (skip batch file name)
SHIFT

FOR %%A IN (%*) DO (
IF %%A==force_copy (
set FORCECOPY=1
)
)

:: Create folder "in_app_purchase_config" to contain inapppurchase configuration files (if does not already exists)
IF NOT EXIST %configDir% md %configDir%

:: Verify if in_app_purchase_config.h already exists
IF %FORCECOPY%==1 (
set copyConfigFile=y
)ELSE IF EXIST "%configDir%\jsoncpp.h" (
set /p copyConfigFile="File 'jsoncpp.h' already exists. Do you want to overwrite it (y/n) ?"
)ELSE (
set copyConfigFile=y
)


:: Copy jsoncpp.h
IF  %copyConfigFile%==y (
echo Copying jsoncpp.h
copy "include\jsoncpp_.h" "%configDir%\jsoncpp.h"
)


:: Copy msvc configuration
call project\copy_config.bat project\vs2012\props\user 		vs2012
call project\copy_config.bat project\vs2013\props\user 		vs2013

:: Copy iphone configuration
call project\copy_config.bat project\xcode4\ios\xcconfigs_jsoncpp\user xcode4\ios\
call project\copy_config.bat project\xcode4\tvos\xcconfigs_jsoncpp\user xcode4\tvos\

:: Copy W8/WP8 configuration
call project\copy_config.bat project\vs2013_w81\props\user	vs2013_w81
call project\copy_config.bat project\vs2012_wp80\props\user	vs2012_wp80
call project\copy_config.bat project\vs2013_wp81\props\user	vs2013_wp81

:: Copy W8/WP8 configuration
call project\copy_config.bat project\vs2015_w10\props\user	vs2015_w10

call project\copy_config.bat project\linux\ linux
:end
