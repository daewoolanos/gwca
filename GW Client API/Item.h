#ifndef _ITEM_H
#define _ITEM_H

struct Bag {
	byte unknown1[4];
	long index;
	long id;
	Item* containerItem;
	long numOfItems;
	Bag** bagArray;
	Item** itemArray;
	long fakeSlots;
	long slots;
};

struct ItemExtra {
	byte rarity;
	byte unknown1[3];
	byte modifier;
	byte unknown2[13];
	byte lastModifier;
};

struct ItemExtraReq {
	byte requirement;
	byte attribute; //Skill Template Format
};

struct Item {
	long id;
	long agentId; //non-zero if on the ground
	byte unknown1[4];
	Bag* bag;
	ItemExtraReq* extraItemReq;
	byte unknown2[4];
	wchar_t* customized;
	byte unknown3[6];
	short extraId;
	byte unknown4[8];
	long modelId;
	byte* modStruct;
	byte unknown5[4];
	ItemExtra* extraItemInfo;
	byte unknown6[15];
	byte quantity;
	byte unknown7[2];
	byte slot;
};

struct DyeInfo {
	byte unknown1;
	byte Shinyness;
	short DyeId;
};

struct EquipmentItemData {
	long ModelId;
	DyeInfo Dye;
	long Value;
	byte unknown[4];
};

struct Equipment {
	byte* vtable;
	byte unknown1[32];
	EquipmentItemData Items[7];
	/* EquipmentItemData Weapon;
	EquipmentItemData Offhand;
	EquipmentItemData Chest;
	EquipmentItemData Legs;
	EquipmentItemData Head;
	EquipmentItemData Feet;
	EquipmentItemData Hands; */
	byte unknown2[32];
	long ItemIds[7];
};

class ItemManager {
public:
	ItemManager(){}
	~ItemManager();

	long GetBagSize(int iBag){
		Bag* pBag = GetBagPtr(iBag);
		if(!pBag){ return 0; }
		return pBag->slots;
	}

	long GetBagItems(int iBag){
		Bag* pBag = GetBagPtr(iBag);
		if(!pBag){ return 0; }
		return pBag->numOfItems;
	}

	long GetItemId(int iBag, int iSlot){
		Item* pItem = GetItemPtr(iBag, iSlot);
		if(!pItem){ return 0; }

		return pItem->id;
	}

	long GetItemModelId(int iBag, int iSlot){
		Item* pItem = GetItemPtr(iBag, iSlot);
		if(!pItem){ return 0; }

		return pItem->modelId;
	}

	long GetItemModelId(long itemId){
		Item* pItem = GetItemPtr(itemId);
		if(!pItem){ return 0; }

		return pItem->modelId;
	}

	long FindIdKit(){	
		for(int i = 1;i < 17;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ continue; }
				if(pCurrentItem->modelId == 2989||pCurrentItem->modelId == 5899){ return pCurrentItem->id; }
			}
		}
		return 0;
	}

	long FindSalvageKit(){
		for(int i = 1;i < 17;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ continue; }
				if(pCurrentItem->modelId == 2991||pCurrentItem->modelId == 2992){ return pCurrentItem->id; }
			}
		}
		return 0;
	}

	long GetItemByModelId(long model, long startingBag = 1){
		for(int i = startingBag;i < 17;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ continue; }
				if(pCurrentItem->modelId == model){ return pCurrentItem->id; }
			}
		}
		return 0;
	}

	long GetItemPositionByItemId(long itemId, long mode, long startingBag = 1){
		for(int i = startingBag;i < 17;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ continue; }
				if(pCurrentItem->id == itemId){
					if(mode == 1){ return i; }
					if(mode == 2){ return j+1; }
				}
			}
		}

		return 0;
	}

	long GetItemPositionByModelId(long modelId, long mode, long startingBag = 1){
			for(int i = startingBag;i < 17;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ continue; }
				if(pCurrentItem->modelId == modelId){
					if(mode == 1){ return i; }
					if(mode == 2){ return j+1; }
				}
			}
		}
		return 0;
	}

	long GetDyePositionByColor(short extraId, long mode, long startingBag = 1){
			for(int i = startingBag;i < 17;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ continue; }
				if(pCurrentItem->modelId == 146 && pCurrentItem->extraId == extraId){
					if(mode == 1){ return i; }
					if(mode == 2){ return j+1; }
				}
			}
		}
		return 0;
	}

	long FindEmptySlot(long startingBag, long mode){
		for(int i = startingBag;i < 17;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ 
					if(mode == 1){ return i; }
					if(mode == 2){ return j+1; }
				}
			}
		}
		return 0;
	}

	long FindNextGoldItem(long lastBag, long startingBag = 1){
		for(int i = startingBag;i < lastBag+1;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ continue; }
				if(pCurrentItem->extraItemInfo->rarity == 0x40){ return pCurrentItem->id; }
			}
		}
		return 0;
	}

	long GetItemPositionByRarity(byte rarity, long mode, long startingBag = 1){
		for(int i = startingBag;i < 17;i++){
			Bag* pBag = GetBagPtr(i);
			if(!pBag){ continue; }

			Item** pItems = pBag->itemArray;
			if(!pItems){ continue; }
			Item* pCurrentItem;
			for(int j = 0;j < pBag->slots;j++){
				pCurrentItem = pItems[j];
				if(!pCurrentItem){ continue; }
				if(pCurrentItem->extraItemInfo->rarity == rarity){
					if(mode == 1){ return i; }
					if(mode == 2){ return j+1; }
				}
			}
		}
		return 0;
	}

	long GetOffhandDamageMod(long itemId){
		Item* pItem = GetItemPtr(itemId);
		if(!pItem){ return 0; }
		byte* pItemMods = pItem->modStruct;
		if(!pItemMods){ return 0; }

		byte DmgModSignature[] = { 0xA8, 0x0A, 0x0A, 0x01, 0xAC, 0x0A, 0x0A, 0x01 };

		for(int i = 1;i < 140;i++){
			if(!memcmp(pItemMods + i, DmgModSignature, sizeof(DmgModSignature))){
				return pItemMods[i + 8];
			}
		}
		return 0;
	}

	Bag* GetBagPtr(int iBag){
		if(iBag < 1){ return 0; }
		Bag** pBags = MySectionA->BagArrayPointer();
		if(!pBags){ return 0; }
		Bag* pBag = pBags[iBag];
		return pBag;
	}

	Item* GetItemPtr(int iBag, int iSlot){
		Bag* pBag = GetBagPtr(iBag);
		if(!pBag){ return 0; }
		if(iSlot > pBag->slots || iSlot < 1){ return 0; }

		Item** pItems = pBag->itemArray;
		if(!pItems){ return 0; }

		Item* pItem = pItems[(iSlot - 1)];
		return pItem;
	}

	Item* GetItemPtr(long itemId){
		if(itemId > MySectionA->ItemArraySize()){ return 0; }

		Item** aItems = MySectionA->ItemArray();
		if(!aItems){ return 0; }

		return aItems[itemId];
	}

	Item* GetItemPtrByAgentId(long agentId){
		Item** aItems = MySectionA->ItemArray();
		Item* pCurrentItem = NULL;

		for(int i = 1;i < MySectionA->ItemArraySize();i++){
			pCurrentItem = aItems[i];
			if(!pCurrentItem){ continue; }
			if(pCurrentItem->agentId == agentId){ return pCurrentItem; }
		}
		return NULL;
	}
};

#endif