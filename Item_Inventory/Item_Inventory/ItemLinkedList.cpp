#include "ItemLinkedList.h"


// ItemNode 생성자
ItemNode::ItemNode(const Item& item) {
    data = item;
    prev = nullptr;
    next = nullptr;
}


// ItemLinkedList 생성자
ItemLinkedList::ItemLinkedList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}


// ItemLinkedList 소멸자
ItemLinkedList::~ItemLinkedList() {
    clear();
}


// 맨 뒤에 아이템 추가
void ItemLinkedList::add(const Item& item) {

    ItemNode* newNode = new ItemNode(item);

    // 리스트가 비어있는 경우
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    }

    // 기존 노드가 있는 경우
    else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }

    size++;
}


// 인덱스를 기준으로 아이템 삭제
bool ItemLinkedList::remove(int index) {

    // 인텍스 범위 초과 방지
    if (index < 0 || index >= size) {
        return false;
    }

    ItemNode* target;


    // 이중연결리스트의 장점 살리기, 앞이나 뒤에서 더 가까운쪽부터 탐색
    if (index < size / 2) {

        target = head;

        for (int i = 0; i < index; i++) {
            target = target->next;
        }
    }else {

        target = tail;

        for (int i = size - 1; i > index; i--) {
            target = target->prev;
        }
    }


    // 첫 노드인지 아닌지
    if (target->prev != nullptr) {
        target->prev->next = target->next;
    }else {
        head = target->next;
    }
    // 마지막 노드인지 아닌지
    if (target->next != nullptr) {
        target->next->prev = target->prev;
    }else {
        tail = target->prev;
    }


    delete target;
    size--;

    return true;
}


// 인덱스를 기준으로 아이템 반환
Item* ItemLinkedList::get(int index) {

    // 잘못된 인덱스
    if (index < 0 || index >= size) {
        return nullptr;
    }

    ItemNode* current;


    // 이중연결리스트의 장점 살리기, 앞이나 뒤에서 더 가까운쪽부터 탐색
    if (index < size / 2) {

        current = head;

        for (int i = 0; i < index; i++) {
            current = current->next;
        }
    } else {

        current = tail;

        for (int i = size - 1; i > index; i--) {
            current = current->prev;
        }
    }


    return &current->data;//포인터니까 꼭 주소로 반환!
}


// 아이템 ID로 검색
Item* ItemLinkedList::findById(int id) {

    ItemNode* current = head;

    while (current != nullptr) {

        if (current->data.getId() == id) {
            return &current->data;
        }

        current = current->next;
    }
    //못찾으면 NULL 반환
    return nullptr;
}


// 현재 아이템 개수 반환
int ItemLinkedList::getSize() const {
    return size;
}


// 리스트가 비어있는지 확인
bool ItemLinkedList::isEmpty() const {
    return size == 0;
}


// 전체 리스트 비우기
void ItemLinkedList::clear() {

    ItemNode* current = head;

    while (current != nullptr) {

        ItemNode* nextNode = current->next;

        delete current;

        current = nextNode;
    }

    head = nullptr;
    tail = nullptr;
    size = 0;
}