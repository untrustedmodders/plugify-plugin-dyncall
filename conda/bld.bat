@echo off
REM bld.bat - For Windows builds

REM Create the target directories
if not exist "%PREFIX%\bin" mkdir "%PREFIX%\bin"
if not exist "%PREFIX%" mkdir "%PREFIX%"

REM Copy the DLL and plugin file
copy bin\plugify-plugin-dyncall.dll "%PREFIX%\bin\" || exit 1
copy plugify-plugin-dyncall.pplugin "%PREFIX%\" || exit 1

REM Create activation scripts
if not exist "%PREFIX%\etc\conda\activate.d" mkdir "%PREFIX%\etc\conda\activate.d"
if not exist "%PREFIX%\etc\conda\deactivate.d" mkdir "%PREFIX%\etc\conda\deactivate.d"

REM Create activation script
echo @echo off > "%PREFIX%\etc\conda\activate.d\plugify-plugin-dyncall.bat"
echo set "PLUGIFY_DYNCALL_PLUGIN_PATH=%%CONDA_PREFIX%%;%%PLUGIFY_DYNCALL_PLUGIN_PATH%%" >> "%PREFIX%\etc\conda\activate.d\plugify-plugin-dyncall.bat"

REM Create deactivation script  
echo @echo off > "%PREFIX%\etc\conda\deactivate.d\plugify-plugin-dyncall.bat"
echo set "PLUGIFY_DYNCALL_PLUGIN_PATH=%%PLUGIFY_DYNCALL_PLUGIN_PATH:%%CONDA_PREFIX%%;=%%" >> "%PREFIX%\etc\conda\deactivate.d\plugify-plugin-dyncall.bat"

exit 0
