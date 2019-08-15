# NplBrowser
Running chromium embedded framework(cef) brwoser in paracraft

### Building with vs2017
 1. clone NplBrowser and its submodule
```
git clone --recursive https://github.com/tatfook/NplBrowser.git
```
 2. run cmd 
```
.\build.bat
```
 3. check binaries at: **deps/NPLRuntime/ParaWorld/bin32/cef3**
- build nplruntime [optional]
    - build boost 1.68.0, set environment: BOOST_ROOT
 ```bash
# debug
.\b2.exe address-model=32 toolset=msvc-14.1 runtime-link=static threading=multi variant=debug --with-thread --with-date_time --with-filesystem --with-system --with-chrono --with-signals --with-serialization --with-iostreams --with-regex stage
# release
.\b2.exe address-model=32 toolset=msvc-14.1 runtime-link=static threading=multi variant=release --with-thread --with-date_time --with-filesystem --with-system --with-chrono --with-signals --with-serialization --with-iostreams --with-regex stage
``` 
    - build nplruntime

### Deps
 - NplBrowser depends on cef3 [03/08/2019 - CEF 3.3626.1895.g7001d56 / Chromium 72.0.3626.121](http://opensource.spotify.com/cefbuilds/index.html)
 - build boost 

- [chromium cmd line](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-command-line-arguments)
### Running cefclient.exe directly with command lines
```
cefclient.exe  -window_title="NplBrowser" -window_name="nplbrowser_wnd" -bounds="0,0,800,600" -url="https://keepwork.com"
```

### Running in npl
 - the main logic is in [NplBrowserPlugin](https://github.com/NPLPackages/paracraft/blob/master/script/apps/Aries/Creator/Game/NplBrowser/NplBrowserPlugin.lua)

```lua
---------------------------------------------------------------------------
NPL.load("(gl)script/apps/Aries/Creator/Game/NplBrowser/NplBrowserPlugin.lua");
local NplBrowserPlugin = commonlib.gettable("NplBrowser.NplBrowserPlugin");
local id = "nplbrowser_wnd";
NplBrowserPlugin.Start({id = id, url = "http://www.keepwork.com", withControl = true, x = 200, y = 200, width = 800, height = 600, });
NplBrowserPlugin.OnCreatedCallback(id,function()
    NplBrowserPlugin.Open({id = id, url = "http://www.keepwork.com", resize = true, x = 100, y = 100, width = 300, height = 300, });
    NplBrowserPlugin.Show({id = id, visible = false});
    NplBrowserPlugin.Show({id = id, visible = true});
    NplBrowserPlugin.Zoom({id = id, zoom = 1}); --200%
    NplBrowserPlugin.EnableWindow({id = id, enabled = false});
    NplBrowserPlugin.ChangePosSize({id = id, x = 100, y = 100, width = 800, height = 400, });
    NplBrowserPlugin.Quit({id = id,});
end)
---------------------------------------------------------------------------
```
 - [NplBrowserPage](https://github.com/NPLPackages/paracraft/blob/master/script/apps/Aries/Creator/Game/NplBrowser/NplBrowserPage.lua) is more easier for creating cef browser window in npl
```lua
NPL.load("(gl)script/apps/Aries/Creator/Game/NplBrowser/NplBrowserPage.lua");
local NplBrowserPage = commonlib.gettable("NplBrowser.NplBrowserPage");
NplBrowserPage.Open();
```
### Links
- [Cef3 wiki](https://bitbucket.org/chromiumembedded/cef/wiki/Home)
- [cef cmd line](https://peter.sh/experiments/chromium-command-line-switches/)
- [NplBrowser assets](https://api.github.com/repos/tatfook/NplBrowser/releases?per_page=100)(deprecated)

![image](https://user-images.githubusercontent.com/5885941/55061101-e26efe80-50ad-11e9-94a9-edd185e880bd.png)


