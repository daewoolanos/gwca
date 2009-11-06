#include "Main.h"

/*	Guild Wars Client API - GWCA
	This is a large set of functions that makes interfacing with Guild Wars a lot easier.
	Uses Window Messages for communicating by hooking the WndProc of GW, allowing you
	to SendMessage directly to the Guild Wars window.
	Protocol rules are, if your Message expects a reply, then you have to provide the
	window handle of your application in lParam. The reply is sent with code 0x500. */

byte* BaseOffset = NULL;
byte* PacketSendFunction = NULL;
byte* CurrentTarget = NULL;
byte* AgentArrayPtr = NULL;
byte* AgentArrayMaxPtr = NULL;
byte* MessageHandlerStart = NULL;
byte* MessageHandlerReturn = NULL;
byte* SkillLogStart = NULL;
byte* SkillLogReturn = NULL;
byte* WriteWhisperStart = NULL;
byte* TargetFunctions = NULL;
byte* HeroSkillFunction = NULL;
byte* ClickToMoveFix = NULL;
byte* BuildNumber = NULL;
byte* ChangeTargetFunction = NULL;
byte* MaxZoomStill = NULL;
byte* MaxZoomMobile = NULL;
byte* SkillCancelStart = NULL;
byte* SkillCancelReturn = NULL;
byte* AgentNameFunction = NULL;
byte* SellSessionStart = NULL;
byte* SellSessionReturn = NULL;
byte* SellItemFunction = NULL;
byte* BuyItemFunction = NULL;
byte* PingLocation = NULL;
byte* LoggedInLocation = NULL;
byte* NameLocation = NULL;
byte* EmailLocation = NULL;
byte* DeadLocation = NULL;
byte* BasePointerLocation = NULL;
byte* MapIdLocation = NULL;
byte* DialogStart = NULL;
byte* DialogReturn = NULL;

dword FlagLocation = 0;
dword PacketLocation = 0;

AgentArray Agents;

bool LogSkills = false;
HWND ScriptHwnd = NULL;
wchar_t* pName;
long MoveItemId = NULL;
long TmpVariable = NULL;

long SellSessionId = NULL;
long LastDialogId = 0;

Skillbar* MySkillbar = NULL;
CSectionA* MySectionA = new CSectionA();
ItemManager* MyItemManager = new ItemManager();

unsigned int MsgUInt = NULL;
WPARAM MsgWParam = NULL;
LPARAM MsgLParam = NULL;
HWND MsgHwnd = NULL;
int MsgInt;
int MsgInt2;
int MsgEvent = 0;
float MsgFloat;
float MsgFloat2;

HANDLE PacketMutex;
std::vector<CPacket*> PacketQueue;
std::vector<SkillLogSkill> SkillLogQueue;
std::vector<SkillLogSkill> SkillCancelQueue;

void _declspec(naked) SkillLogHook(){
	SkillLogSkill* skillPtr;

	_asm {
		POP EDI
		MOV skillPtr,EDI
		MOV EAX,DWORD PTR DS:[ESI+0x10]
		INC EAX
		MOV DWORD PTR DS:[ESI+0x10],EAX
	}

	SkillLogQueue.push_back(*skillPtr);

	_asm {
		JMP SkillLogReturn
	}
}

void _declspec(naked) SkillCancelHook(){
	SkillLogSkill* cancelSkillPtr;

	_asm {
		MOV ESI,ECX
		MOV EAX,DWORD PTR DS:[EDI]
		MOV ECX,DWORD PTR DS:[ESI+0x4]
		PUSHAD
		MOV cancelSkillPtr,EDI
	}

	SkillCancelQueue.push_back(*cancelSkillPtr);

	_asm {
		POPAD
		JMP SkillCancelReturn
	}
}

void _declspec(naked) SellSessionHook(){
	_asm {
		PUSH ESI
		MOV ESI,ECX
		PUSH EDI

		MOV EDX,DWORD PTR DS:[ESI+4]
		MOV SellSessionId,EDX

		MOV EDX,2

		JMP SellSessionReturn
	}
}

void _declspec(naked) DialogHook(){
	_asm {
		PUSH EBP
		MOV EBP,ESP
		
		MOV EAX,DWORD PTR SS:[EBP+8]
		MOV LastDialogId,EAX

		MOV EAX,DWORD PTR DS:[ECX+8]
		TEST AL,1
		JMP DialogReturn
	}
}

