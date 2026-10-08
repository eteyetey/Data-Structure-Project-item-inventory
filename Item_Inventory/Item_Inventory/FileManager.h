#pragma once

#include <string>
#include <vector>
#include "Item.h"
#include "Inventory.h"

using namespace std;

class FileManager {
public:

    // 아이템 원본 데이터 불러오기
    static bool loadItems(const string& fileName, vector<Item>& items);

    // 인벤토리 데이터 불러오기
    static bool loadInventory(const string& fileName, Inventory& inventory);

    // 인벤토리 데이터 저장하기
    static bool saveInventory(const string& fileName, Inventory& inventory);

};