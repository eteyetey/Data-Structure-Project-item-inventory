#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Item.h"

using namespace std;


// 문자열을 ItemType으로 변환
ItemType stringToItemType(const string& type) {

    if (type == "Consumable") {
        return ItemType::Consumable;
    }
    else if (type == "Durability") {
        return ItemType::Durability;
    }
    else if (type == "Permanent") {
        return ItemType::Permanent;
    }
    else {
        return ItemType::Unusable;
    }
}


// ItemType을 문자열로 변환
string itemTypeToString(ItemType type) {

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


int main() {

    // 아이템 정보 파일 열기
    ifstream file("items.txt");

    // 파일을 열지 못한 경우
    if (!file.is_open()) {
        cout << "Failed to open items.txt" << endl;
        return 1;
    }

    string line;

    // 첫 번째 헤더 줄 건너뛰기
    getline(file, line);


    // 파일에서 내구도형 아이템 하나 찾기
    while (getline(file, line)) {

        // 빈 줄은 무시
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);

        string id;
        string name;
        string category;
        string rarity;
        string type;
        string maxDurability;


        // | 기준으로 데이터 분리
        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, category, '|');
        getline(ss, rarity, '|');
        getline(ss, type, '|');
        getline(ss, maxDurability, '|');


        ItemType itemType = stringToItemType(type);


        // 내구도형 아이템만 테스트
        if (itemType == ItemType::Durability) {

            int maxDurabilityValue = stoi(maxDurability);

            // 테스트용 현재 내구도
            int currentDurability = 35;


            // 파일에서 읽은 아이템 정보를 사용하고
            // 현재 내구도만 따로 지정하여 객체 생성
            Item item(
                stoi(id),
                name,
                category,
                stoi(rarity),
                itemType,
                currentDurability,
                maxDurabilityValue
            );


            // 생성된 아이템 정보 출력
            cout << "Durability Item Load Test" << endl;
            cout << "ID : " << item.getId() << endl;
            cout << "Name : " << item.getName() << endl;
            cout << "Category : " << item.getCategory() << endl;
            cout << "Rarity : " << item.getRarity() << endl;
            cout << "Type : " << itemTypeToString(item.getType()) << endl;

            cout << "Durability : "
                << item.getDurability()
                << " / "
                << item.getMaxDurability()
                << endl;


            // 하나만 테스트할 것이므로 반복 종료
            break;
        }
    }


    // 파일 닫기
    file.close();

    return 0;
}