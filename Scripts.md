This page will try to describe the different scripts available in the `trunk/Scripts/` folder which can be located by pressing 'Source':
[trunk/Source/-link](http://code.google.com/p/gwca/source/browse/#svn/trunk/Scripts)


# GWCAConstants.au3 #
This file contains the basis for all GWCA scripts to function. There are the command enum declaration, the basic variables and the functions.

Not much to say about this file really, but remember that it does nothing by itself - at all!
Just `#include "GWCAConstants.au3"` at the top of your scripts like it is the procedure in all the other scripts and you will never have to care about this file to be honest.



# GWCA Commander.au3 #
This is the debugging/information gathering tool of GWCA. It contains all of the GWCA commands in a drop-down list and a GUI that allows you to easily pull the information you need to create your bot or tool.
By running this script from the SciTe editor, the values returned by GWCA will also be printed to console, which is probably more handy if you're gathering lots of info, so you can just copy-paste.



# GWCA SkillLog.au3 #
The Skill Logger is a quite simple application. It simply displays whenever a skill is used along with some basic information. Not really that useful by itself, but great for demonstration purposes.



# GWCA Interrupt Bot.au3 #
The most powerful script that has been created using GWCA so far.
The Interrupt Bot is a highly advanced, customizable tool that doesn't take too long to get used to, but can provide great results.
The bot has been optimized and thoroughly tested to assure that your interrupts look genuine. It's not about hitting those 1/4 casts, but more to steadily interrupt, without missing, and allow you to do whatever you feel like at the same time. You may also manually interrupt still, granted you're fast enough, it takes everything into account really.



# GWCA Chestbot.au3 #
This bot runs chests in a reliable manner outside Boreal Station with the build Dwarven Stability in slot 1 and Dash in slot 2.
It's more of a demonstration and proof of concept than a good bot really, so if you're planning to use it, rewrite it first to enhance its capabilities as it can be heavily improved.



# GWCA Backbreaker Bot.au3 #
A very fun bot that you can turn on and watch for good laughs. It will play a Backbreaker sin bar (look in the script for the skills) with different qualities. Remember to have a spear set, hammer set and dagger set appropriate to what the script wants. If you're not stupid, you'll notice that you can actually decide which weaponset will function as what, by simply changing a few values at the top of the script, but lets not dwell at that!