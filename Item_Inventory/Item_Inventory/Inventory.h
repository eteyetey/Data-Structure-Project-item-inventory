
#pragma once

#include <vector>
#include <map>
#include "ItemLinkedList.h"

using namespace std;


//넣을 수 있는 아이템 수량이 정해져있기 때문에 꽉차서 못넣는 경우가 생길것을 대비해 반환형을 만든것임.
struct AddResult {
    int added;       // 실제 추가된 수량
    int remaining;   // 추가하지 못한 수량
};


class Inventory {
private:
    // 실제 아이템 저장 및 순서 관리
    ItemLinkedList items;

    // ID를 통해 같은 종류 아이템에 빠르게 접근
    map<int, vector<ItemNode*>> itemMap;

    int maxSlots = 100;
    int maxStack = 100;


public:
    Inventory();

    //최대 슬롯개수, 최대 중첩개수
  


    // 아이템 추가(기존 void형이였지만, 아이템 개수제한이 넘어가면 전부 넣을 수 없기때문에 얼마나 못넣는지 알려줘야해서 반환형을 변경함)
    AddResult addItem(const Item& item);


    //슬롯 단위로 아이템을 추가
    bool addLoadedSlot(const Item& item);

    //인벤토리의 특정 슬롯 아이템 삭제
    bool removeItemAt(int index);

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

    //남은 슬롯 개수
    int getRemainingSlots() const;
    

    void clear();
    void printAll();

    // 최대 슬롯 및 중첩 수량 설정
    bool setMaxSlots(int value);
    bool setMaxStack(int value);

    // 현재 설정값 반환
    int getMaxSlots() const;
    int getMaxStack() const;
};
