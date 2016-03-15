# First-time User Guide #

If you have never used GWCA before, then you may need some information before you start or you'll end up being pretty damn confused probably!

First of all, you need to choose whether you use the pre-compiled .dll or compile the project yourself. The last option is only for advanced users who have Visual Studio 2008 or Visual Express C++ installed on their computers. For all normal users I suggest simply using the .dll that I have compiled for you.

## Using the pre-compiled Dll ##
First, go and download [Graphics.dll](http://gwca.googlecode.com/files/Graphics.dll) by clicking this link.

You will need a Dll Injector (google) to load GWCA, and to see that it's working, the window title will change when you log in to `Guild Wars - <Character Name>` where `<Character Name>` is your character's name of course.

Skip the next paragraph.

## Compiling the project ##
Download the latest source. This can be found by going to the Downloads tab and getting the .rar archive with the highest revision number or simply choosing the one with the tag `Featured`.

Un-pack the archive and open the solution by double-clicking `GW Client API.sln`. Now switch the compile mode from `Debug` to `Release` and `Build Solution`.

A few warnings should appear - but no errors! When compiled, locate the file `Graphics.dll` and do the same procedure as above, moving it to your Guild Wars folder.

## How do I use GWCA now? ##
A good idea would be to download all of the example scripts from http://code.google.com/p/gwca/source/browse/#svn/trunk/Scripts. Remember also to get `new list.txt` and `gwca-skillset.ini` - otherwise the Skill Log and Interrupt Bot will not function optimally.

To use the scripts you will of course need to have the latest version of AutoIt3 installed on your computer. Go to http://www.autoitscript.com/autoit3/downloads.shtml to download and install the latest version.

After you've tried the example scripts and taken a few peeks at their code, you may want to try making a script by yourself.

## Making your own scripts ##
There are a few mandatory things you will need to do in each and everyone of your scripts using GWCA.

  * Include the `GWCAConstants.au3` file, which contains the basic functions and declarations of variables that GWCA uses. Necessary for all GWCA scripts.

Also very important to note is the difference between the Cmd() and CmdCB() functions. When you just want to execute a command and not receive a return value, you use Cmd(). When the command you use gets a value back though, you have to use CmdCB().
Keep in mind that maximum amount of parameters with Cmd() and CmdCB() is 2.

So basically, if you were to make a standard script that just accessed the standard GWCA functionalities you would start out with:

```
#include "GWCAConstants.au3"

;Insert your own code now
```

And that's basically it! Now, you need to know something about what CmdCB() returns. When you use a command with a return then a global array called $cbVar is filled with the value(s). Return value 1 is in $cbVar[0](0.md) and return value 2 is in $cbVar[2](2.md). You may also put a variable in front of CmdCB() like `$myVar = CmdCB($CA_GetCoords, -2)` - just remember that $myVar will be an array aswell!

Now, lets add some commands to our script!


```
#include "GWCAConstants.au3"

$cbType = "float" ;Change return type to float value
$myCoords = CmdCB($CA_GETCOORDS, -2) ;Get your own coordinates

ConsoleWrite("[Check 1] X: "&$cbVar[0]&", Y: "&$cbVar[1]&@CRLF) ;Print to chat, notice that I use $cbVar array and not $myCoords. $cbVar only works until the next call to Cmd() or CmdCB(), but $myCoords will exist forever.

Cmd($CA_MOVE, _FloatToInt($cbVar[0]+450), _FloatToInt($cbVar[1]-150)) ;Now move to position X+450,Y-150 - remember to pack the float values

Sleep(4000) ;Wait 4 seconds

MoveEx($myCoords[0], $myCoords[1], 0) ;Now using MoveEx(), go to the exact same spot as you stood on before

Sleep(4000) ;Wait another 4 seconds

CmdCB($CA_GETCOORDS, -2) ;Get your coordinates again
ConsoleWrite("[Check 2] X: "&$cbVar[0]&", Y: "&$cbVar[1]&@CRLF) ;And print them to chat like before! Should be the same coordinates.
```

The comments pretty much say what's going on, so I'll just let you execute this script by yourself and let you observe what happens!

Good luck with your future GWCA-based scripts for Guild Wars!