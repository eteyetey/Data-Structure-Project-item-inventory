#pragma once

#include <vector>
#include <map>
#include "ItemLinkedList.h"

using namespace std;


class Inventory {
private:
    // 실제 아이템 저장 및 순서 관리
    ItemLinkedList items;

    // ID를 통해 같은 종류 아이템에 빠르게 접근
    map<int, vector<ItemNode*>> itemMap;

public:
    Inventory();

    // 아이템 추가
    void addItem(const Item& item);

    // 같은 ID 중 특정 아이템 하나 삭제
    bool removeItem(int id, int index);

    // 같은 ID 아이템 전부 삭제
    bool removeAllItems(int id);

    // 인덱스로 아이템 조회
    Item* getItem(int index);

    // 같은 ID를 가진 모든 아이템 조회
    vector<Item*> findItemsById(int id);

    // 같은 ID 중 특정 아이템 사용
    bool useItem(int id, int index);

    int getItemCount() const;
    bool isEmpty() const;

    void clear();
    void printAll();
};