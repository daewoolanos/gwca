This page contains documentation of all of the GWCA commands and how they are interacted with.

# How-to use #
First of all, to use GWCA you must inject the .Dll into Guild Wars. This can be done by using a Dll Injector application (search google).

Communication between GWCA and your script is performed using Named Pipes to make sure speeds are always at the best.

In AutoIt3, simply look at some of the scripts in the Scripts folder to get an idea of how it is done. Remember also to 'register' the necessary custom messages for your GUI using 'GUIRegisterMsg(identifier, functionname)'.

You can register up to 3 different messages with GWCA:

  * The SkillLogCallback and PartyCallback (0x4A): This uses the WM\_COPYDATA message to send a structure from GWCA which contains information about skills being cast. See the GWCA SkillLog or Interrupt Bot. The SkillLogCallback is also the PartyCallback, which returns lots of information about your party. See GWCA MiniParty View for this.

  * The SkillCancelCallback (0x501): Which is commonly used with the SkillLogCallback if you wish to track which skills are cancelled/interrupted aswell.

  * The SkillCompleteCallback (0x502): Used to check whenever a skill completes activating. Does not trigger on skills without activation nor attack skills.

For instruction on how to use these 'Callbacks' please have a look at some of the example scripts in Scripts folder.

Also note, that when using the GWCA functions in AutoIt3, remember to set the `$cbType` variable to the right type before you use a command. For example, when you use CA\_GETCOORDS, remember to set `$cbType` to "float", so that the values are automatically un-packed. If you - for some reason - wants to get a float value but `$cbType = "int"` then you may use the function `_IntToFloat()`. Use `_FloatToInt()` to do it the other way around.
Return values are always in the `$cbVar` array. `$cbVar[0]` = first value, `$cbVar[1]` = last value, but you may also specify a variable that receives the contents of `$cbVar` - just take note that it's an array!


And last, but still very important:
When using commands that require an id (of an agent), specifying -1 will automatically use your current target, and -2 will automatically use your self.


# List of Commands #
The commands follow a distinct identifier pattern because they are a custom enumeration. To use them in AutoIt3, this enumeration has been pre-declared in 'GWCAConstants.au3' so you don't have to remember all the numbers though.

**General commands:**

| **Command** | **Description** |
|:------------|:----------------|
| CA\_GetCurrentTarget | No parameters, returns an integer with the id of your current target. |
| CA\_GetMyId | No parameters, returns an integer with the id of your own agent. |
| CA\_Casting | No parameters, returns a boolean which tells whether or not you're using a skill. |
| CA\_SkillRecharge | Parameter = skillslot, returns an integer which is either a timestamp if the skill is recharging, or 0 if it's recharged. |
| CA\_SkillAdrenaline | Parameter = skillslot, returns an integer telling how many points of adrenaline the specified skill has. |
| CA\_SetLogAndHwnd | Parameter = boolean to enable skill log, parameter 2 = hwnd of your script's window. |
| CA\_GetAgentAndTargetPtr | No parameters, used for debugging purposes. |
| CA\_GetSkillbarSkillId | Parameter = skillslot, obsolete. |
| CA\_GetMyMaxHP | No parameters, returns an integer with your max hp and an integer with your current hp. Optional parameter, supply the agent id of a hero to get the information on that instead. |
| CA\_GetMyMaxEnergy | No parameters, returns an integer with your max energy and an integer with your current energy. Optional parameter, supply the agent id of a hero to get the information on that instead. |
| CA\_GetBuildNumber | No parameters, returns an integer with the current build number of the GW client you're running. |
| CA\_ChangeMaxZoom | Parameter = integer with zoom value, changes your max zoom. Default is 750, default for observer mode is 1400. |
| CA\_GetLastDialogId | No parameters. Returns an integer with the id of the latest dialog option pressed. To be used with CA\_Dialog. |
| CA\_SetEngineHook | Parameter = boolean whether to turn on or off. Disables rendering of graphics completely lowering CPU usage to 0-1%. Big thanks to ACB for this! |
| CA\_GetHeroCasting | Parameter = hero index. Returns the Casting variable of specified hero, just like CA\_Casting for yourself. |
| CA\_GetHeroSkillRecharge | Parameter = hero index, parameter 2 = skill slot. Returns a timestamp of when the skill finishes recharging or 0 if recharged. Use HeroRechargeTimeLeft() to cope easier. |
| CA\_GetHeroSkillAdrenaline | Parameter = hero index, parameter 2 = skill slot. Returns the current amount of adrenaline points (not strikes) of the specified skill. |
| CA\_GetHeroSkillId | Parameter = hero index, parameter 2 = skill slot. Returns the skill id of the specified skill. |
| CA\_GetHeroAgentId | Parameter = hero index. Returns the agent id of the specified hero. |


---


**Action-related commands:**

