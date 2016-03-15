# Troubleshooting #

If you are having issues getting GWCA to work, you're probably experiencing an issue that has been noticed by others before you.
Here is the list of currently known fixes for making GWCA work!
<br />
<br />

  * If you don't know what to do with the GWCA Dll, you should know, that it has to be injected into the Guild Wars process. There are numerous injectors available online and also on [Game Revision](http://gamerevision.com/). If you're simply googling for one, watch out, as there are examples of people who put malware in their injectors - so take a look at where you're getting it from.
<br />

  * If you running Windows Vista or Windows 7, disable User Account Control (UAC). There are numerous guides on the interwebs, so just google "Disable UAC" or your mother-language counterpart and follow instructions. Some will say that simply running the scripts "As Admin" is enough, but it is first of all tedious to do that every time you start a script, and there are rare cases where you **have** to disable UAC to make GWCA work.
<br />

  * If your operating system is 64bit/x64, it is very important always to run everything related to GWCA in x86 (32bit) mode. This is due to the fact, that GWCA was written for the x86 platform, and even recompiling the whole project in x64 isn't assured to make everything work right.
<br />

  * If you have been using pre-pipes versions of GWCA (rev219 and below) and trying to make the transition to the pipes versions (rev226 and up) you may experience that it doesn't work. This is most likely related to your version of AutoIt. To function properly, GWCA requires AutoIt version 3.3.2.0 or higher. There have been reported issues with version 3.3.6.1 though, so I suggest staying away from it. If you're using GWCA.NET - the C# library - this is of course not of any importance.