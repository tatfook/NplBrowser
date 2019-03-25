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
- [Command Line Arguments](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-command-line-arguments)
### Command line args

- https://api.github.com/repos/tatfook/NplBrowser/releases?per_page=100
```lua
---------------------------------------------------------------------------
-window_title="NplBrowser"
-window_name="nplbrowser_wnd"
-parent_handle=""
-hide-controls
-hide-top-menu
-url="http://www.keepwork.com"
-bounds="0,0,800,600"
---------------------------------------------------------------------------
NPL.load("(gl)Mod/NplBrowser/NplBrowserPlugin.lua");
local NplBrowserPlugin = commonlib.gettable("Mod.NplBrowser.NplBrowserPlugin");
local id = "nplbrowser_wnd";
NplBrowserPlugin.Start({id = id, url = "http://www.keepwork.com", withControl = true, x = 0, y = 0, width = 800, height = 600, });
NplBrowserPlugin.Open({id = id, url = "http://www.keepwork.com", resize = true, x = 100, y = 100, width = 1024, height = 768, });
NplBrowserPlugin.Show({id = id, visible = false});
NplBrowserPlugin.EnableWindow({id = id, enabled = false});
NplBrowserPlugin.ChangePosSize({id = id, x = 100, y = 100, width = 400, height = 400, });
NplBrowserPlugin.Quit({id = id,});
---------------------------------------------------------------------------

-- start with cmdline directly
local parent_handle = ParaEngine.GetAttributeObject():GetField("AppHWND", 0);
parent_handle = tostring(parent_handle);
local cmdLine = string.format('
    -window_title="NplBrowser" 
    -window_name="nplbrowser_wnd" 
    -hide-top-menu 
    -url="http://www.keepwork.com" 
    -bounds="0,0,800,600"
    -parent_handle="%s"
',parent_handle);
ParaGlobal.ShellExecute("open", ParaIO.GetCurDirectory(0).."cef3\\cefclient.exe", cmdLine, "", 1); 
```