void _declspec(naked) CustomMsgHandler(){
	_asm {
		MOV EAX,DWORD PTR DS:[EBP+0x8]
		MOV MsgHwnd,EAX
		MOV EAX,DWORD PTR DS:[EBP+0xC]
		MOV MsgUInt,EAX
		MOV EAX,DWORD PTR DS:[EBP+0x10]
		MOV MsgWParam,EAX
		MOV EAX,DWORD PTR DS:[EBP+0x14]
		MOV MsgLParam,EAX
	}

	if(MsgUInt < 0x400){
		_asm {
			SUB ESP,0x20
			PUSH EBX
			PUSH ESI
			PUSH EDI
			MOV EDI,DWORD PTR SS:[EBP+8]
			JMP MessageHandlerReturn
		}
	}

	switch(MsgUInt){
		//Stuff related to you
		case 0x401: //Current Target : No return
			PostMessage((HWND)MsgLParam, 0x500, *(long*)CurrentTarget, 0);
			break;
		case 0x402: //Get your own agent ID : Return int
			PostMessage((HWND)MsgLParam, 0x500, myId, 0);
			break;
		case 0x403: //Check if you're casting : Return int/bool
			ReloadSkillbar();
			if(MySkillbar==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, MySkillbar->Casting, 0);
			break;
		case 0x404: //Check if skill is recharging : Return int/long
			ReloadSkillbar();
			if(MySkillbar==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, MySkillbar->Skill[MsgWParam-1].Recharge, 0);
			break;
		case 0x405: //Check adrenaline points of a skill : Return int/long
			ReloadSkillbar();
			if(MySkillbar==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, MySkillbar->Skill[MsgWParam-1].Adrenaline, 0);
		case 0x406: //Put stuff in Msg variables : No return
			switch(MsgWParam){
				case 1:
					MsgInt = MsgLParam;
					break;
				case 2:
					MsgInt2 = MsgLParam;
					break;
				case 3:
					memcpy(&MsgFloat, &MsgLParam, sizeof(int));
					break;
				case 4:
					memcpy(&MsgFloat2, &MsgLParam, sizeof(int));
					break;
			}
		case 0x407: //Set SkillLog and Script hWnd : No return
			LogSkills = (bool)MsgWParam;
			ScriptHwnd = (HWND)MsgLParam;
			break;
		case 0x408: //Get base Agent array pointer and Current target pointer : Return ptr & ptr
			PostMessage((HWND)MsgLParam, 0x500, (WPARAM)AgentArrayPtr, (LPARAM)CurrentTarget);
			break;
		case 0x409: //Get skill id of skills on your Skillbar : Return int/dword
			ReloadSkillbar();
			if(MySkillbar==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, MySkillbar->Skill[MsgWParam-1].Id, 0);
			break;
		case 0x40A: //Get your own max health (and current health): Return int/long & float
			if(Agents[myId]==NULL){RESPONSE_INVALID;}
			MsgInt = (int)(floor(Agents[myId]->MaxHP * Agents[myId]->HP));
			PostMessage((HWND)MsgLParam, 0x500, Agents[myId]->MaxHP, MsgInt);
			break;
		case 0x40B: //Get your own max energy (and current energy): Return int/long & float
			if(Agents[myId]==NULL){RESPONSE_INVALID;}
			MsgInt = (int)(floor(Agents[myId]->MaxEnergy * Agents[myId]->Energy));
			PostMessage((HWND)MsgLParam, 0x500, Agents[myId]->MaxEnergy, MsgInt);
			break;
		case 0x40C: //Get build number of GW : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, buildNumber, 0);
			break;
		case 0x40D: //Change max zoom of GW : No return
			MsgFloat = (float)MsgWParam;
			if(MsgFloat < 0 || MsgFloat > 10000){RESPONSE_INVALID;}
			ChangeMaxZoom(MsgFloat);
			break;
		case 0x40E: //Get last dialog id : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, LastDialogId, 0);
			break;
			
		//Packet Related Commands
		case 0x410: //Attack : No return
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			AttackTarget(MsgWParam);
			break;
		case 0x411: //Move to x, y : No return
			memcpy(&MsgFloat, &MsgWParam, sizeof(int));
			memcpy(&MsgFloat2, &MsgLParam, sizeof(int));
			MovePlayer(MsgFloat, MsgFloat2);
			break;
		case 0x412: //Use skill : No return
			if(MsgLParam == -1){MsgLParam = *(long*)CurrentTarget;}
			if(MsgLParam == -2){MsgLParam = myId;}
			UseSkill(MsgWParam, MsgLParam, MsgEvent);
			break;
		case 0x413: //Change weapon set : No return
			ChangeWeaponSet(MsgWParam-1);
			break;
		case 0x414: //Zone map : No return
			if(MsgLParam!=NULL)
				MoveMap(MsgWParam, 2, MsgLParam);
			else
				MoveMap(MsgWParam);
			break;
		case 0x415: //Drop gold : No return
			DropGold(MsgWParam);
			break;
		case 0x416: //Go to NPC : No return
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			GoNPC(MsgWParam);
			break;
		case 0x417: //Go to player : No return
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			GoPlayer(MsgWParam);
			break;
		case 0x418: //Go to signpost : No return
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			GoSignpost(MsgWParam);
			break;
		case 0x419: //Use attack skill : No return
			if(MsgLParam == -1){MsgLParam = *(long*)CurrentTarget;}
			UseAttackSkill(MsgWParam, MsgLParam, MsgEvent);
			break;
		case 0x41A: //Enter challenge mission : No return
			EnterChallenge();
			break;
		case 0x41B: //Open chest : No return
			OpenChest();
			break;
		case 0x41C: //Set event skill mode : No return
			MsgEvent = (int)MsgWParam;
			break;
		case 0x41D: //Use skillbar skill : No return
			ReloadSkillbar();
			if(MsgLParam == -1){MsgLParam = *(long*)CurrentTarget;}
			if(MsgLParam == -2){MsgLParam = myId;}
			if(MySkillbar==NULL){RESPONSE_INVALID;}
			UseSkill(MySkillbar->Skill[MsgWParam-1].Id, MsgLParam, MySkillbar->Skill[MsgWParam-1].Event);
			break;
		case 0x41E: //Pick up item : No return
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			PickUpItem(MsgWParam);
			break;
		case 0x41F: //Use skillbar attack skill : No return
			ReloadSkillbar();
			if(MsgLParam == -1){MsgLParam = *(long*)CurrentTarget;}
			if(MySkillbar==NULL){RESPONSE_INVALID;}
			UseAttackSkill(MySkillbar->Skill[MsgWParam-1].Id, MsgLParam, MySkillbar->Skill[MsgWParam-1].Event);
			break;
		case 0x420: //Dialog packet : No return
			Dialog(MsgWParam);
			break;
		case 0x421: //Change target : No return
			_asm MOV ECX,MsgWParam
			_asm MOV EDX,0
			_asm CALL ChangeTargetFunction
			break;
		case 0x422: //Write status about Bot (and/or Lock) : No return (wparam=bot, lparam=lock, 0=nothing, 1=on, 2=off)
			switch(MsgWParam){
				case 1:
					WriteWhisper(L"Interrupts turned on", L"GWCA");
					break;
				case 2:
					WriteWhisper(L"Interrupts turned off", L"GWCA");
					break;
			}
			switch(MsgLParam){
				case 1:
					WriteWhisper(L"Locked on target", L"Lock On");
					break;
				case 2:
					WriteWhisper(L"No longer locked on target", L"Lock Off");
					break;
			}
			break;
		case 0x423: //Target nearest foe : No return
			TargetNearestFoe();
			break;
		case 0x424: //Target nearest ally : No return
			TargetNearestAlly();
			break;
		case 0x425: //Target nearest item : No return
			TargetNearestItem();
			break;
		case 0x426: //Write status about Bot Delay : No return
			switch(MsgWParam){
				case 0:
					WriteWhisper(L"Interrupt delay is off", L"Delay Off");
					break;
				case 1:
					WriteWhisper(L"Interrupt delay is on", L"Delay On");
					break;
			}
			break;
		case 0x427: //Target called target : No return
			TargetCalledTarget();
			break;
		case 0x428: //Use hero 1 skill : No return
			if(MsgLParam == -1){MsgLParam = *(long*)CurrentTarget;}
			if(MsgLParam == -2){MsgLParam = myId;}
			if(Agents[MsgLParam]==NULL){RESPONSE_INVALID;}
			UseHero1Skill(MsgWParam-1, MsgLParam);
			break;
		case 0x429: //Use hero 2 skill : No return
			if(MsgLParam == -1){MsgLParam = *(long*)CurrentTarget;}
			if(MsgLParam == -2){MsgLParam = myId;}
			if(Agents[MsgLParam]==NULL){RESPONSE_INVALID;}
			UseHero2Skill(MsgWParam-1, MsgLParam);
			break;
		case 0x42A: //Use hero 3 skill : No return
			if(MsgLParam == -1){MsgLParam = *(long*)CurrentTarget;}
			if(MsgLParam == -2){MsgLParam = myId;}
			if(Agents[MsgLParam]==NULL){RESPONSE_INVALID;}
			UseHero3Skill(MsgWParam-1, MsgLParam);
			break;
		case 0x42B: //Write status about Bot Miss : No return
			switch(MsgWParam){
				case 0:
					WriteWhisper(L"Random interrupt miss is off", L"Miss Off");
					break;
				case 1:
					WriteWhisper(L"Random interrupt miss is on", L"Miss On");
					break;
			}
			break;
		case 0x42C: //Cancel movement : No return
			CancelAction();
			break;
		case 0x42D: //Write status about current tab : No return
			switch(MsgWParam){
				case 1:
					WriteWhisper(L"Switched to tab 1", L"Tab 1");
					break;
				case 2:
					WriteWhisper(L"Switched to tab 2", L"Tab 2");
					break;
			}
			break;
		case 0x42E: //Get ptr to name of agent : Return wchar_t*
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			pName = GetAgentName(MsgWParam);
			PostMessage((HWND)MsgLParam, 0x500, (WPARAM)pName, 0);
			break;
		case 0x42F: //Command hero 1 to location : No return
			memcpy(&MsgFloat, &MsgWParam, sizeof(float));
			memcpy(&MsgFloat2, &MsgLParam, sizeof(float));
			CommandHero(0x1A, MsgFloat, MsgFloat2);
			break;
		case 0x430: //Command hero 2 to location : No return
			memcpy(&MsgFloat, &MsgWParam, sizeof(float));
			memcpy(&MsgFloat2, &MsgLParam, sizeof(float));
			CommandHero(0x1B, MsgFloat, MsgFloat2);
			break;
		case 0x431: //Command hero 3 to location : No return
			memcpy(&MsgFloat, &MsgWParam, sizeof(float));
			memcpy(&MsgFloat2, &MsgLParam, sizeof(float));
			CommandHero(0x1C, MsgFloat, MsgFloat2);
			break;
		case 0x432: //Command all to location : No return
			memcpy(&MsgFloat, &MsgWParam, sizeof(float));
			memcpy(&MsgFloat2, &MsgLParam, sizeof(float));
			CommandAll(MsgFloat, MsgFloat2);
			break;
		case 0x433: //Change region and language : No return
			ChangeDistrict(MsgWParam, MsgLParam);
			break;
		case 0x434: //Send /resign to chat, effectively resigning : No return
			SendChat('/',"resign");
			break;
		case 0x435: //Send "Return to Outpost" packet : No return
			ReturnToOutpost();
			break;

		//SectionA related commands
		case 0x440: //Check if map is loading : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, mapLoading, 0);
			break;
		case 0x441: //Get map id : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->MapId(), 0);
			break;
		case 0x442: //Get ping : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->Ping(), 0);
			break;
		case 0x443: //Check if logged in : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->LoggedIn(), 0);
			break;
		case 0x444: //Check if you're dead : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->Dead(), 0);
			break;
		case 0x445: //Get current and max balthazar faction : Return int/long & int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->CurrentBalthFaction(), MySectionA->MaxBalthFaction());
			break;
		case 0x446: //Get current and max kurzick faction : Return int/long & int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->CurrentKurzickFaction(), MySectionA->MaxKurzickFaction());
			break;
		case 0x447: //Get current and max luxon faction : Return int/long & int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->CurrentLuxonFaction(), MySectionA->MaxLuxonFaction());
			break;
		case 0x448: //Get current Treasure Title (credits to ddarek): Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleTreasure(), 0);
			break;
		case 0x449: //Get current Lucky Title (credits to ddarek): Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleLucky(), 0);
			break;
		case 0x44A: //Get current Unlucky Title (credits to ddarek): Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleUnlucky(), 0);
			break;
		case 0x44B: //Get current Wisdom Title (credits to ddarek): Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleWisdom(), 0);
			break;

		//Agent Related Commands
		case 0x450: //Check for agent existency : Return int/bool
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){
				PostMessage((HWND)MsgLParam, 0x500, 0, 0);
			}else{
				PostMessage((HWND)MsgLParam, 0x500, 1, 1);
			}
			break;
		case 0x451: //Get agent's primary and secondary profession : Return byte & byte
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->Primary, Agents[MsgWParam]->Secondary);
			break;
		case 0x452: //Get player number of agent : Return word
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->PlayerNumber, 0);
			break;
		case 0x453: //Get HP of agent : Return float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			memcpy(&MsgInt, &Agents[MsgWParam]->HP, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x454: //Get rotation of agent in degrees : Return float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgFloat = (float)(Agents[MsgWParam]->Rotation * 180 / 3.14159265358979323846);
			memcpy(&MsgInt, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x455: //Get agent's current skill : Return word
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->Skill, 0);
			break;
		case 0x456: //Get X,Y coords of agent : Return float & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			memcpy(&MsgInt, &Agents[MsgWParam]->X, sizeof(float));
			memcpy(&MsgInt2, &Agents[MsgWParam]->Y, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x457: //Get weapon speeds of agent (weapon attack speed, attack speed modifier) : Return float & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			memcpy(&MsgInt, &Agents[MsgWParam]->WeaponAttackSpeed, sizeof(float));
			memcpy(&MsgInt2, &Agents[MsgWParam]->AttackSpeedModifier, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x458: //Is agent in spirit range of me : Return int/long
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->InSpiritRange, 0);
			break;
		case 0x459: //Get team ID of agent (0 = none) : Return byte
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->TeamId, 0);
			break;
		case 0x45A: //Get agent's combat mode : Return byte
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->CombatMode, 0);
			break;
		case 0x45B: //Get agent's model mode : Return float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			memcpy(&MsgInt, &Agents[MsgWParam]->ModelMode, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x45C: //Get agent's health pips : Return int
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			//memcpy(&MsgInt, &Agents[MsgWParam]->HPPips, sizeof(float));
			MsgInt = ((Agents[MsgWParam]->HPPips / 0.0038) > 0.0) ? floor((Agents[MsgWParam]->HPPips / 0.0038) + 0.5) : ceil((Agents[MsgWParam]->HPPips / 0.0038) - 0.5);//floor((Agents[MsgWParam]->HPPips / 0.0038) + 0.5);
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x45D: //Get agent's effect bit map : Return byte
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->Effects, 0);
			break;
		case 0x45E: //Get agent's hex bit map : Return byte
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->Hex, 0);
			break;
		case 0x45F: //Get agent's model animation : Return int/dword
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->ModelAnimation, 0);
			break;
		case 0x460: //Get agent's energy - ONLY WORKS FOR YOURSELF! : Return float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			memcpy(&MsgInt, &Agents[MsgWParam]->Energy, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x461: //Get pointer to agent : Return ptr
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, (WPARAM)Agents[MsgWParam], 0);
			break;
		case 0x462: //Get agent type : Return int/long
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->Type, 0);
			break;
		case 0x463: //Get agent level : Return byte
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->Level, 0);
			break;
		case 0x464: //Get agent's name properties : Return int/long
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->NameProperties, 0);
			break;
		case 0x465: //Get max agent id : Return unsigned int/dword
			PostMessage((HWND)MsgLParam, 0x500, maxAgent, 0);
			break;
		case 0x466: //Get nearest agent to yourself : Return int/long
			MsgInt = GetNearestAgentToAgent(myId);
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x467: //Get distance between agent and you : Return float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgFloat = GetDistanceFromAgentToAgent(myId, MsgWParam);
			memcpy(&MsgInt, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x468: //Get nearest agent to agent : Return int/long
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = GetNearestAgentToAgent(MsgWParam);
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x469: //Get distance from agent (set previously in MsgInt by 0x406 : 1) to agent (MsgWParam) : Return float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			if(MsgInt < 1 || (unsigned int)MsgInt > maxAgent){RESPONSE_INVALID;}
			MsgFloat = GetDistanceFromAgentToAgent(MsgInt, MsgWParam);
			memcpy(&MsgInt, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x46A: //Get nearest agent to agent AND the distance between them : Return int/long & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = GetNearestAgentToAgent(MsgWParam);
			MsgFloat = GetDistanceFromAgentToAgent(MsgWParam, MsgInt);
			memcpy(&MsgInt2, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x46B: //Get model state of agent : Return int/long
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->ModelState, 0);
			break;
		case 0x46C: //Check if agent is attacking : Return int/bool
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			if(Agents[MsgWParam]->ModelState == 0x60||
				Agents[MsgWParam]->ModelState == 0x440||
				Agents[MsgWParam]->ModelState == 0x460)
			{
				PostMessage((HWND)MsgLParam, 0x500, 1, 1);
			}else{
				PostMessage((HWND)MsgLParam, 0x500, 0, 0);
			}
			break;
		case 0x46D: //Check if agent is knocked down : Return int/bool
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			if(Agents[MsgWParam]->ModelState == 0x450){
				PostMessage((HWND)MsgLParam, 0x500, 1, 1);
			}else{
				PostMessage((HWND)MsgLParam, 0x500, 0, 0);
			}
			break;
		case 0x46E: //Check if agent is moving : Return int/bool
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			if(Agents[MsgWParam]->ModelState == 0x0C||
				Agents[MsgWParam]->ModelState == 0x4C||
				Agents[MsgWParam]->ModelState == 0xCC)
			{
				PostMessage((HWND)MsgLParam, 0x500, 1, 1);
			}else{
				PostMessage((HWND)MsgLParam, 0x500, 0, 0);
			}
			break;
		case 0x46F: //Check if agent is dead : Return int/bool
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			if(Agents[MsgWParam]->ModelState == 0x400 || Agents[MsgWParam]->HP == 0){
				PostMessage((HWND)MsgLParam, 0x500, 1, 1);
			}else{
				PostMessage((HWND)MsgLParam, 0x500, 0, 0);
			}
			break;
		case 0x470: //Check if agent is casting/using skill : Return int/bool
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			if(Agents[MsgWParam]->Skill != NULL){
				PostMessage((HWND)MsgLParam, 0x500, 1, 1);
			}else{
				PostMessage((HWND)MsgLParam, 0x500, 0, 0);
			}
			break;
		case 0x471: //Get agent by player number (and the corresponding TeamId) : Return int/long & byte
			MsgInt = GetFirstAgentByPlayerNumber(MsgWParam);
			if(Agents[MsgInt] == NULL)
				PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			else
				PostMessage((HWND)MsgLParam, 0x500, MsgInt, Agents[MsgInt]->TeamId);
			break;
		case 0x472: //Get agents allegiance and team : Return word & byte
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->Allegiance, Agents[MsgWParam]->TeamId);
			break;
		case 0x473: //Get nearest enemy (by TeamId) to agent and the distance between them : Return int/long & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = GetNearestEnemyToAgent(MsgWParam);
			MsgFloat = GetDistanceFromAgentToAgent(MsgWParam, MsgInt);
			memcpy(&MsgInt2, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x474: //Check if agent is under attack from enemy melee (by TeamId) : Return int
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = IsAttackedMelee(MsgWParam);
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x475: //Get nearest item to agent : Return int/long & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = GetNearestItemToAgent(MsgWParam);
			MsgFloat = GetDistanceFromAgentToAgent(MsgWParam, MsgInt);
			memcpy(&MsgInt2, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x476: //Get nearest agent by player number to self : Return int/long & float
			MsgInt = GetNearestAgentByPlayerNumber(MsgWParam);
			if(Agents[MsgInt] == NULL){
				PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			}else{
				MsgFloat = GetDistanceFromAgentToAgent(myId, MsgInt);
				memcpy(&MsgInt2, &MsgFloat, sizeof(float));
				PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			}
			break;
		case 0x477: //Get current speed of agent : Return float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgFloat = sqrt(pow(Agents[MsgWParam]->MoveX, 2) + pow(Agents[MsgWParam]->MoveY, 2));
			memcpy(&MsgInt, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, 0);
			break;
		case 0x478: //Get nearest enemy to agent by allegiance and the distance between them : Return int/long & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = GetNearestEnemyToAgentByAllegiance(MsgWParam);
			MsgFloat = GetDistanceFromAgentToAgent(MsgWParam, MsgInt);
			memcpy(&MsgInt2, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x479: //Get nearest alive enemy to agent and the distance between them : Return int/long & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = GetNearestAliveEnemyToAgent(MsgWParam);
			MsgFloat = GetDistanceFromAgentToAgent(MsgWParam, MsgInt);
			memcpy(&MsgInt2, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x47A: //Get weapon type : Return int/long
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			PostMessage((HWND)MsgLParam, 0x500, Agents[MsgWParam]->WeaponType, 0);
			break;
		case 0x47B: //Get nearest signpost to agent : Return int/long & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = GetNearestSignpostToAgent(MsgWParam);
			MsgFloat = GetDistanceFromAgentToAgent(MsgWParam, MsgInt);
			memcpy(&MsgInt2, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x47C: //Get nearest npc to agent by allegiance : Return int/long & float
			if(MsgWParam == -1){MsgWParam = *(long*)CurrentTarget;}
			if(MsgWParam == -2){MsgWParam = myId;}
			if(Agents[MsgWParam]==NULL){RESPONSE_INVALID;}
			MsgInt = GetNearestNpcToAgentByAllegiance(MsgWParam);
			MsgFloat = GetDistanceFromAgentToAgent(MsgWParam, MsgInt);
			memcpy(&MsgInt2, &MsgFloat, sizeof(float));
			PostMessage((HWND)MsgLParam, 0x500, MsgInt, MsgInt2);
			break;
		case 0x47D: //Get nearest agent to coords : No return (use 0x47E to return)
			memcpy(&MsgFloat, &MsgWParam, sizeof(float));
			memcpy(&MsgFloat2, &MsgLParam, sizeof(float));
			TmpVariable = GetNearestAgentToCoords(MsgFloat, MsgFloat2);
			break;
		case 0x47E: //Get the values of MsgInt2 and TmpVariable : Return int/long & int/long
			PostMessage((HWND)MsgLParam, 0x500, MsgInt2, TmpVariable);
			break;
		case 0x47F: //Get nearest NPC to coords : No return (use 0x47E to return)
			memcpy(&MsgFloat, &MsgWParam, sizeof(float));
			memcpy(&MsgFloat2, &MsgLParam, sizeof(float));
			TmpVariable = GetNearestNPCToCoords(MsgFloat, MsgFloat2);
			break;

		//Item related commands
		case 0x510: //Get gold : Return int/long & int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->MoneySelf(), MySectionA->MoneyStorage());
			break;
		case 0x511: //Get bag size : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetBagSize(MsgWParam), 0);
			break;
		case 0x512: //Get backpack item id : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemId(1, MsgWParam), MyItemManager->GetItemModelId(1, MsgWParam));
			break;
		case 0x513: //Get belt pouch item id : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemId(2, MsgWParam), MyItemManager->GetItemModelId(2, MsgWParam));
			break;
		case 0x514: //Get bag 1 item id : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemId(3, MsgWParam), MyItemManager->GetItemModelId(3, MsgWParam));
			break;
		case 0x515: //Get bag 2 pack item id : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemId(4, MsgWParam), MyItemManager->GetItemModelId(4, MsgWParam));
			break;
		case 0x516: //Get equipment pack item id : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemId(5, MsgWParam), MyItemManager->GetItemModelId(5, MsgWParam));
			break;
		case 0x517: //Get first ID kit item id : Return int/long & int/long
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->FindIdKit(), 0);
			break;
		case 0x518: //Identify item by indexes : No return
			MsgInt = MyItemManager->FindIdKit();
			MsgInt2 = MyItemManager->GetItemId(MsgWParam, MsgLParam);
			if(!MsgInt || !MsgInt2){break;}
			IdentifyItem(MsgInt, MsgInt2);
			break;
		case 0x519: //Identify item by item id : No return
			MsgInt = MyItemManager->FindIdKit();
			if(!MsgInt){break;}
			IdentifyItem(MsgInt, MsgWParam);
			break;
		case 0x51A: //Deposit gold in storage : No return
			MsgInt = MySectionA->MoneySelf();
			MsgInt2 = MySectionA->MoneyStorage();
			if(MsgWParam == -1){
				if((MsgInt2 + MsgInt) > 1000000){ MsgInt = 1000000 - MsgInt2; }
				MsgInt2 += MsgInt;
				MsgInt = MySectionA->MoneySelf() - MsgInt;
			}else{
				MsgInt2 += MsgWParam;
				MsgInt -= MsgWParam;
			}
			ChangeGold(MsgInt, MsgInt2);
			break;
		case 0x51B: //Withdraw gold from storage : No return
			MsgInt = MySectionA->MoneySelf();
			MsgInt2 = MySectionA->MoneyStorage();
			if(MsgWParam == -1){
				if((MsgInt2 - (100000 - MsgInt)) < 0){
					MsgInt += MsgInt2;
					MsgInt2 = 0;
				}else{
					MsgInt2 -= 100000 - MsgInt;
					MsgInt += 100000;
				}
			}else{
				MsgInt2 -= MsgWParam;
				MsgInt += MsgWParam;
			}
			ChangeGold(MsgInt, MsgInt2);
			break;
		case 0x51C: //Sell item by indexes : No return
			MsgInt = MyItemManager->GetItemId(MsgWParam, MsgLParam);
			if(!SellSessionId || !MsgInt){break;}
			SellItem(MsgInt);
			break;
		case 0x51D: //Sell item by item id : No return
			if(!SellSessionId){break;}
			SellItem(MsgWParam);
			break;
		case 0x51E: //Buy ID kit : No return
			BuyItem(*(long*)(MySectionA->MerchantItems() + 0x10), 1, 100);
			break;
		case 0x51F: //Buy superior ID kit : No return
			BuyItem(*(long*)(MySectionA->MerchantItems() + 0x14), 1, 500);
			break;
		case 0x520: //Prepare MoveItem by setting item id (internal) : No return
			if(MsgWParam && MsgLParam){
				MoveItemId = MyItemManager->GetItemId(MsgWParam, MsgLParam);
			}else{
				MoveItemId = MsgWParam;
			}
			break;
		case 0x521: //Move the item specified by 0x520 : No return
			if(!MoveItemId){break;}
			MoveItem(MoveItemId, MyItemManager->GetBagPtr(MsgWParam)->id, (MsgLParam - 1));
			break;
		case 0x522: //Get backpack item rarity and quantity : Return byte & byte
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemPtr(1, MsgWParam)->extraItemInfo->rarity,
				MyItemManager->GetItemPtr(1, MsgWParam)->quantity);
			break;
		case 0x523: //Get belt pouch item rarity and quantity : Return byte & byte
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemPtr(2, MsgWParam)->extraItemInfo->rarity,
				MyItemManager->GetItemPtr(2, MsgWParam)->quantity);
			break;
		case 0x524: //Get bag 1 item rarity and quantity : Return byte & byte
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemPtr(3, MsgWParam)->extraItemInfo->rarity,
				MyItemManager->GetItemPtr(3, MsgWParam)->quantity);
			break;
		case 0x525: //Get bag 2 item rarity and quantity : Return byte & byte
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemPtr(4, MsgWParam)->extraItemInfo->rarity,
				MyItemManager->GetItemPtr(4, MsgWParam)->quantity);
			break;
		case 0x526: //Get equipment pack item rarity and quantity : Return byte & byte
			PostMessage((HWND)MsgLParam, 0x500, MyItemManager->GetItemPtr(5, MsgWParam)->extraItemInfo->rarity,
				MyItemManager->GetItemPtr(5, MsgWParam)->quantity);
			break;
		case 0x527: //Use item by indexes : No return
			MsgInt = MyItemManager->GetItemId(MsgWParam, MsgLParam);
			if(!MsgInt){break;}
			UseItem(MsgInt);
			break;
		case 0x528: //Use item by item id : No return
			UseItem(MsgWParam);
			break;
		case 0x529: //Drop item by indexes : No return
			if(MyItemManager->GetItemPtr(MsgWParam, MsgLParam)){
				DropItem(MyItemManager->GetItemId(MsgWParam, MsgLParam),
					MyItemManager->GetItemPtr(MsgWParam, MsgLParam)->quantity);
			}
			break;
		case 0x52A: //Drop item by id and specifying amount : No return
			if(MsgLParam == -1 && MyItemManager->GetItemPtr(MsgWParam)){
				MsgLParam = MyItemManager->GetItemPtr(MsgWParam)->quantity;
			}
			DropItem(MsgWParam, MsgLParam);
			break;
		case 0x52B: //Accept all unclaimed items : No return
			if(!MyItemManager->GetBagPtr(7)){break;}
			AcceptAllItems(MyItemManager->GetBagPtr(7)->id);
			break;

		//Title related commands
		case 0x550: //Get current Sunspear Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleSunspear(), 0);
			break;
		case 0x551: //Get current Lightbringer Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleLightbringer(), 0);
			break;
		case 0x552: //Get current Vanguard Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleVanguard(), 0);
			break;
		case 0x553: //Get current Norn Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleNorn(), 0);
			break;
		case 0x554: //Get current Asura Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleAsura(), 0);
			break;
		case 0x555: //Get current Deldrimor Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleDeldrimor(), 0);
			break;
		case 0x556: //Get current North Mastery Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleNorthMastery(), 0);
			break;
		case 0x557: //Get current Drunkard Title : Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleDrunkard(), 0);
			break;
		case 0x558: //Get current Sweet Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleSweet(), 0);
			break;
		case 0x559: //Get current Party Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleParty(), 0);
			break;
		case 0x55A: //Get current Commander Title: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleCommander(), 0);
			break;
		case 0x55B: //Get current Luxon Title Track: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleTrackLuxon(), 0);
			break;
		case 0x55C: //Get current Kurzick Title Track: Return int/long
			PostMessage((HWND)MsgLParam, 0x500, MySectionA->TitleTrackKurzick(), 0);
			break;
	}
	
	_asm {
		MOV EAX,1
		POP EDI
		POP ESI
		POP EBX
		MOV ESP,EBP
		POP EBP
		RETN 0x10
	}
}

