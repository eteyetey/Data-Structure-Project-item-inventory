#include "Inventory.h"
#include <iostream>

using namespace std;


// 생성자
Inventory::Inventory() {}


// 아이템 추가
void Inventory::addItem(const Item& item) {

    int id = item.getId();

    // 중첩 가능한 아이템인 경우
    if (item.isStackable()) {

        auto it = itemMap.find(id);

        // 같은 ID의 아이템이 이미 존재하면 수량만 증가
        if (it != itemMap.end() && !it->second.empty()) {

            ItemNode* node = it->second[0];

            node->data.addQuantity(item.getQuantity());

            return;
        }
    }


    // 새로운 노드 추가
    ItemNode* newNode = items.add(item);

    // 맵에도 추가
    itemMap[id].push_back(newNode);
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