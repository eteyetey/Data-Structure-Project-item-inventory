
#include <iostream>
#include <vector>

#include "Item.h"
#include "Inventory.h"
#include "FileManager.h"

using namespace std;

int main() {

    Inventory inventory;
    vector<Item> itemData;

    // 1. 아이템 원본 데이터 불러오기
    cout << "===== 1. Load Items =====" << endl;

    if (!FileManager::loadItems("items.txt", itemData)) {
        cout << "Failed to load items.txt" << endl;
        return 1;
    }

    cout << "Loaded item types: " << itemData.size() << endl;

    for (const Item& item : itemData) {
        cout << item.getId() << " | " << item.getName() << endl;
    }


    // 2. 인벤토리 데이터 불러오기
    cout << "\n===== 2. Load Inventory =====" << endl;

    if (!FileManager::loadInventory("inventory.txt", inventory)) {
        cout << "Failed to load inventory.txt" << endl;
        return 1;
    }

    inventory.printAll();

    int tmpid, tmpindex;

    while (true) {
        cin >> tmpid;
        cin >> tmpindex;

        inventory.useItem(tmpid, tmpindex);

        inventory.printAll();
    }
    

    
}