void ReloadSkillbar(){
	MySkillbar = (Skillbar*)(*(dword*)MySectionA->SkillbarPtr());
}

void SellItem(long itemId){
	SellItemStruct* pSell = new SellItemStruct;
	pSell->sessionId = SellSessionId;
	pSell->itemId = itemId;

	_asm {
		MOV ECX,pSell
		CALL SellItemFunction
	}

	delete[] pSell;
}

void BuyItem(long id, long quantity, long value){
	long* itemQuantity = &quantity;
	long* itemId = &id;

	_asm {
		PUSH itemQuantity
		PUSH itemId
		PUSH 1
		PUSH 0
		PUSH 0
		PUSH 0
		PUSH 0
		MOV EDX,value
		MOV ECX,1
		CALL BuyItemFunction
	}
}

void WriteWhisper(const wchar_t* chatMsg, const wchar_t* chatName){
	 _asm {
		MOV EAX,chatMsg
		MOV EDX,chatName
		MOV ECX,0
		PUSH EAX
		CALL WriteWhisperStart
		MOV EAX,1
	}
}

wchar_t* GetAgentName(int agentId){
	_asm {
		MOV ECX,agentId
		CALL AgentNameFunction
		ADD EAX,4
	}
}

void TargetNearestFoe(){
	_asm {
		MOV EBX,0
		MOV EAX,TargetFunctions
		CALL EAX
	}
}

