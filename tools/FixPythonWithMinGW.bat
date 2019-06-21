

set CURRENT_PATH=%~dp0%

cd %~dp0\..\python37\libs\

echo EXPORTS > python37.def
nm python37.lib | grep " T _" | sed "s/.* T _//" >> python37.def
dlltool --input-def python37.def --dllname python37 --output-lib libpython37_mingw.a