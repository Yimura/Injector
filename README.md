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
- Automatic mode (webserver required)
- Manual mode (Inject a dll in a target application)

### Automatic Mode

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

If you run the injector for the first time and no `settings.json` is present in the folder from which you executed the Injector it will create an example file.

```js
{
    "dll_provider": "http://example.com", // the host from which to download updates
    "target_application": "wordpad.exe", // the target application to inject into
    "version": 11 // the version that is compared against on the server, this one should be lower than what's on the webserver
}
```

That's it.

### Manual Mode

Alternatively you can set which program to inject into and which dll to use, open a terminal and pass 2 arguments:
|Position|Argument|
|--|--|
|1|Target Application|
|2|DLL to inject into the application|

Example:
```bash
Injector.exe wordpad.exe example.dll
```
