
#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;


// 파일에는 타입이 문자열로 쓰여있으니까 이걸 ItemType으로 변환하는 기능이 필요함
static bool stringToItemType(const string& type, ItemType& result) {

    if (type == "Consumable") {
        result = ItemType::Consumable;
    }
    else if (type == "Durability") {
        result = ItemType::Durability;
    }
    else if (type == "Permanent") {
        result = ItemType::Permanent;
    }
    else if (type == "Unusable") {
        result = ItemType::Unusable;
    }
    else {
        return false;
    }

    return true;
}


// 반대로 파일에 저장할때는 ItemType을 문자열로 바꿔서 저장해야함
static string itemTypeToString(ItemType type) {

    switch (type) {
    case ItemType::Consumable:
        return "Consumable";
    case ItemType::Durability:
        return "Durability";
    case ItemType::Permanent:
        return "Permanent";
    case ItemType::Unusable:
        return "Unusable";
    }

    return "Unusable";
}


// 문자열을 정수로 변환
static bool parseInt(const string& text, int& result) {

    try {
        size_t pos = 0;
        int value = stoi(text, &pos);

        if (pos != text.size()) {
            return false;
        }

        result = value;
        return true;
    }
    catch (const exception&) {
        return false;
    }
}


// 문자열을 | 기준으로 분리해서 벡터로 반환
static vector<string> splitLine(const string& line) {

    vector<string> result;
    string field;
    stringstream ss(line);

    while (getline(ss, field, '|')) {
        result.push_back(field);
    }

    // 마지막 빈 필드 처리
    if (!line.empty() && line.back() == '|') {
        result.push_back("");
    }

    return result;
}


// 아이템 원본 데이터 불러오기
bool FileManager::loadItems(const string& fileName, vector<Item>& items) {

    ifstream file(fileName);

    if (!file.is_open()) {
        return false;
    }

    string line;
    //로딩된 아이템을 잠깐 담아놓을 용도(모두 정상로딩시 items에 넣을 예정)
    vector<Item> loadedItems;

    // 맨 처음줄 읽기(범주라서 필요없음)
    if (!getline(file, line)) {
        return false;
    }

    while (getline(file, line)) {

        if (line.empty()) {
            continue;
        }

        vector<string> data = splitLine(line);

        // 원본 데이터는 6개 필드
        if (data.size() != 6) {
            return false;
        }

        int id, rarity, maxDurability;
        ItemType type;

        //참조를 통해 각 변수들에 값을 넣음과 동시에 값이 올바르게 변환되지 않을 경우 정지
        if (!parseInt(data[0], id) ||
            !parseInt(data[3], rarity) ||
            !parseInt(data[5], maxDurability) ||
            !stringToItemType(data[4], type)) {
            return false;
        }

        //잘못된 값이 들어있는 경우 멈춤
        if (id < 0 || rarity < 0 || maxDurability < 0) {
            return false;
        }

        Item item(id, data[1], data[2],
            rarity, type, maxDurability);

        loadedItems.push_back(item);
    }

    if (file.bad()) {
        return false;
    }

    // 정상적으로 읽었을 때만 기존 데이터 교체
    items.swap(loadedItems);

    return true;
}


// 인벤토리 데이터 불러오기
bool FileManager::loadInventory(const string& fileName, Inventory& inventory) {

    ifstream file(fileName);

    if (!file.is_open()) {
        return false;
    }

    string line;
    vector<Item> loadedItems;

    // 헤더 읽기
    if (!getline(file, line)) {
        return false;
    }

    while (getline(file, line)) {

        if (line.empty()) {
            continue;
        }

        vector<string> data = splitLine(line);

        // 저장 데이터는 8개 필드
        if (data.size() != 8) {
            return false;
        }

        int id, rarity, durability, maxDurability, quantity;
        ItemType type;

        if (!parseInt(data[0], id) ||
            !parseInt(data[3], rarity) ||
            !parseInt(data[5], durability) ||
            !parseInt(data[6], maxDurability) ||
            !parseInt(data[7], quantity) ||
            !stringToItemType(data[4], type)) {
            return false;
        }

        if (id < 0 || rarity < 0 || maxDurability < 0 ||
            quantity <= 0) {
            return false;
        }

        if (type == ItemType::Durability) {
            if (durability < 0 || durability > maxDurability ||
                quantity != 1) {
                return false;
            }
        }
        else if (durability != 0 || maxDurability != 0) {
            return false;
        }

        // 중첩 가능한 아이템은 한 슬롯에 최대 100개
        if (type != ItemType::Durability &&
            quantity > Inventory::MAX_STACK) {
            return false;
        }

        // 파일에 기록된 슬롯이 100개를 초과하면 오버플로
        if (loadedItems.size() >= Inventory::MAX_SLOTS) {
            return false;
        }


        Item item(id, data[1], data[2],
            rarity, type, durability, maxDurability);

        item.setQuantity(quantity);
        loadedItems.push_back(item);
    }

    if (file.bad()) {
        return false;
    }


    inventory.clear();


    //인벤토리에 로딩된 아이템 슬롯단위로 추가
    for (const Item& item : loadedItems) {
        inventory.addLoadedSlot(item);
    }


    return true;
}


// 인벤토리 데이터 내보내기
bool FileManager::saveInventory(const string& fileName, Inventory& inventory) {

    ofstream file(fileName);

    if (!file.is_open()) {
        return false;
    }

    // 파일 헤더 작성
    file << "ID|NAME|CATEGORY|RARITY|TYPE|DURABILITY|MAX_DURABILITY|QUANTITY\n";

    //현재 인벤토리에 들어있는 아이템 개수만큼 반복하며 하나씩 집어넣어
    for (int i = 0; i < inventory.getItemCount(); i++) {

        Item* item = inventory.getItem(i);

        if (item == nullptr) {
            return false;
        }

        // 구분자가 데이터에 포함되면 저장 형식이 깨짐
        if (item->getName().find('|') != string::npos ||
            item->getCategory().find('|') != string::npos ||
            item->getName().find('\n') != string::npos ||
            item->getCategory().find('\n') != string::npos) {
            return false;
        }
        //형식에 맞게 작성
        file << item->getId() << "|"
            << item->getName() << "|"
            << item->getCategory() << "|"
            << item->getRarity() << "|"
            << itemTypeToString(item->getType()) << "|"
            << item->getDurability() << "|"
            << item->getMaxDurability() << "|"
            << item->getQuantity() << "\n";

        if (!file) {
            return false;
        }
    }

    file.close();

    return !file.fail();
}