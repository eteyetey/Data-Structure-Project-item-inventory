#pragma once

#include <vector>
#include "Item.h"

using namespace std;


// 아이템을 삽입, 삭제할 일이 빈번하므로 연결리스트 구조 사용
// 양방향 탐색을 위해 이중 연결리스트 사용
class ItemNode {
public:
    Item data;
    ItemNode* prev;
    ItemNode* next;

    ItemNode(const Item& item);
};


class ItemLinkedList {
private:
    ItemNode* head;
    ItemNode* tail;
    int size;

public:
    // 생성자 / 소멸자
    ItemLinkedList();
    ~ItemLinkedList();

    // 맨 뒤에 아이템 추가
    ItemNode* add(const Item& item);

    // 인덱스를 기준으로 아이템 삭제
    bool remove(int index);

    // 노드 포인터를 기준으로 삭제
    bool removeNode(ItemNode* node);

    // 인덱스를 기준으로 아이템 반환
    Item* get(int index);

    // 같은 ID를 가진 모든 아이템 노드 반환
    vector<ItemNode*> findById(int id);

    // 같은 ID를 가진 모든 아이템 삭제
    bool removeById(int id);

    // 현재 아이템 개수 반환
    int getSize() const;

    // 리스트가 비어있는지 확인
    bool isEmpty() const;

    // 전체 리스트 비우기
    void clear();
};