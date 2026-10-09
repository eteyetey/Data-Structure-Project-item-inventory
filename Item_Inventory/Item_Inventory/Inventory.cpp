
#include "Inventory.h"
#include <iostream>
#include <algorithm>

using namespace std;


// 생성자
Inventory::Inventory() {}


// 아이템 추가

AddResult Inventory::addItem(const Item& item) {

    int id = item.getId();
    int requested = item.getQuantity();

    //일단 성공0개 실패n개
    AddResult result = { 0, requested };

    // 잘못된 수량은 삽입하지 않음
    if (requested <= 0) {
        return result;
    }

    // 중첩 불가능한 아이템인 경우
    if (!item.isStackable()) {

        // 중첩 불가능한 아이템은 1개씩 삽입
        for (int i = 0; i < requested; i++) {

            //현재 연결리스트에 들어있는 아이템개수(정확히는 슬롯개수) 가 맥스를 초과할경우 넣는것을 멈춤
            if (items.getSize() >= maxSlots) {
                break;
            }

            //하나씩 넣는법 : 일단 추가하려는 아이템의 개수 1짜리 item 임시 객체를 생성하고 
            Item newItem = item;
            newItem.setQuantity(1);

            // 새로운 노드 추가
            ItemNode* newNode = items.add(newItem);

            // 맵에도 추가
            itemMap[id].push_back(newNode);

            //하나 추가 성공할때마다 성공이 1개씩 늘고 남은게 1개씩 줄음
            result.added++;
            result.remaining--;
        }

        //성공 몇개 실패 몇개 반환
        return result;
    }

    // 중첩 가능한 아이템인 경우
    auto it = itemMap.find(id);

    // 같은 ID의 아이템이 이미 존재하면 수량만 증가

    if (it != itemMap.end()) {

        //이미 존재하는 슬롯에 대해 일단 남는 공간이 있으면 집어넣고봄
        for (ItemNode* node : it->second) {

            //더이상 넣을게 없이 다넣었다면 정지
            if (result.remaining <= 0) {
                break;
            }

            int currentQuantity = node->data.getQuantity();

            // 이미 꽉 찬 스택은 건너뛰기
            if (currentQuantity >= maxStack) {
                continue;
            }

            int space = maxStack - currentQuantity;
            int amount = min(space, result.remaining);

            node->data.addQuantity(amount);

            result.added += amount;
            result.remaining -= amount;
        }
    }

    //남는슬롯은 이제 없다면 슬롯 개수가 초과되지않는한 슬롯을 새로 만듬
    while (result.remaining > 0 &&
        items.getSize() < maxSlots) {

        //남은 아이템 개수랑 슬롯 하나의 최대치중 적은거 만큼 채워햐함(남으면 또 다음 슬롯 만들면됨)
        int amount = min(maxStack, result.remaining);

        Item newItem = item;
        newItem.setQuantity(amount);

        // 새로운 노드 추가
        ItemNode* newNode = items.add(newItem);

        // 맵에도 추가
        itemMap[id].push_back(newNode);

        result.added += amount;
        result.remaining -= amount;
    }
    

    return result;
}




// 슬롯 단위로 추가(새로운 슬롯을 추가하는 느낌) 그러나 개수 초과분을 저장하지 않음(초과시 오류)
bool Inventory::addLoadedSlot(const Item& item) {

    // 인벤토리 슬롯 초과
    if (items.getSize() >= maxSlots) {
        return false;
    }

    int quantity = item.getQuantity();

    // 수량 검사
    if (quantity <= 0) {
        return false;
    }

    if (item.isStackable()) {
        if (quantity > maxStack) {
            return false;
        }
    }
    else if (quantity != 1) {
        return false;
    }

    // 새로운 노드 추가
    ItemNode* newNode = items.add(item);

    // 맵에도 추가
    itemMap[item.getId()].push_back(newNode);

    return true;
}

//인벤토리의 특정 인덱스 아이템 삭제
bool Inventory::removeItemAt(int index) {

    Item* item = items.get(index);

    if (item == nullptr) {
        return false;
    }

    int id = item->getId();

    auto it = itemMap.find(id);

    if (it == itemMap.end()) {
        return false;
    }

    // 해당 아이템의 노드 찾기
    for (int i = 0; i < static_cast<int>(it->second.size()); i++) {
        if (&it->second[i]->data == item) {

            ItemNode* node = it->second[i];

            items.removeNode(node);
            it->second.erase(it->second.begin() + i);

            if (it->second.empty()) {
                itemMap.erase(it);
            }

            return true;
        }
    }

    return false;
}


// 같은 ID 중 특정 아이템 하나 삭제
bool Inventory::removeItem(int id, int index) {

    auto it = itemMap.find(id);

    // 해당 ID가 없는 경우
    if (it == itemMap.end()) {
        return false;
    }

    vector<ItemNode*>& nodes = it->second;

    // 인덱스 범위 확인
    if (index < 0 || index >= nodes.size()) {
        return false;
    }


    ItemNode* target = nodes[index];

    // 연결리스트에서 삭제
    if (!items.removeNode(target)) {
        return false;
    }

    // 맵의 vector에서도 삭제
    nodes.erase(nodes.begin() + index);


    // 같은 ID 아이템이 더 이상 없으면 map에서도 제거
    if (nodes.empty()) {
        itemMap.erase(it);
    }

    return true;
}