| **Command** | **Description** |
|:------------|:----------------|
| CA\_Attack  | Parameter = id of target, attacks the specified target with weapon. |
| CA\_Move    | Parameters = X and Y coords of location to move to, remember to pack the float values (or simply use MoveEx()) when using this command. |
| CA\_MoveOld | Same parameters as CA\_Move. This command uses the old mechanics of Move - simply sending a CLICK\_LOCATION packet. |
| CA\_UseSkill | Parameter = skill slot, parameter 2 = target id. Works on all skills including attack skills. Remember that some skills that target yourself may require that specified (if so, use -2 as target id). Current target = -1. |
| CA\_ChangeWeaponSet | Parameter = weapon slot, changes weapon set. |
| CA\_ZoneMap | Parameter = zone id, optional parameter 2 = district number (use 0 for active district), zones to the outpost/city specified and if district number is provided also attempts to enter that district. Always uses current region and language. |
| CA\_DropGold | Parameter = amount of gold, drops the gold on the ground. |
| CA\_GoNpc   | Parameter = id of NPC, goes to specified NPC. |
| CA\_GoPlayer | Parameter = id of player, goes to specified player (for use in outposts/cities). |
| CA\_GoSignpost | Parameter = id of signpost/chest, goes to specified signpost/chest (yellow name). |
| CA\_EnterChallenge | No parameters, enters the challenge if you're in a Challenge Mission outpost. |
| CA\_EnterChallengeForeign | No parameters, used after CA\_EnterChallenge, it allows you to skip past the foreign character window. |
| CA\_OpenChest | No parameters, opens the nearest chest if you're within 'close range'. |
| CA\_SetEventSkillMode | Obsolete, don't use it. |
| CA\_PickupItem | Parameter = agent id of item, picks up the specified item. |
| CA\_Dialog  | Parameter = dialog option id, experimental function, requires knowledge about the NPC's dialog options. Can be found by using CA\_GetLastDialogId |
| CA\_ChangeTarget | Parameter = id of target, changes target. |
| CA\_TargetNearestFoe | No parameters, targets the nearest foe. |
| CA\_TargetNearestAlly | No parameters, targets the nearest ally. |
| CA\_TargetNearestItem | No parameters, targets the nearest item/signpost/chest. |
| CA\_TargetCalledTarget | No parameters, targets the currently called target of the party. |
| CA\_UseHero1Skill | Parameter = skillslot, parameter 2 = id of target. |
| CA\_UseHero2Skill | Parameter = skillslot, parameter 2 = id of target. |
| CA\_UseHero3Skill | Parameter = skillslot, parameter 2 = id of target. |
| CA\_CancelAction | No parameters, cancels your current action. Just like pressing ESC. |
| CA\_GetName | Parameter = id of target, returns a pointer to a wchar\_t[.md](.md) with the name of the player (can only be used for players), which can be ReadProcessMemory()'ed. |
| CA\_CommandHero1 | Parameters = X and Y floats packed of course. To unflag, set X and Y to $FLAG\_RESET like this: Cmd($CA\_CommandHero1, $FLAG\_RESET, $FLAG\_RESET)|
| CA\_CommandHero2 | Same as CA\_COMMANDHERO1. |
| CA\_CommandHero3 | Same as CA\_COMMANDHERO1. |
| CA\_CommandAll | Same as CA\_COMMANDHERO1. |
| CA\_ChangeDistrict | Parameter = region (-2 = international, 0 = america, 1 = asia korean, 2 = europe, 3 = asia chinese, 4 = asia japanese), parameter 2 = language (experiment around with this!). |
| CA\_Resign  | No parameters.  |
| CA\_UpdateAgentPosition | Parameter = agent id (or -1/-2). Abuses the fact that the client actually knows where you are, even though it may not display the correct position. |
| CA\_ReturnToOutpost | No parameters. "Presses" the Return To Outpost button after resigning. |
| CA\_GoAgent | Parameter = id of agent. Goes to the specified target. Same as follow. Will not open NPC dialog or attack enemy. |
| CA\_DonateFaction | Parameter = 0 for Kurzick, 1 for Luxon. Donates 5000 faction towards your faction. Not recently tested, but it should require open dialog with Faction Npc. |
| CA\_SetSkillbarSkill | Parameter = skill slot, parameter 2 = skill id. |
| CA\_ChangeSecondProfession | Parameter = profession number. Follows this enumeration 0-10 = { None,W,R,Mo,N,Me,E,A,Rt,P,D }. |
| CA\_TargetNextPartyMember | No parameters. Functions like the GW Control: Targetting - Party Member: Next |
| CA\_TargetNextFoe | No parameters. Functions like the GW Control: Targetting - Next Foe |
| CA\_SkipCinematic | No parameters. Skips the current cinematic/cutscene. |
| CA\_DismissBuff | Parameter = Hero (from 0 = self to 3 = Hero3), parameter 2 = index of the buff in order it was casted. |
| CA\_SendChat | Parameter = pointer to text. Use SendChat() in GWCAConstants.au3 as wrapper function. |
| CA\_WriteWhisper | Parameter = pointer to message, parameter 2 = pointer to sender. Use wrapper function WriteWhisper() in GWCAConstants.au3 |
| CA\_OpenStorage | No parameters. Experimental function which visually opens your storage. Note: only works after opening your storage once! Else GW will crash! |
| CA\_PrepareZoneMapEx | Internal command. Please use ZoneMapEx() function instead of this. |
| CA\_ZoneMapEx | Internal command. Requires previous call to CA\_PrepareZoneMapEx, so please use ZoneMapEx() function instead of this. |