void TargetNearestAlly(){
	_asm {
		MOV EBX,0
		MOV EAX,TargetFunctions
		ADD EAX,0x1D
		CALL EAX
	}
}

void TargetNearestItem(){
	_asm {
		MOV EBX,0
		MOV EAX,TargetFunctions
		ADD EAX,0x3A
		CALL EAX
	}
}

void TargetCalledTarget(){
	_asm {
		MOV EBX,0
		MOV EAX,TargetFunctions
		ADD EAX,0x115
		CALL EAX
	}
}

void UseHero1Skill(long SkillNumber, long Target){
	_asm {
		MOV EDX,SkillNumber
		MOV ECX,0x1A
		PUSH Target
		CALL HeroSkillFunction
	}
}

void UseHero2Skill(long SkillNumber, long Target){
	_asm {
		MOV EDX,SkillNumber
		MOV ECX,0x1B
		PUSH Target
		CALL HeroSkillFunction
	}
}

void UseHero3Skill(long SkillNumber, long Target){
	_asm {
		MOV EDX,SkillNumber
		MOV ECX,0x1C
		PUSH Target
		CALL HeroSkillFunction
	}
}

bool CompareAccName(wchar_t* cmpName){
	if(wcscmp(cmpName, MySectionA->Email()) == NULL)
		return true;
	else
		return false;
}

