# NplBrowser
Embed chromium into paracraft

### Building with vs2017
- clone NplBrowser and its submodule
```
git clone --recursive https://github.com/LiXizhi/NPLRuntime.git
```
- run build.bat
- check binaries at: **deps/NPLRuntime/ParaWorld/bin32/cef3**
- build nplruntime [optional]
    - build boost 1.68.0
    - build nplruntime


### Deps
 - cef3 win32 lib: [03/08/2019 - CEF 3.3626.1895.g7001d56 / Chromium 72.0.3626.121](http://opensource.spotify.com/cefbuilds/index.html)
 - build boost cmds
```bash
# debug
.\b2.exe address-model=32 toolset=msvc-14.1 runtime-link=static threading=multi variant=debug --with-thread --with-date_time --with-filesystem --with-system --with-chrono --with-signals --with-serialization --with-iostreams --with-regex stage
# release
.\b2.exe address-model=32 toolset=msvc-14.1 runtime-link=static threading=multi variant=release --with-thread --with-date_time --with-filesystem --with-system --with-chrono --with-signals --with-serialization --with-iostreams --with-regex stage
``` 
```lua
local id = "test_win";
local dll_name = "cef3/NplCefPlugin_d.dll";
local subProcessName = "cef3/NplCefProcess_d.exe";
local parentHandle = ParaEngine.GetAttributeObject():GetField("AppHWND", 0);
NPL.activate(dll_name,{ cmd = "Start", subProcessName = subProcessName, parentHandle = parentHandle, showTitleBar = false, }); 
NPL.activate(dll_name,{ cmd = "Open", id = id, url = "http://www.wikicraft.cn/", withControl = false, x = 0, y = 0, width = 400, height = 300, }); 
```
- [Command Line Arguments](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-command-line-arguments)
### Command line args
```
-window_title=""
-window_name=""
-parent_handle=""
-hide-controls
-hide-top-menu
-url=""
-bounds="0,0,800,600"
local parent_handle = ParaEngine.GetAttributeObject():GetField("AppHWND", 0);
local cmdLine = [[-window_title="NplBrowser" -window_name="nplbrowser_wnd" -hide-top-menu -url="http://www.keepwork.com" -bounds="0,0,800,600"]];
ParaGlobal.ShellExecute("open", ParaIO.GetCurDirectory(0).."cef3\\cefclient.exe", cmdLine, "", 1); 
```
