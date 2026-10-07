#pragma once
#include <string>

using namespace std;

enum class ItemType {
    Consumable,     // 1회용 사용 아이템
    Durability,     // 내구도형 사용 아이템
    Permanent,      // 영구 사용가능 아이템
    Unusable        // 사용 불가형 아이템
};

class Item {
private:
    int id;                     // 아이템 고유 ID
    string name;                // 아이템 이름
    string category;            // 아이템 카테고리
    int rarity;                 // 아이템 희귀도(성급)

    ItemType type;              // 아이템 타입(사용 관련)

    int durability;             // 아이템 현재 내구도
    int maxDurability;          // 아이템 최대 내구도

    int quantity = 1;           // 아이템 수량

public:
    // 기본 생성자
    Item();

    // 현재 내구도가 최대치인 경우
    Item(int id, string name, string category,
        int rarity, ItemType type, int maxDurability = 0);

    // 현재 내구도를 따로 설정하고 싶은 경우
    Item(int id, string name, string category,
        int rarity, ItemType type,
        int durability, int maxDurability);

    // getter
    int getId() const;
    string getName() const;
    string getCategory() const;
    int getRarity() const;
    ItemType getType() const;
    int getDurability() const;
    int getMaxDurability() const;
    int getQuantity() const;

    // setter
    void setName(const string& name);
    void setCategory(const string& category);
    void setRarity(int rarity);
    void setQuantity(int quantity);

    // 수량 증가
    void addQuantity(int amount);

    // 수량 감소
    bool removeQuantity(int amount);

    // 아이템 중첩 가능 여부
    bool isStackable() const;

    // 아이템 사용
    bool use();

    // 아이템 사용 확인 메시지 출력
    void printUseMsg(string name);

    // 사용 가능한 아이템인지 확인
    bool isUsable() const;

    // 내구도가 모두 소모되었는지 확인
    bool isBroken() const;
};