bool CompareCharName(wchar_t* cmpName){
	if(wcscmp(cmpName, MySectionA->Name()) == NULL)
		return true;
	else
		return false;
}

void ChangeMaxZoom(float fZoom){
	DWORD dwOldProtection;
	VirtualProtect(MaxZoomStill, sizeof(float), PAGE_EXECUTE_READWRITE, &dwOldProtection);
	memcpy(MaxZoomStill, &fZoom, sizeof(float));
	VirtualProtect(MaxZoomStill, sizeof(float), dwOldProtection, NULL);
	VirtualProtect(MaxZoomMobile, sizeof(float), PAGE_EXECUTE_READWRITE, &dwOldProtection);
	memcpy(MaxZoomMobile, &fZoom, sizeof(float));
	VirtualProtect(MaxZoomStill, sizeof(float), dwOldProtection, NULL);
}

void SendPacket(CPacket* pak){
	if(WaitForSingleObject(PacketMutex, 1000) == WAIT_TIMEOUT) return;
	PacketQueue.push_back(pak);
	ReleaseMutex(PacketMutex);
}

void SendPacketQueueThread(){
	while(true){
		Sleep(10);

		if(WaitForSingleObject(PacketMutex, 100) == WAIT_TIMEOUT) continue;
		if(PacketQueue.size() < 1 || mapLoading == 2) goto nextLoop;
		if(MySectionA->LoggedIn() != 1 && mapLoading != 1) goto nextLoop;

		{
			std::vector<CPacket*>::iterator itrPak = PacketQueue.begin();
			CPacket* CurPacket = *itrPak;

			dword testValue = 0x99;
			_asm {
				MOV ECX, FlagLocation
				MOV ECX, DWORD PTR DS:[ECX]
				MOVZX ECX, BYTE PTR DS:[ECX+8]
				MOV testValue, ECX
			}
			if((testValue & 1)){
				{
					byte* buffer = CurPacket->Buffer;
					dword psize = CurPacket->Size;
				
					_asm {
						MOV EAX, PacketLocation
						MOV EAX, DWORD PTR DS:[EAX]
						MOV ECX, EAX
						MOV EDX, psize
						PUSH buffer
						CALL PacketSendFunction
					}
				}

				delete [] CurPacket->Buffer;
				delete CurPacket;
			}

			PacketQueue.erase(itrPak);
		}

nextLoop:
		ReleaseMutex(PacketMutex);
	}
}

