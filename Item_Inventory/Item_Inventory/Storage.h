
#pragma once

#include "Inventory.h"

using namespace std;

class Storage {
private:
    //스토리지 정보
    int id;
    string name;

    //인벤토리 클래스를 재활용
    Inventory inventory;

    // 인벤과 스토리지간의 아이템 이동시 몇개성공 몇개실패를 알기위해 반환형을 AddResult 사용
    AddResult transfer(Inventory& from, Inventory& to, int index, int quantity);

public:
    Storage(int id = 0, const string& name = "");

    //넣기
    AddResult deposit(Inventory& player, int index, int quantity);

    // 빼기
    AddResult withdraw(Inventory& player, int index, int quantity);

    // 보관함 내부 Inventory 반환
    Inventory& getInventory();

    int getId() const;
    string getName() const;

    void setName(string newName);

    // 보관함 최대 용량 설정
    bool setMaxSlots(int value);
    bool setMaxStack(int value);

    // 보관함 전체 출력
    void printAll();
};