// 같은 ID 아이템 전부 삭제
bool Inventory::removeAllItems(int id) {

    auto it = itemMap.find(id);

    // 해당 ID가 없는 경우
    if (it == itemMap.end()) {
        return false;
    }


    vector<ItemNode*>& nodes = it->second;


    // 같은 ID의 모든 노드 삭제
    for (ItemNode* node : nodes) {
        items.removeNode(node);
    }


    // 맵에서도 제거
    itemMap.erase(it);

    return true;
}


// 인덱스로 아이템 조회
Item* Inventory::getItem(int index) {
    return items.get(index);
}


// 같은 ID를 가진 모든 아이템 조회
vector<Item*> Inventory::findItemsById(int id) {

    vector<Item*> result;

    auto it = itemMap.find(id);

    // 해당 ID가 없는 경우
    if (it == itemMap.end()) {
        return result;
    }


    for (ItemNode* node : it->second) {
        result.push_back(&node->data);
    }


    return result;
}


bool Inventory::useItem(int id, int index) {

    auto it = itemMap.find(id);

    // 해당 ID가 없는 경우
    if (it == itemMap.end()) {
        return false;
    }

    vector<ItemNode*>& nodes = it->second;

    // 인덱스 범위 확인
    if (index < 0 || index >= nodes.size()) {
        return false;
    }


    ItemNode* node = nodes[index];
    Item& item = node->data;


    // 사용 불가능한 경우
    if (!item.isUsable()) {
        return false;
    }


    // 아이템 사용 실패
    if (!item.use()) {
        return false;
    }


    // 내구도형 아이템이 사용 후 파괴된 경우
    if (item.getType() == ItemType::Durability && item.isBroken()) {

        items.removeNode(node);

        nodes.erase(nodes.begin() + index);

        // 같은 ID 아이템이 더 이상 없으면 map에서도 제거
        if (nodes.empty()) {
            itemMap.erase(it);
        }

        return true;
    }


    // 소모품이면 수량 1 감소
    if (item.getType() == ItemType::Consumable) {

        item.removeQuantity(1);


        // 수량이 0이 되면 노드 자체 삭제
        if (item.getQuantity() == 0) {

            items.removeNode(node);

            nodes.erase(nodes.begin() + index);

            // 같은 ID 아이템이 더 이상 없으면 map에서도 제거
            if (nodes.empty()) {
                itemMap.erase(it);
            }
        }
    }


    return true;
}


// 현재 인벤토리 슬롯 개수 반환
int Inventory::getItemCount() const {
    return items.getSize();
}


// 인벤토리가 비어있는지 확인
bool Inventory::isEmpty() const {
    return items.isEmpty();
}


//남은 슬롯의 개수를 반환
int Inventory::getRemainingSlots() const {
    return maxSlots - items.getSize();
}



// 인벤토리 전체 비우기
void Inventory::clear() {

    // 맵에는 포인터만 있으므로 먼저 제거
    itemMap.clear();

    // 실제 노드 삭제
    items.clear();
}


// 인벤토리 전체 출력
void Inventory::printAll() {

    if (items.isEmpty()) {
        cout << "Inventory is empty." << endl;
        return;
    }


    for (int i = 0; i < items.getSize(); i++) {

        Item* item = items.get(i);

        if (item == nullptr) {
            continue;
        }


        cout << "[" << i << "] "
            << "ID: " << item->getId()
            << " | Name: " << item->getName();


        // 중첩 가능한 아이템
        if (item->isStackable()) {
            cout << " | Quantity: " << item->getQuantity();
        }

        // 내구도형 아이템
        if (item->getType() == ItemType::Durability) {
            cout << " | Durability: "
                << item->getDurability()
                << " / "
                << item->getMaxDurability();
        }


        cout << endl;
    }
}

// 최대 슬롯 개수 설정
bool Inventory::setMaxSlots(int value) {

    // 1 이상의 값만 허용
    if (value <= 0) {
        return false;
    }

    // 현재 사용 중인 슬롯보다 작게 설정 불가
    if (value < items.getSize()) {
        return false;
    }

    maxSlots = value;
    return true;
}


// 최대 중첩 수량 설정
bool Inventory::setMaxStack(int value) {

    if (value <= 0) {
        return false;
    }

    // 기존 아이템의 수량이 새로운 제한을 초과하는지 검사(90개 이미 들어있는데 최대치를 50개로 바꿀수는 없음)
    for (int i = 0; i < items.getSize(); i++) {

        Item* item = items.get(i);

        if (item != nullptr && item->isStackable()
            && item->getQuantity() > value) {
            return false;
        }
    }

    maxStack = value;
    return true;
}


// 최대 슬롯 개수 반환
int Inventory::getMaxSlots() const {
    return maxSlots;
}


// 최대 중첩 수량 반환
int Inventory::getMaxStack() const {
    return maxStack;
}
