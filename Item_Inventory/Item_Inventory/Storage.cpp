
#include "Storage.h"

using namespace std;


// 생성자
Storage::Storage(int id, const string& name) {
    this->id = id;
    this->name = name;
}


// 두 인벤토리 사이에서 아이템 이동
AddResult Storage::transfer(Inventory& from, Inventory& to, int index, int quantity) {

    AddResult result = { 0, quantity };

    // 잘못된 수량
    if (quantity <= 0) {
        return { 0, 0 };
    }

    // 자기 자신에게 이동 불가
    if (&from == &to) {
        return result;
    }

    // 이동할 아이템 조회
    Item* item = from.getItem(index);

    if (item == nullptr) {
        return result;
    }

    // 실제 보유 수량보다 많이 이동 불가
    if (quantity > item->getQuantity()) {
        return result;
    }

    // 중첩 불가능한 아이템은 1개만 이동 가능
    if (!item->isStackable() && quantity != 1) {
        return result;
    }

    // 이동할 아이템 복사
    Item movingItem = *item;
    movingItem.setQuantity(quantity);

    // 받는 쪽에 먼저 추가
    result = to.addItem(movingItem);

    // 실제 이동된 수량이 없으면 종료
    if (result.added <= 0) {
        return result;
    }

    // 보내는 쪽 아이템 수량 감소
    int remainingQuantity = item->getQuantity() - result.added;

    if (remainingQuantity == 0) {
        from.removeItemAt(index);
    }
    else {
        item->setQuantity(remainingQuantity);
    }

    return result;
}


// 플레이어 인벤토리 -> 보관함
AddResult Storage::deposit(Inventory& player, int index, int quantity) {
    return transfer(player, inventory, index, quantity);
}


// 보관함 -> 플레이어 인벤토리
AddResult Storage::withdraw(Inventory& player, int index, int quantity) {
    return transfer(inventory, player, index, quantity);
}


// 내부 Inventory 반환
Inventory& Storage::getInventory() {
    return inventory;
}

int Storage::getId() const {
    return id;
}

string Storage::getName() const {
    return name;
}

void Storage::setName(string newName) {
    name = newName;
}


// 보관함 최대 슬롯 개수 설정
bool Storage::setMaxSlots(int value) {
    return inventory.setMaxSlots(value);
}


// 보관함 최대 중첩 개수 설정
bool Storage::setMaxStack(int value) {
    return inventory.setMaxStack(value);
}


// 보관함 내용 출력
void Storage::printAll() {
    inventory.printAll();
}
