This page contains all LUA commands for the GWCA.

# General #

Before starting with the commands let me mention some important things about interpretation of LUA files.

**1st**

A big problem of LUA is that it doesn't check the parameters you give the Function. This means that LUA doesn't see a diffrence between IdentifyItem(x), IdentifyItem(x,y) and IdentifyItem(x,y,z). While the first example would propably lead to a crash the other ones will work, though IdentifyItem only takes 2 Parameters. Cause of this i implemented a check, that checks if u called the function with enough arguments and prints an error message if not.

**2nd**

Some functions like GetAgentPosition() return more than one value. U can access this values through LUA by simply adding more Variables that get the return.

eg:

`pos[x]`,`pos[y]` = GetAgentPosition()


**3d**

LUA doesnt care about types but i will mention them in the function listing ;) its also not possible to set standard values so i mention them only so u know what to write there.


# Commands #

This are the Commands (unsorted :P):

  * void MovePlayer(float nX, float nY, float nZ = 0);
  * void SendChat(char channel, char message);
  * void SendChat(char channel, wchar\_t message);
  * void MoveMap(dword MapID, dword Region = 2, dword District = 0, dword Language = 0);
  * void ChangeDistrict(dword Region = 2, dword Language = 0);
  * void UseSkill(dword SkillID, dword TargetID, dword Event = 0);
  * void ChangeWeaponSet(dword WeaponSet);
  * void PickUpItem(long TargetID);
  * void UseItem(dword ItemID);
  * void EquipItem(dword ItemID);
  * void DropItem(dword ItemID, dword Amount = 1);
  * void DropGold(dword Amount);
  * void AttackTarget(long TargetID);
  * void CommandHero(long id, float nX, float nY);
  * void CommandAll(float nX, float nY);
  * void GetQuest(byte id);
  * void QuestReward(byte id);
  * void Dialog(long pick);
  * void GoNPC(long id);
  * void DonateFaction();
  * void ExchangeAmber();
  * void OpenChest();
  * void GoSignpost(long id);
  * void EnterChallenge();
  * void GoPlayer(long id);
  * void UseAttackSkill(dword SkillID, dword TargetID, dword Event = 0);
  * void CancelAction();
  * void IdentifyItem(long idKit, long itemId);
  * void ChangeGold(long goldSelf, long goldStorage);
  * void MoveItem(long itemId, long bagId, long slot);
  * void ReturnToOutpost();
  * void AcceptAllItems(long bagId);
  * void AddHero(long HeroId);
  * void KickHero(long HeroId);
  * void SwitchMode(long NMHM = 1);
  * void AddNpc(long NpcId);
  * void KickNpc(long NpcId);
  * void TravelGH();
  * void LeaveGH();
  * 
---

  * long GetNearestAgentToAgent(long agentId);
  * long GetNearestEnemyToAgent(long agentId);
  * long GetNearestEnemyToAgentByAllegiance(long agentId);
  * long GetNearestAliveEnemyToAgent(long agentId);
  * long GetNearestItemToAgent(long agentId);
  * long GetNearestSignpostToAgent(long agentId);
  * long GetNearestNpcToAgentByAllegiance(long agentId);
  * float GetDistanceFromAgentToAgent(long agent1, long agent2);
  * float GetAngleFromAgentToAgent(long agent1, long agent2);
  * long GetFirstAgentByPlayerNumber(word playerNum);
  * long GetNearestAgentByPlayerNumber(word playerNum);
  * long GetNearestAgentToCoords(float x, float y);
  * long GetNearestNPCToCoords(float x, float y);
  * int IsAttackedMelee(long agentId);
  * 
---

  * void SellItem(long itemId);
  * void BuyItem(long id, long quantity, long value);
  * void WriteWhisper(const wchar\_t chatMsg, const wchar\_t chatName);
  * wchar\_t GetAgentName(int agentId);
  * void TargetNearestFoe();
  * void TargetNearestAlly();
  * void TargetNearestItem();
  * void TargetCalledTarget();
  * void UseHeroSkill(long HeroId, long SkillNumber, long Target);
  * bool CompareCharName(wchar\_t cmpName);
  * void ChangeMaxZoom(float fZoom);
  * 
---

  * bool IsCasting()
  * long SkillRecharge(int ID)
  * long SkillAdrenaline(int ID)
  * int GetSkillbarSkillId(int slot)
  * float GetAgentHP(int id)
  * float GetAgentEnergy(int id)
  * void ChangeTarget(int ID)
  * int,int GetProfessions(int ID)
  * int GetPlayerNumberOfAgent(int ID)
  * float GetRotationOfAgent(int id)
  * float,float GetAgentPosition(int id)
  * int GetAgentTeamId(int agentid)
  * int GetAgentHPPips(int id)
  * byte GetAgentEffectBitmap(int id)
  * byte GetAgentHexBitmap(int id)
  * long GetAgentType(int id)
  * int GetAgentLevel(int id)
  * long GetAgentProperties(int id)
  * bool IsAgentAttacking(int id)
  * bool IsAgentKnockedDown(int id)
  * bool IsAgentMoving(int id)
  * bool IsAgentCasting(int id)
  * int GetAgentAllegiance(int id)
  * int GetBagSize(int bag)
  * int GetFirstIdKitId()
  * void IdentifieItemByIndex(int bag,int slot)
  * void IdentifieItemByID(int id)
  * void DepositGold(int amount)
  * void WithdrawGold(int amount)
  * void SellItemByIndex(int bag,int slot)
  * void BuyIDKit()
  * void BuySuperiorIDKit()
  * int,int GetItemId(int bag,int slot)
  * int,int GetItemInfo(int bag,int slot)
  * void UseItemByIndex(int bag,int slot)
  * int,wchar\_t GetItemLastModifier(int bag,int slot)
  * int GetCurrentTargetID()
  * int GetOwnID()
  * int GetMyMaxHP()
  * int GetMyMaxEnergy()
  * int GetMapLoading()
  * int GetPing()
  * int GetLoggedInt()
  * int GetDead()
  * int GetCurrentBalthFaction()
  * int GetCurrentMaxBalthFaction()
  * int GetCurrentKurzickFaction()
  * int GetCurrentKurzickMaxFaction()
  * int GetCurrentLuxonFaction()
  * int GetCurrentLuxonMaxFaction()
  * int GetTitleTreasure()
  * int GetTirleLucky()
  * int GetTitleUnlucky()
  * int GetTitleWisdom()
  * int GetMoneySelf()
  * int GetMoneyStorage()
  * void SetSkillbarSkill(int slot,int id)
  * void ChangeSecondClass(dword newclass)