---


**Specific memory commands:**

| **Command** | **Description** |
|:------------|:----------------|
| CA\_GetMapLoading | No parameters, returns an integer which is 0, 1 or 2. |
| CA\_GetMapId | No parameters, returns the id of the current map/zone. |
| CA\_GetPing | No parameters, returns the current ping displayed on the Performance Monitor in GW. |
| CA\_GetLoggedIn | No parameters, returns boolean. |
| CA\_GetDead | No parameters, returns boolean, but kinda obsolete though. |
| CA\_GetBalthFaction | No parameters, returns integer of current balthazar faction and integer of maximum balthazar faction. |
| CA\_GetKurzFaction | No parameters, returns integer of current kurzick faction and integer of maximum kurzick faction. |
| CA\_GetLuxonFaction | No parameters, returns integer of current luxon faction and integer of maximum luxon faction. |
| CA\_GetTitleTreasure | No parameters, returns integer of current treasure title. |
| CA\_GetTitleLucky | No parameters, returns integer of current lucky title. |
| CA\_GetTitlyUnlucky | No parameters, returns integer of current unlucky title. |
| CA\_GetTitleWisdom | No parameters, returns integer of current wisdom title. |
| CA\_GetTitleGamer | No parameters, returns integer of current gamer title. |
| CA\_GetExperience | No parameters, returns integer of your current experience points. |
| CA\_GetConnection | No parameters, returns integer of your connection status. 2 = logged into character, 1/0 = not logged into character. |
| CA\_GetMatchStatus | No parameters, returns integer of the match status. For RA, when in a match, this value is 1 before it starts, 2 when started, and 3 when the match is over (check if you're dead at this point to see if you won the match). Values may vary in other PvP types! |


---


**Agent info commands:**

| **Command** | **Description** |
|:------------|:----------------|
| CA\_GetAgentExist | Parameter = id of agent/target, returns boolean. |
| CA\_GetProfessions | Parameter = id of agent, returns integer of primary and integer of secondary profession. |
| CA\_GetPlayerNumber | Parameter = id of agent, returns integer of player number or, if the agent is an NPC/monster, the 'Model number'. |
| CA\_GetHP   | Parameter = id of agent, returns float between 0 and 1 with the health in percent. |
| CA\_GetRotation | Parameter = id of agent, returns float of rotation in degrees. |
| CA\_GetSkill | Parameter = id of agent, returns integer of current skill being used by agent. |
| CA\_GetCoords | Parameter = id of agent, returns float of X and float of Y coordinate, packed of course. |
| CA\_GetWeaponSpeeds | Parameter = id of agent, returns floats of the weapon speeds (figure them out yourself if you really need them). |
| CA\_GetSpiritRange | Obsolete and unfunctional. |
| CA\_GetTeamId | Parameter = id of agent, returns integer of team id. 1 = blue, 2 = red, 3 = yellow and so on. |
| CA\_GetCombatMode | Obsolete, don't use it. |
| CA\_GetModelMode | Also obsolete.  |
| CA\_GetHpPips | Parameter = id of agent, returns integer of how many pips the target has. |
| CA\_GetEffects | Parameter = id of agent, returns bitmap with the information that displays the different effects that you see on the health bar of the target, like enchanted, poisoned, deep wound etc. |
| CA\_GetHex  | Parameter = id of agent, returns integer. If 0, target is not hexed. If 1, target is hexed without degen and if 2, target is hexed with degen. |
| CA\_GetModelAnimation | Parameter = id of agent, returns integer. The model animation number depends on profession and sex, but can be used to determine emotes, etc. |
| CA\_GetEnergy | Obsolete, as it only works for yourself. Just use CA\_GetMyMaxEnergy. |
| CA\_GetAgentPtr | Parameter = id of agent, returns pointer to the agent structure of target in GW memory. |
| CA\_GetAgentMovementPtr | Parameter = id of agent, returns pointer to the agent movement structure of target in GW memory. |
| CA\_GetType | Parameter = id of agent, returns integer. 0xDB = players/npc's/monsters (living stuff), 0x200 = signposts/chests/solid objects, 0x400 = items that can be picked up. |
| CA\_GetLevel | Parameter = id of agent, returns integer with the level. |
| CA\_GetNameProperties | Obsolete.       |
| CA\_GetMaxId | No parameters, returns integer with the size of the agent array divided by 4 (because the agent array consists of Agent pointers (pointer = 4 bytes)). |
| CA\_GetMyNearestAgent | No parameters, returns id of your nearest agent. |
| CA\_GetMyDistanceToAgent | Parameter = id of agent, returns float with distance. |
| CA\_GetNearestAgentToAgent | Parameter = id of agent, returns id of the nearest agent to the specified agent. |
| CA\_GetDistanceFromAgentToAgent | Parameter = id of agent, parameter 2 = id of other agent. Returns the distance between the two agents as a float. |
| CA\_GetNearestAgentToAgentEx | Parameter = id of agent, same as CA\_GetNearestAgentToAgent, but also returns float with distance between them. |
| CA\_GetModelState | Kinda obsolete. |
| CA\_GetIsAttacking | Parameter = id of agent, returns boolean. |
| CA\_GetIsKnockedDown | Parameter = id of agent, returns boolean. |
| CA\_GetIsMoving | Same as the other 'Is' commands, but this one can even tell the truth through rubberbanding and random stuck! |
| CA\_GetIsDead | Same as the other 'Is' commands. |
| CA\_GetIsCasting | Obsolete when there's CA\_GetSkill, but whatever. |
| CA\_GetFirstAgentByPlayerNumber | Parameter = player/model number, returns the first (id-wise) agent with that number. |
| CA\_GetAllegiance | Parameter = id of agent, returns integer with the allegiance of agent. 0x100 = ally/non-attackable, 0x300 = enemy, 0x400 = spirit/pet, 0x500 = minion, 0x600 = npc/minipet. |
| CA\_GetNearestEnemyToAgentEx | Parameter = id of agent, returns id of nearest enemy to agent by team id (so, only for PvP) and float of distance. |
| CA\_GetIsAttackedMelee | Parameter = id of agent, returns integer with estimate of whether agent is being attacked by melee. Kinda obsolete function. |
| CA\_GetNearestItemToAgentEx | Parameter = id of agent, returns agent id of item and distance as float. |
| CA\_GetNearestAgentByPlayerNumber | Same as CA\_GetFirstAgentByPlayerNumberexcept it returns the nearest agent to you. |
| CA\_GetSpeed | Parameter = id of agent, returns float with current speed of agent (how far the agent moves per second in GW units). |
| CA\_GetNearestEnemyToAgentByAllegiance | Parameter = id of agent, returns id of enemy and distance as float. Only works for yourself or your allies. |
| CA\_GetNearestAliveEnemyToAgent | Parameter = id of agent, returns id of enemy and distance as float. Only works for yourself or your allies. |
| CA\_GetWeaponType | Parameter = id of agent, returns integer with weapon type. 1=bow, 2=axe, 3=hammer, 4=daggers, 5=scythe, 6=spear, 7=sword, 10=wand. |
| CA\_GetNearestSignpostToAgent | Parameter = id of agent, returns id of the nearest signpost/chest. |
| CA\_GetNearestNpcToAgentByAllegiance | Parameter = id of agent, returns id of the nearest NPC. |
| CA\_GetNearestAgentToCoords | Parameters = x and y coordinates packed. Returns the nearest agent to the specified coords. |
| CA\_GetNearestNpcToCoords | Parameters = x and y coordinates packed. Returns the nearest NPC to the specified coords. |
| CA\_GetLoginNumber | Parameter = id of agent. Returns the unique login number in instance of player. If used on non-player it will return 0. |
| CA\_GetNumberOfAgentsByPlayerNumber | Parameter = player/model number. Returns the number of alive agents within compass range with specified id. |
| CA\_GetNumberOfAliveEnemyAgents | No parameters. Returns the number of alive and visible enemy agents in total. |
| CA\_GetNextItem | Parameter = starting id. Returns the agent id of the next item. For looping through all items, use this command with the returned agent id as parameter after taking care of that item. |
| CA\_GetTarget | Parameter = agent id. Returns the currently logged target of the specified agent. |
| CA\_SetAttribute | Parameter = attribute id, parameter 2 = value. Remember to use CA\_ResetAttributes before starting to set attributes in scripts! See the $ATTRIB_constants in GWCAConstants.au3 or http://wiki.guildwars.com/wiki/Template_format for attribute id's._|
| CA\_ResetAttributes | No parameters. Resets the saved attributes, does NOT set your attributes to 0! To be used before a series of CA\_SetAttribute. |
| CA\_PlayerHasBuff | Parameter = Skill Id. Returns boolean. |
| CA\_Hero1HasBuff | Parameter = Skill Id. Returns boolean. |
| CA\_Hero2HasBuff | Parameter = Skill Id. Returns boolean. |
| CA\_Hero3HasBuff | Parameter = Skill Id. Returns boolean. |
| CA\_GetAgentDanger | Parameter = agent id (or -1/-2). Returns the number of foes to the agent which are currently targetting it. |
| CA\_GetTypeMap | Parameter = agent id (or -1/-2). Returns the bitmap integer used to determine things such as boss (0xC00), spirit (0x40000) and player (0x400000). Remember to use BitAND() like with CA\_GetEffects. |
| CA\_GetAgentWeapons | Parameter = agent id (or -1/-2). Returns the item id('s) of the agent's weapon(s). Can be used to acquire a bit more detailed info about weapons than CA\_GetWeaponType. |
| CA\_GetNextAgent | Parameter = starting id. Returns the agent id of the next valid agent. For looping through all agents, use this command with the returned agent id again. |
| CA\_GetNextAlly | Parameter = starting id. Returns the agent id of the next valid ally. For looping through all allies, use this command with the returned agent id again. |
| CA\_GetNextFoe | Parameter = starting id. Returns the agent id of the next valid foe. For looping through all foes, use this command with the returned agent id again. |
| CA\_GetExtraType | Paramter = agent id (or -1/-2). Returns the extra type of the agent as integer. This number can be used for distinguishing between chests and signposts for example. |
| CA\_PrepareNearestPlayerNumberToCoords | Parameter = player/model number. Sets up the CA\_GetNearestPlayerNumberToCoords command. But please use the wrapper function GetNearestPlayerNumberToCoords() in GWCAConstants.au3 instead! |
| CA\_GetNearestPlayerNumberToCoords | Parameters = coordinate set in packed floats. Requires previous call to CA\_PrepareNearestPlayerNumberToCoords. But please use the wrapper function GetNearestPlayerNumberToCoords() in GWCAConstants.au3 instead! |
| CA\_GetFirstAgentByPlayerNumberByTeam | Parameter = player/model number, parameter 2 = team id. Returns the first agent (iteration-wise) with the specified player/model number and team id making you able to easily identify your Ghostly Hero etc. in PvP. |
| CA\_GetNearestAliveEnemyToCoords | Parameters = coordinate set in packed floats. Returns the nearest alive enemy to the specified coordinates and the distance to the enemy. |
| CA\_GetNextAliveFoe | Parameter = starting id. Returns the agent id of the next valid alive foe. For looping through all alive foes, use this command with the returned agent id again. |
| CA\_PrepareNearestAliveEnemyToCoordsByPlayerNumber | Internal command used in GetNearestAliveEnemyToCoordsByPlayerNumber(). Please use this function instead! |
| CA\_GetNearestAliveEnemyToCoordsByPlayerNumber | Internal command. Requires previous call to CA\_PrepareNearestAliveEnemyToCoordsByPlayerNumber, but please use the GetNearestAliveEnemyToCoordsByPlayerNumber() function instead! |
| CA\_GetNearestAliveAgentByPlayerNumber | Parameter = player/model number. Returns the nearest alive agent with specified player/model number and your distance to it. |


---


**Item related commands:**

| **Command** | **Description** |
|:------------|:----------------|
| CA\_GetGold | No parameters. Returns integer with gold on your character and integer with gold in your storage. |
| CA\_GetBagSize | Parameter = bag index (see $BAG_declarations in GWCAConstants.au3). Returns the number of slots and the number of items currently in the bag._|
| CA\_SetBag  | Parameter = bag index. Sets the current bag to work with. |
| CA\_GetItemId | Parameter = item slot. Optional parameter = bag index (otherwise use CA\_SetBag). Returns item id and item model id. |
| CA\_GetIdKit | No parameters. Returns item id of the first ID kit it finds. If return is non-zero then you have an ID kit in your inventory. |
| CA\_IdentifyItem | Parameter = bag index, parameter 2 = item slot. |
| CA\_IdentifyItemById | Parameter = item id. If you've returned the item id from one of the prior commands you can use this command instead of CA\_IdentifyItem. |
| CA\_DepositGold | Parameter = amount of gold to deposit in storage. Specify -1 if you want to deposit the maximum amount. Can be used anywhere in the outpost. |
| CA\_WithdrawGold | Parameter = amount of gold to withdraw from storage. Specify -1 if you want to withdraw the maximum amount. Can be used anywhere in the outpost. |
| CA\_SellItem | Parameter = bag index, parameter 2 = item slot. Requires an open dialog with the Merchant of the outpost! |
| CA\_SellItemById | Parameter = item id. Requires an open dialog with the Merchant of the outpost! |
| CA\_BuyIdKit | No parameters. Requires an open dialog with the Merchant of the outpost! |
| CA\_BuySuperiorIdKit | No parameters. Requires an open dialog with the Merchant of the outpost! |
| CA\_PrepareMoveItem | For internal use. Use the wrapper functions MoveItem() and MoveItemById() instead. |
| CA\_MoveItem | For internal use. Use the wrapper functions MoveItem() and MoveItemById() instead. See the declaration of the bag id's in GWCAConstants.au3 |
| CA\_GetItemInfo | Parameter = item slot. Optional parameter = bag index (otherwise use CA\_SetBag). Returns integer with item rarity and integer with quantity. |
| CA\_UseItem | Parameter = bag index, parameter 2 = item slot. Uses the item specified. |
| CA\_UseItemById | Parameter = item id. Uses the item specified. |
| CA\_DropItem | Parameter = bag index, parameter 2 = item slot. Drops the specified item. If the quantity is above 1 then this command will drop it all. |
| CA\_DropItemById | Parameter = item id, parameter 2 = amount. Drops the specified item by the specified amount. Setting amount to -1 will drop it all. |
| CA\_AcceptAllItems | No parameters. Accepts all unclaimed items (from the unclaimed items window that may appear). Doesn't check if there isn't room for all the items, this simply accepts them! |
| CA\_GetItemLastModifier | Parameter = item slot. Optional parameter = bag index (otherwise use CA\_SetBag). Returns integer with the last modifier of the item and wchar\_t`*` with customize text. Last modifier can be used to check which dye (model id 146) it is. If customize text = 0 then item is not customized. |
| CA\_FindItemByModelId | Parameter = item model id. Searches for the first occurence of the specified model id and returns integer with the item id or 0 if item wasn't found. |
| CA\_FindEmptySlot | Parameter = bag index to start searching from. Returns integer with bag index and integer with item slot. This function also searches the storage. If any of the returns = 0, then no empty slots were found. |
| CA\_FindGoldItem | Optional parameter = bag index of last bag to search in. Searches from the backpack until (and including) the bag index supplied. If not set the function will search all bags including storage. The return is integer with item id of the gold item found and integer with the item's model id. If return value is 0 then no gold items were found. |
| CA\_GetItemPositionByItemId | Parameter = item id. Returns the position of the item; bag and slot-wise. If any of the returns are 0, the item was not found. |
| CA\_GetItemPositionByModelId | Parameter = item model id. Returns the position of the item; bag and slot-wise. If any of the returns are 0, the item was not found. |
| CA\_GetItemPositionByRarity | Parameter = rarity (use constants declared in GWCAConstants.au3). Returns the position of the item; bag and slot-wise. If any of the returns are 0, the item was not found. |
| CA\_GetItemModelIdById | Parameter = item id. Returns the model id of the specified item. |
| CA\_GetItemInfoById |Parameter = item id. Returns the rarity and quantity of the specified item. |
| CA\_GetItemLastModifierById | Parameter = item id. Returns the last modifier (see CA\_GetItemLastModifier) and customized text of the specified item. |
| CA\_EquipItem | Parameter = bag index, parameter 2 = item slot. Equips the specified item. |
| CA\_EquipItemById | Parameter = item id. Equips the specified item. |
| CA\_SalvageItem | Parameter = item id OR locate item by index with bag index,item slot. Searches for a salvage kit and uses it on the specified item. Currently disabled cause it doesnt work! |
| CA\_GetSalvageKit | No parameters. Searches for a salvage kit and returns the item id if it finds one. |
| CA\_BuyItem | Parameter = merchant item index (in the order you see at your merchant's window, starting from 1), parameter 2 = gold cost of the item. Buys the specified item if you used the right gold cost. |
| CA\_GetItemIdByAgent | Parameter = agent id (or -1/-2). Returns item id and model id of the specified agent, if it's an item. |
| CA\_GetItemInfoByAgent | Parameter = agent id (or -1/-2). Returns item rarity and quantity of the specified agent, if it's an item. |
| CA\_GetItemLastModifierByAgent | Parameter = agent id (or -1/-2). Returns the last modifier (see CA\_GetItemLastModifier) and customized wchar\_t**of the specified agent, if it's an item.**|
| CA\_GetNearestItemByModelId | Parameter = item model id. Finds the nearest item with the specified model id and returns the agent id and the distance between you and the item as a float. |
| CA\_GetItemExtraId | Parameter = item slot. Optional parameter = bag index (otherwise use CA\_SetBag). Returns the extra id of the item - used to determine dye color etc. |
| CA\_GetItemExtraIdById | Parameter = item id. Returns the extra id of the item - used to determine dye color etc. |
| CA\_GetItemExtraIdByAgent | Parameter = agent id (or -1/-2). Returns the extra id of the item - used to determine dye color etc. |
| CA\_GetItemReq | Parameter = item slot. Optional parameter = bag index (otherwise use CA\_SetBag). Returns the attribute requirement of the item. |
| CA\_GetItemReqById | Parameter = item id. Returns the attribute requirement of the item. |
| CA\_GetItemReqByAgent | Parameter = agent id (or -1/-2). Returns the attribute requirement of the item. |
| CA\_GetDyePositionByColor | Parameter = dye color id (see GWCAConstants.au3 $DYE_), optional parameter 2 = starting bag. Returns the position, if found, of the specified dye as bag,slot._|
| CA\_GetItemDmgMod | Parameter = item slot. Optional parameter = bag index (otherwise use CA\_SetBag). Returns the damage mod of the item. Only works for offhands/shields! |
| CA\_GetItemDmgModById | Parameter = item id. Returns the damage mod of the item. Only works for offhands/shields! |
| CA\_GetItemDmgModByAgent | Parameter = agent id (or -1/-2). Returns the damage mod of the item. Only works for offhands/shields! |


---


**Title related commands:**

| **Command** | **Description** |
|:------------|:----------------|
| CA\_GetTitleSunspear | No parameters. Returns integer with title progress. |
| CA\_GetTitleLightbringer | No parameters. Returns integer with title progress. |
| CA\_GetTitleVanguard | No parameters. Returns integer with title progress. |
| CA\_GetTitleNorn | No parameters. Returns integer with title progress. |
| CA\_GetTitleAsura | No parameters. Returns integer with title progress. |
| CA\_GetTitleDeldrimor | No parameters. Returns integer with title progress. |
| CA\_GetTitleNorthMastery | No parameters. Returns integer with title progress. |
| CA\_GetTitleDrunkard | No parameters. Returns integer with title progress. |
| CA\_GetTitleSweet | No parameters. Returns integer with title progress. |
| CA\_GetTitleParty | No parameters. Returns integer with title progress. |
| CA\_GetTitleCommander | No parameters. Returns integer with title progress. |
| CA\_GetTitleLuxon | No parameters. Returns integer with title progress. |
| CA\_GetTitleKurzick | No parameters. Returns integer with title progress. |


---


**More general commands:**

| **Command** | **Description** |
|:------------|:----------------|
| CA\_AddHero | Parameter = hero id. Adds hero to the party. |
| CA\_KickHero | Parameter = hero id. Kicks hero from the party. |
| CA\_SwitchMode | Parameter = boolean to set HM or NM. HM = 1, NM = 0. |
| CA\_AddNpc  | Parameter = henchman id according to Party Search list. Adds henchman to the party. |
| CA\_KickNpc | Parameter = henchman id. Kicks henchman from the party. |
| CA\_TravelGH | No parameters. Travels to your Guild Hall. |
| CA\_LeaveGH | No parameters. Leaves your Guild Hall. |
| CA\_InitMapLoad | No parameters. Inits the CA\_MapIsLoaded command. |
| CA\_MapIsLoaded | No parameters. Returns boolean. Use CA\_InitMapLoad before !!! |
| CA\_GetMapOverlayCoords | Parameter = map overlay id (get this with CA\_GetNearestMapOverlayToCoords). Returns the x,y coords of the specified map overlay. |
| CA\_GetMapOverlayInfo | Parameter = map overlay id. Returns the option variable (usually color) and the icon model id of the specified map overlay. |
| CA\_GetNearestMapOverlayToCoords | Parameters = x and y coordinates packed. Returns the nearest Map Overlay to the specified coords. |
| CA\_GetPartyInfo | Parameter = team id, parameter 2 = script hwnd. Advanced command. Uses a team size of 8 by default. Change this with CA\_SetTeamSize. See the definition of $tagPARTYINFO in GWCAConstants.au3 and the example script `GWCA MiniParty View.au3`. |
| CA\_ClearPacketQueue | No parameters. Advanced command to clean up packet queue if GW surprises you with a load screen (to avoid crashing). If used with CmdCB() it returns the number of packets removed from the queue. |
| CA\_SetHeroMode | Parameter = hero index (1,2,3), parameter 2 = hero mode (see $HEROMODE_variables). Changes the mode of the specified hero to the specified state. 0 = fight, 1 = guard, 2 = avoid._|
| CA\_QuestCheck | Parameter = quest id (or -1 for active quest). If the quest is in your Quest Log, it returns the quest id and the current log state. |
| CA\_QuestCoords | Parameter = quest id (or -1 for active quest). If the quest is in your Quest Log, it returns the X,Y coordinates of the marker. |
| CA\_QuestActive | No parameters. Returns the quest id of the currently active quest. |
| CA\_QuestAbandon | Parameter = quest id (or -1 for active quest). Abandons the specified quest. |
| CA\_SetTeamSize | Parameter = team size. For use with CA\_GetPartyInfo. Set the team size to be evaluated using the CA\_GetPartyInfo command. |
| CA\_AllocMem | Parameter = size. Returns pointer to memory. Example script: http://pastebin.com/f52bae4a0 - credits to Gabba for the function. |
| CA\_FreeMem | Parameter = 0, parameter 2 = pointer to memory. Frees allocated memory. |
| CA\_GetRegionAndLanguage | No parameters. Returns your current region and language. |
| CA\_TraderRequest | Parameter = item index. The item index is the number on the list at the trader. For the Rune trader experiment around. Use this to setup CA\_TraderBuy. |
| CA\_TraderCheck | No parameters. Returns the trader item id and it's cost that is currently active. Loop this until the item id or cost is non-zero to verify that the info has been received before calling CA\_TraderBuy. |
| CA\_TraderBuy | No parameters. Optionally returns whether the packet was sent or not as boolean. Requires a CA\_TraderRequest and waiting until item id and cost has been received before it works! |
| CA\_TraderRequestSell | Parameter = bag index, parameter 2 = item slot. Sends a Request Quote on the specified item (remember to use the right trader for the item). Wait using CA\_TraderCheck until the info is received. |
| CA\_TraderRequestSellById | Parameter = item id. Same as CA\_TraderRequestSell, but instead, simply takes an item id as input. |
| CA\_TraderSell | No parameters. Optionally returns whether the packet was sent or not as boolean. Requires a CA\_TraderRequestSell(ById) and waiting until item id and cost has been received before it works! |
| CA\_GetNumberOfFoesInRangeOfAgent | Parameter = agent id (or -1/-2), parameter 2 = radius as float. Returns the number of foes counted. |
| CA\_GetNumberOfAlliesInRangeOfAgent | Parameter = agent id (or -1/-2), parameter 2 = radius as float. Returns the number of allies counted. |
| CA\_GetNumberOfItemsInRangeOfAgent | Parameter = agent id (or -1/-2), parameter 2 = radius as float. Returns the number of items counted. |
| CA\_TradePlayer | Parameter = agent id (or -1/-2). Like pressing the "Trade" button next to a player's name. |
| CA\_GetMapBoundariesPtr | No parameters. Returns a pointer to the structure that defines the boundaries of the current map. If return is zero, the client hasn't fully loaded the zone. |
| CA\_GetTimeStamp | No parameters. Returns the current internal time stamp. Used in skill recharges, effects durations and other things. |
| CA\_LockHero | Parameter = hero index, parameter 2 = agent target (or -1/-2). Set to 0 to un-lock the target of the hero. |
| CA\_CancelMaintainedEnchantment | Parameter = skill id. Cancels the specified self-targetting or in other way targetting you maintained enchantment. |
| CA\_GetSkillType | Parameter = skill id. Returns the type of the specified skill. See the $SKILLTYPE_variables in GWCAConstants.au3 for use._|


---


**Player trade related commands**

| **Command** | **Description** |
|:------------|:----------------|
| CA\_SubmitOffer | Parameter = gold amount to offer. Like pressing the "Submit Offer" button, but also including the amount of gold offered. |
| CA\_ChangeOffer | No parameters. Like pressing the "Change Offer" button. |
| CA\_OfferItem | Parameter = bag index, parameter 2 = item slot OR parameter 1 = item id and parameter 2 = 0. Offers the specified item in the trade window. |
| CA\_CancelTrade | No parameters. Like pressing the "Cancel" button in a trade. |
| CA\_AcceptTrade | No parameters. Like pressing the "Accept" button in a trade. Can only be used after both players have submitted their offer. |


---


**Effect monitor related commands**
| **Command** | **Description** |
|:------------|:----------------|
| CA\_GetEffectCount | No parameters. Returns the number of effects currently on you. |
| CA\_GetEffect | Parameter = skill id. If the effect exists on you, the id of the effect is returned. |
| CA\_GetEffectByIndex | Parameter = index (0-based) of effect. Returns the skill id of the specified effect. |
| CA\_GetEffectDuration | Parameter = skill id. If the effect exists on you, the total duration and the current time left of the effect is returned as floats in milliseconds. |


---


**Equipment related commands**
| **Command** | **Description** |
|:------------|:----------------|
| CA\_GetEquipmentModelId | Parameter = agent id (or -1/-2), parameter 2 = equipment index. Returns the model id of the specified equipment piece. |
| CA\_GetEquipmentDyeInfo | Parameter = agent id (or -1/-2), parameter 2 = equipment index. Returns the dye id and shinyness of the specified equipment piece. |
| CA\_SetEquipmentAgent | Parameter = agent id (or -1/-2). Sets the agent to currently work with equipment commands. Does not need to be set for the Get commands - only the Set commands. |
| CA\_SetEquipmentModelId | Parameter = equipment index, parameter 2 = item model id. Sets the model id of the equipment piece and updates the agent. |
| CA\_SetEquipmentDye | Parameter = equipment index, parameter 2 = dye id. Sets the dye id of the equipment piece and updates the agent. |
| CA\_SetEquipmentShinyness | Parameter = equipment index, parameter 2 = shinyness. Sets the shinyness of the equipment piece and updates the agent. The shinyness is the rendering mask that is used alongside the dye id. |