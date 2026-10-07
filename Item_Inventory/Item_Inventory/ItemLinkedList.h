#pragma once

#include "Item.h"

//아이템을 삽입, 삭제할일이 빈번, 연결리스트 구조가 적합. 또한 단일 연결리스트보다 탐색이 효율적인 이중연결리스트 사용.
//Item클래스 전용이기 때문에 자료형으로 인한 충돌을 방지할 수 있음.
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
    void add(const Item& item);

    // 인덱스를 기준으로 아이템 삭제
    bool remove(int index);

    // 인덱스를 기준으로 아이템 반환
    Item* get(int index);

    // 아이템 ID로 검색
    Item* findById(int id);

    // 현재 아이템 개수 반환
    int getSize() const;

    // 리스트가 비어있는지 확인
    bool isEmpty() const;

    // 전체 리스트 비우기
    void clear();
};