void SkillLogQueueThread(){
	COPYDATASTRUCT SkillLogCDS;
	LoggedSkillStruct SkillInfo;

	SkillLogCDS.dwData = 1;
	SkillLogCDS.lpData = &SkillInfo;
	SkillLogCDS.cbData = sizeof(LoggedSkillStruct);

	wchar_t* sWindowText = new wchar_t[50];
	dword tTicks = 0;

	while(true){
		Sleep(10);
		if(SkillLogQueue.size() > 0 && LogSkills){
			SkillInfo.AgentId = SkillLogQueue.front().AgentId;
			
			if(Agents[SkillInfo.AgentId] != NULL){
				SkillInfo.TeamId = Agents[SkillInfo.AgentId]->TeamId;
				SkillInfo.Allegiance = Agents[SkillInfo.AgentId]->Allegiance;
			}else{
				SkillInfo.TeamId = 0;
				SkillInfo.Allegiance = 0;
			}

			SkillInfo.Distance = GetDistanceFromAgentToAgent(myId, SkillInfo.AgentId);
			SkillInfo.MyId = myId;
			SkillInfo.SkillId = SkillLogQueue.front().Skill;
			SkillInfo.Activation = SkillLogQueue.front().Activation;
			SkillInfo.Ping = MySectionA->Ping();

			SendMessage(ScriptHwnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)&SkillLogCDS);

			SkillLogQueue.erase(SkillLogQueue.begin()); //Remove handled skill from queue
		}else{
			SkillLogQueue.clear();
		}

		if(SkillCancelQueue.size() > 0 && LogSkills){
			PostMessage(ScriptHwnd, 0x501, SkillCancelQueue.front().AgentId, SkillCancelQueue.front().Skill);
			SkillCancelQueue.erase(SkillCancelQueue.begin());
		}else{
			SkillCancelQueue.clear();
		}
		
		if(MsgHwnd){
			if((GetTickCount() - tTicks) > 5000){
				tTicks = GetTickCount();

				if(!MySectionA->Name()[0]){
					SetWindowTextW(MsgHwnd, L"Guild Wars");
				}else{
					swprintf(sWindowText, L"Guild Wars - %s", MySectionA->Name());
					SetWindowTextW(MsgHwnd, sWindowText);
				}
			}
		}
	}
}

