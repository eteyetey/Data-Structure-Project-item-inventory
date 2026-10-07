#include "Item.h"
#include <iostream>

// 기본 생성자
Item::Item() {
    id = 0;
    name = "";
    category = "";
    rarity = 0;

    type = ItemType::Unusable;

    durability = 0;
    maxDurability = 0;
}

// 현재 내구도가 최대치인 경우
Item::Item(int id, string name, string category,
    int rarity, ItemType type, int maxDurability) {

    this->id = id;
    this->name = name;
    this->category = category;
    this->rarity = rarity;
    this->type = type;

    // 내구도형 아이템만 내구도 사용
    if (type == ItemType::Durability) {
        this->maxDurability = maxDurability;
        this->durability = maxDurability;
    }
    else {
        this->maxDurability = 0;
        this->durability = 0;
    }
}

// 현재 내구도를 따로 설정하고 싶은 경우
Item::Item(int id, string name, string category,
    int rarity, ItemType type,
    int durability, int maxDurability) {

    this->id = id;
    this->name = name;
    this->category = category;
    this->rarity = rarity;
    this->type = type;

    if (type == ItemType::Durability) {
        this->maxDurability = maxDurability;

        // 현재 내구도가 음수라면 0으로 맞춤
        if (durability < 0) {
            this->durability = 0;
        }

        // 현재 내구도가 최대치를 넘을 경우 최대치로 맞춤
        else if (durability > maxDurability) {
            this->durability = maxDurability;
        }

        else {
            this->durability = durability;
        }
    }
    else {
        this->maxDurability = 0;
        this->durability = 0;
    }
}


// getter

int Item::getId() const {
    return id;
}

string Item::getName() const {
    return name;
}

string Item::getCategory() const {
    return category;
}

int Item::getRarity() const {
    return rarity;
}

ItemType Item::getType() const {
    return type;
}

int Item::getDurability() const {
    return durability;
}

int Item::getMaxDurability() const {
    return maxDurability;
}


// setter

void Item::setName(const string& name) {
    this->name = name;
}

void Item::setCategory(const string& category) {
    this->category = category;
}

void Item::setRarity(int rarity) {
    this->rarity = rarity;
}


// 아이템 사용

bool Item::use() {

    switch (type) {

    case ItemType::Consumable:
        // 수량 감소는 나중에 Inventory에서 처리
        printUseMsg(name);
        return true;

    case ItemType::Durability:

        // 내구도가 없으면 사용 실패
        if (durability <= 0) {
            return false;
        }

        durability--;
        printUseMsg(name);
        return true;

    case ItemType::Permanent:
        printUseMsg(name);
        return true;

    case ItemType::Unusable:
        return false;
    }

    return false;
}


// 아이템 사용 메시지 출력

void Item::printUseMsg(string name) {
    cout << name << " used successfully!" << endl;
}


// 사용 가능한 아이템인지 확인

bool Item::isUsable() const {

    if (type == ItemType::Unusable) {
        return false;
    }

    // 내구도형 아이템인데 내구도가 없으면 사용 불가능
    if (type == ItemType::Durability && durability <= 0) {
        return false;
    }

    return true;
}


// 내구도가 모두 소모되었는지 확인

bool Item::isBroken() const {
    return type == ItemType::Durability && durability <= 0;
}