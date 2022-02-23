# Injector

An injector that fetches version.json from a webserver, downloads a dll and injects it into the game.

## Setup

To setup the build environment, run the following commands in your preferred terminal.

```bash
git clone https://github.com/Yimura/Injector.git --recurse
cd Injector
premake5 vs2019
```

Get premake5 from [here](https://premake.github.io/download/).

Then afterwards open Injector.sln and start coding.

## How to use

This injector has 2 ways of functioning:
- Remote mode (webserver required)
- Local mode (inject without webserver)

### Setup Remote Server

If you only wish to use the  local features just ignore this step and  skip to Injector settings.

Requirements:
 - Webserver

In the root of your webserver create a file called `version.json`, this file will tell our Injector what to do and if it should download a dll.

```js
{
	"file": "menu.dll", // path to the dll file on the webserver starting from the root / is prefixed in the binary
	"version": "0.1.3", // human readable version number
	"version_machine": 13 // version number that the injector will compare against
}
```

### Injector Settings

If you run the injector for the first time and no `settings.json` is present in the folder from which you executed the Injector it will create an example file.

```js
{
    "application_mode": 0,
    "injection_direction": 0,
    "local": {
        "file": "./some_dll.dll"
    },
    "target_application": "wordpad.exe",
    "web_server": {
        "dll_provider": "http://example.com",
        "file": "latest.dll",
        "version": -1
    }
}
```

|Name|Description|Value|
|--|--|--|
|`application_mode` | If the injector runs locally or with a remote server. | `WEB_SERVER=0` & `LOCAL=1` |
|`target_application`| The process name of the application to inject into.| `string`|
|`local.file`|The local path to a DLL.|`string`|
|`web_server.dll_provider`| Web Server origin. | `string` |
|`web_server.file`|The file name to save the DLL to locally.|`string`|
|`web_server.version`|Machine version integer to compare with the remote version.|`int`|

```
injection_direction is currently not implemented, but still exists for future API.
```

That's it.