void FindOffsets(){
	byte* start = (byte*)0x00401000;
	byte* end = (byte*)0x00DF0000;

	byte PacketSendCode[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x2C, 0x53, 0x56, 0x57, 0x8B,
		0xF9, 0x85 };
	size_t PacketSendCodeSize = 12;

	byte BaseOffsetCode[] = { 0x56, 0x33, 0xF6, 0x3B, 0xCE, 0x74, 0x0E, 0x56, 0x33, 0xD2 };
	size_t BaseOffsetCodeSize = 10;

	byte AgentBaseCode[] = { 0x56, 0x8B, 0xF1, 0x3B, 0xF0, 0x72, 0x04 };
	size_t AgentBaseCodeSize = 7;

	byte MessageHandlerCode[] = { 0x8B, 0x86, 0xA4, 0x0C, 0x00, 0x00, 0x85, 0xC0, 0x0F };
	size_t MessageHandlerCodeSize = 9;

	byte SkillLogCode[] = { 0x8B, 0x46, 0x10, 0x5F, 0x40 };
	size_t SkillLogCodeSize = 5;

	byte MapIdLocationCode[] = { 0xB0, 0x7F, 0x8D, 0x55 };
	size_t MapIdLocationCodeSize = 4;

	byte WriteWhisperCode[] = { 0x55, 0x8B, 0xEC, 0x51, 0x53, 0x89, 0x4D, 0xFC, 0x8B, 0x4D,
		0x08, 0x56, 0x57, 0x8B };
	size_t WriteWhisperCodeSize = 14;

	byte TargetFunctionsCode[] = { 0xBA, 0x01, 0x00, 0x00, 0x00, 0xB9, 0x00, 0x80, 0x00, 0x00,
		0xE8 };
	size_t TargetFunctionsCodeSize = 11;

	byte HeroSkillFunctionCode[] = { 0x5E, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
		0x90, 0x90, 0x55, 0x8B, 0xEC, 0x8B, 0x45, 0x08, 0x50 };
	size_t HeroSkillFunctionCodeSize = 19;

	byte ClickToMoveCode[] = { 0x3D, 0xD3, 0x01, 0x00, 0x00, 0x74 };
	size_t ClickToMoveCodeSize = 6;

	byte BuildNumberCode[] = { 0x8D, 0x85, 0x00, 0xFC, 0xFF, 0xFF, 0x8D };
	size_t BuildNumberCodeSize = 7;

	byte ChangeTargetCode[] = { 0x33, 0xC0, 0x3B, 0xDA, 0x0F, 0x95, 0xC0, 0x33 };
	size_t ChangeTargetCodeSize = 8;

	byte MaxZoomStillCode[] = { 0x3B, 0x44, 0x8B, 0xCB };
	size_t MaxZoomStillCodeSize = 4;

	byte MaxZoomMobileCode[] = { 0x50, 0xEB, 0x11, 0x68, 0x00, 0x80, 0x3B, 0x44, 0x8B, 0xCE };
	size_t MaxZoomMobileCodeSize = 10;

	byte SkillCancelCode[] = { 0x85, 0xC0, 0x74, 0x1D, 0x6A, 0x00, 0x6A, 0x42 };
	size_t SkillCancelCodeSize = 8;

	byte AgentNameCode[] = { 0x57, 0x8B, 0x14, 0x81, 0x8B, 0x82, 0x04, 0x00, 0x00, 0x00,
		0x8B, 0x78, 0x2C, 0xE8 };
	size_t AgentNameCodeSize = 14;

	byte SellSessionCode[] = { 0x33, 0xD2, 0x8B, 0xCF, 0xC7, 0x46, 0x0C };
	size_t SellSessionCodeSize = 7;

	byte SellItemCode[] = { 0x8B, 0x46, 0x0C, 0x8D, 0x7E, 0x0C, 0x85 };
	size_t SellItemCodeSize = 7;

	byte BuyItemCode[] = { 0x64, 0x8B, 0x0D, 0x2C, 0x00, 0x00, 0x00, 0x89, 0x55, 0xFC,
		0x8B };
	size_t BuyItemCodeSize = 11;

	byte PingLocationCode[] = { 0x90, 0x8D, 0x41, 0x24, 0x8B, 0x49, 0x18, 0x6A, 0x30 };
	size_t PingLocationCodeSize = 9;

	byte LoggedInLocationCode[] = { 0x85, 0xC0, 0x74, 0x11, 0xB8, 0x07 };
	size_t LoggedInLocationCodeSize = 6;

	byte NameLocationCode[] = { 0x6A, 0x14, 0x8D, 0x96, 0xBC };
	size_t NameLocationCodeSize = 5;

	byte DeadLocationCode[] = { 0x85, 0xC0, 0x74, 0x11, 0xB8, 0x02 };
	size_t DeadLocationCodeSize = 6;

	byte BasePointerLocationCode[] = { 0x85, 0xC9, 0x74, 0x3D, 0x8B, 0x46 };
	size_t BasePointerLocationCodeSize = 6;

	byte DialogCode[] = { 0x55, 0x8B, 0xEC, 0x8B, 0x41, 0x08, 0xA8, 0x01, 0x75, 0x24 };
	size_t DialogCodeSize = 10;

	while(start!=end){
		if(!memcmp(start, AgentBaseCode, AgentBaseCodeSize)){
			AgentArrayPtr = (byte*)(*(dword*)(start+0xC));
			AgentArrayMaxPtr = AgentArrayPtr+0x8;
			CurrentTarget = AgentArrayPtr-0x500;
		}
		if(!memcmp(start, BaseOffsetCode, BaseOffsetCodeSize)){
			BaseOffset = start;
		}
		if(!memcmp(start, PacketSendCode, PacketSendCodeSize)){
			PacketSendFunction = start;
		}
		if(!memcmp(start, MessageHandlerCode, MessageHandlerCodeSize)){
			MessageHandlerStart = start-0x95;
			MessageHandlerReturn = MessageHandlerStart+9;
		}
		if(!memcmp(start, SkillLogCode, SkillLogCodeSize)){
			SkillLogStart = start;
			SkillLogReturn = SkillLogStart+8;
		}
		if(!memcmp(start, MapIdLocationCode, MapIdLocationCodeSize)){
			MapIdLocation = (byte*)(*(dword*)(start+0x46));
		}
		if(!memcmp(start, WriteWhisperCode, WriteWhisperCodeSize)){
			WriteWhisperStart = start;
		}
		if(!memcmp(start, TargetFunctionsCode, TargetFunctionsCodeSize)){
			TargetFunctions = start;
		}
		if(!memcmp(start, HeroSkillFunctionCode, HeroSkillFunctionCodeSize)){
			HeroSkillFunction = start+0xC;
		}
		if(!memcmp(start, ClickToMoveCode, ClickToMoveCodeSize)){
			ClickToMoveFix = start;
		}
		if(!memcmp(start, BuildNumberCode, BuildNumberCodeSize)){
			BuildNumber = start+0x53;
		}
		if(!memcmp(start, ChangeTargetCode, ChangeTargetCodeSize)){
			ChangeTargetFunction = start-0x78;
		}
		if(!memcmp(start, MaxZoomStillCode, MaxZoomStillCodeSize)){
			MaxZoomStill = start-2;
		}
		if(!memcmp(start, MaxZoomMobileCode, MaxZoomMobileCodeSize)){
			MaxZoomMobile = start+4;
		}
		if(!memcmp(start, SkillCancelCode, SkillCancelCodeSize)){
			SkillCancelStart = start-0xE;
			SkillCancelReturn = SkillCancelStart+7;
		}
		if(!memcmp(start, AgentNameCode, AgentNameCodeSize)){
			AgentNameFunction = start-0x16;
		}
		if(!memcmp(start, SellSessionCode, SellSessionCodeSize)){
			SellSessionStart = start-0x48;
			SellSessionReturn = SellSessionStart+9;
		}
		if(!memcmp(start, SellItemCode, SellItemCodeSize)){
			SellItemFunction = start-8;
		}
		if(!memcmp(start, BuyItemCode, BuyItemCodeSize)){
			BuyItemFunction = start-0xE;
		}
		if(!memcmp(start, PingLocationCode, PingLocationCodeSize)){
			PingLocation = (byte*)(*(dword*)(start-9));
		}
		if(!memcmp(start, LoggedInLocationCode, LoggedInLocationCodeSize)){
			LoggedInLocation = (byte*)(*(dword*)(start-4) + 4);
		}
		if(!memcmp(start, NameLocationCode, NameLocationCodeSize)){
			NameLocation = (byte*)(*(dword*)(start+9));
			EmailLocation = (byte*)(*(dword*)(start-9));
		}
		if(!memcmp(start, DeadLocationCode, DeadLocationCodeSize)){
			DeadLocation = (byte*)(*(dword*)(start-4));
		}
		if(!memcmp(start, BasePointerLocationCode, BasePointerLocationCodeSize)){
			BasePointerLocation = (byte*)(*(dword*)(start-4));
		}
		if(!memcmp(start, DialogCode, DialogCodeSize)){
			DialogStart = start;
			DialogReturn = DialogStart+8;
		}
		if(	CurrentTarget &&
			BaseOffset &&
			PacketSendFunction &&
			MessageHandlerStart &&
			SkillLogStart &&
			MapIdLocation &&
			WriteWhisperStart &&
			TargetFunctions &&
			HeroSkillFunction &&
			ClickToMoveFix &&
			BuildNumber &&
			ChangeTargetFunction &&
			MaxZoomStill &&
			MaxZoomMobile &&
			SkillCancelStart &&
			SellSessionStart &&
			SellItemFunction &&
			BuyItemFunction &&
			PingLocation &&
			LoggedInLocation &&
			NameLocation &&
			DeadLocation &&
			BasePointerLocation &&
			DialogStart){
			return;
		}
		start++;
	}
}

void WriteJMP(byte* location, byte* newFunction){
	DWORD dwOldProtection;
	VirtualProtect(location, 7, PAGE_EXECUTE_READWRITE, &dwOldProtection);
		location[0] = 0xB8;
		*((dword*)(location + 1)) = (dword)newFunction;
		location[5] = 0xFF;
		location[6] = 0xE0;
	VirtualProtect(location, 7, dwOldProtection, &dwOldProtection);
}

void InjectErr(const char* lpzText){
	char* buf = new char[100];
	sprintf(buf, "The %s could not be found!\nPlease contact SOMEONE about this issue.", lpzText);
	MessageBox(NULL, buf, "Hooking error!", MB_OK);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason){
		case DLL_PROCESS_ATTACH:
			FindOffsets();
			if(!BaseOffset){
				InjectErr("BaseOffset");
				return false;
			}
			if(!PacketSendFunction){
				InjectErr("PacketSendFunction");
				return false;
			}else{
				PacketLocation = *(reinterpret_cast<dword*>(BaseOffset - 4));
				FlagLocation = PacketLocation - 0x130;
				PacketMutex = CreateMutex(NULL, false, NULL);
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&SendPacketQueueThread, 0, 0, 0);
			}
			if(!AgentArrayPtr){
				InjectErr("AgentArrayPtr");
				return false;
			}
			if(!MessageHandlerStart){
				InjectErr("MessageHandler");
				return false;
			}else{
				DWORD dwOldProtection;
				VirtualProtect(MessageHandlerStart, 9, PAGE_EXECUTE_READWRITE, &dwOldProtection);
				memset(MessageHandlerStart, 0x90, 9);
				VirtualProtect(MessageHandlerStart, 9, dwOldProtection, NULL);
				WriteJMP(MessageHandlerStart, (byte*)CustomMsgHandler);
			}
			if(!SkillLogStart){
				InjectErr("SkillLog");
				return false;
			}else{
				DWORD dwOldProtection;
				VirtualProtect(SkillLogStart, 8, PAGE_EXECUTE_READWRITE, &dwOldProtection);
				memset(SkillLogStart, 0x90, 8);
				VirtualProtect(SkillLogStart, 8, dwOldProtection, NULL);
				WriteJMP(SkillLogStart, (byte*)SkillLogHook);
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&SkillLogQueueThread, 0, 0, 0);
			}
			if(!MapIdLocation){
				InjectErr("MapIdLocation");
				return false;
			}
			if(!WriteWhisperStart){
				InjectErr("WriteWhisperStart");
				return false;
			}
			if(!TargetFunctions){
				InjectErr("TargetFunctions");
				return false;
			}
			if(!HeroSkillFunction){
				InjectErr("HeroSkillFunction");
				return false;
			}
			if(!ClickToMoveFix){
				InjectErr("ClickToMoveFix");
				return false;
			}else{
				DWORD dwOldProtection;
				VirtualProtect(ClickToMoveFix, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
				byte ClickToMoveFixCode[] = { 0x83, 0xF8, 0x00, 0x90, 0x90 };
				memcpy(ClickToMoveFix, ClickToMoveFixCode, 5);
				VirtualProtect(ClickToMoveFix, 5, dwOldProtection, NULL);
			}
			if(!BuildNumber){
				InjectErr("BuildNumber");
				return false;
			}
			if(!ChangeTargetFunction){
				InjectErr("ChangeTargetFunction");
				return false;
			}
			if(!MaxZoomStill){
				InjectErr("MaxZoomStill");
				return false;
			}
			if(!MaxZoomMobile){
				InjectErr("MaxZoomMobile");
				return false;
			}
			if(!SkillCancelStart){
				InjectErr("SkillCancelStart");
				return false;
			}else{
				DWORD dwOldProtection;
				VirtualProtect(SkillCancelStart, 7, PAGE_EXECUTE_READWRITE, &dwOldProtection);
				memset(SkillCancelStart, 0x90, 7);
				VirtualProtect(SkillCancelStart, 7, dwOldProtection, NULL);
				WriteJMP(SkillCancelStart, (byte*)SkillCancelHook);
			}
			if(!AgentNameFunction){
				InjectErr("AgentNameFunction");
				return false;
			}
			if(!SellSessionStart){
				InjectErr("SellSessionStart");
				return false;
			}else{
				DWORD dwOldProtection;
				VirtualProtect(SellSessionStart, 9, PAGE_EXECUTE_READWRITE, &dwOldProtection);
				memset(SellSessionStart, 0x90, 9);
				VirtualProtect(SellSessionStart, 9, dwOldProtection, NULL);
				WriteJMP(SellSessionStart, (byte*)SellSessionHook);
			}
			if(!SellItemFunction){
				InjectErr("SellItemFunction");
				return false;
			}
			if(!BuyItemFunction){
				InjectErr("BuyItemFunction");
				return false;
			}
			if(!PingLocation){
				InjectErr("PingLocation");
				return false;
			}
			if(!LoggedInLocation){
				InjectErr("LoggedInLocation");
				return false;
			}
			if(!NameLocation){
				InjectErr("NameLocation");
				return false;
			}
			if(!DeadLocation){
				InjectErr("DeadLocation");
				return false;
			}
			if(!BasePointerLocation){
				InjectErr("BasePointerLocation");
				return false;
			}
			if(!DialogStart){
				InjectErr("DialogStart");
				return false;
			}else{
				DWORD dwOldProtection;
				VirtualProtect(DialogStart, 8, PAGE_EXECUTE_READWRITE, &dwOldProtection);
				memset(DialogStart, 0x90, 8);
				VirtualProtect(DialogStart, 8, dwOldProtection, NULL);
				WriteJMP(DialogStart, (byte*)DialogHook);
			}
			
			/*
			AllocConsole();
			FILE *fh;
			freopen_s(&fh, "CONOUT$", "wb", stdout);
			printf("BaseOffset=0x%06X\n", BaseOffset);
			printf("PacketSendFunction=0x%06X\n", PacketSendFunction);
			printf("CurrentTarget=0x%06X\n", CurrentTarget);
			printf("AgentArrayPtr=0x%06X\n", AgentArrayPtr);
			printf("AgentArrayMaxPtr=0x%06X\n", AgentArrayMaxPtr);
			printf("MessageHandlerStart=0x%06X\n", MessageHandlerStart);
			printf("MessageHandlerReturn=0x%06X\n", MessageHandlerReturn);
			printf("SkillLogStart=0x%06X\n", SkillLogStart);
			printf("SkillLogReturn=0x%06X\n", SkillLogReturn);
			printf("WriteWhisperStart=0x%06X\n", WriteWhisperStart);
			printf("TargetFunctions=0x%06X\n", TargetFunctions);
			printf("HeroSkillFunction=0x%06X\n", HeroSkillFunction);
			printf("ClickToMoveFix=0x%06X\n", ClickToMoveFix);
			printf("BuildNumber=0x%06X\n", BuildNumber);
			printf("ChangeTargetFunction=0x%06X\n", ChangeTargetFunction);
			printf("MaxZoomStill=0x%06X\n", MaxZoomStill);
			printf("MaxZoomMobile=0x%06X\n", MaxZoomMobile);
			printf("SkillCancelStart=0x%06X\n", SkillCancelStart);
			printf("SkillCancelReturn=0x%06X\n", SkillCancelReturn);
			printf("AgentNameFunction=0x%06X\n", AgentNameFunction);
			printf("SellSessionStart=0x%06X\n", SellSessionStart);
			printf("SellItemFunction=0x%06X\n", SellItemFunction);
			printf("BuyItemFunction=0x%06X\n", BuyItemFunction);
			printf("PingLocation=0x%06X\n", PingLocation);
			printf("LoggedInLocation=0x%06X\n", LoggedInLocation);
			printf("NameLocation=0x%06X\n", NameLocation);
			printf("DeadLocation=0x%06X\n", DeadLocation);
			printf("BasePointerLocation=0x%06X\n", BasePointerLocation);
			printf("DialogStart=0x%06X\n", DialogStart);
			printf("DialogReturn=0x%06X\n", DialogReturn);
			*/
			break;

		case DLL_PROCESS_DETACH:
			break;
	}
	return true;
}