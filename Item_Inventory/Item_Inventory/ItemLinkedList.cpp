#include "ItemLinkedList.h"
#include <algorithm>

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


// 맨 뒤에 아이템 추가(추후에 맵 자료구조와 연결하기 위해 void가 아닌 ItemNode 포인터를 반환함)
ItemNode* ItemLinkedList::add(const Item& item) {

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
    return newNode;
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

bool ItemLinkedList::removeNode(ItemNode* node) {

    // 노드가 NULL이면 삭제불가
    if (node == nullptr) {
        return false;
    }

    // 첫 노드가 아닌 경우
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    }
    else {
        head = node->next;
    }

    //막노드가 아닌 경우
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    }
    else {
        tail = node->prev;
    }

    delete node;
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


// 같은 ID를 가진 모든 아이템 노드 반환
vector<ItemNode*> ItemLinkedList::findById(int id) {

    vector<ItemNode*> result;

    ItemNode* current = head;

    while (current != nullptr) {

        if (current->data.getId() == id) {
            result.push_back(current);
        }

        current = current->next;
    }

    return result;
}

// 같은 ID를 가진 모든 아이템 삭제
bool ItemLinkedList::removeById(int id) {

    ItemNode* current = head;

    bool removed = false;

    while (current != nullptr) {

        // 삭제 후 이동할 다음 노드 미리 저장
        ItemNode* nextNode = current->next;


        if (current->data.getId() == id) {

            // 첫 노드가 아닌 경우
            if (current->prev != nullptr) {
                current->prev->next = current->next;
            }
            else {
                head = current->next;
            }


            // 마지막 노드가 아닌 경우
            if (current->next != nullptr) {
                current->next->prev = current->prev;
            }
            else {
                tail = current->prev;
            }


            delete current;
            size--;

            removed = true;
        }

        current = nextNode;
    }

    return removed;
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


// 선택한 기준으로 연결리스트 정렬
void ItemLinkedList::sortItems(SortField field, bool ascending) {
    if (size < 2) {
        return;
    }

    // 노드를 삭제하지 않고 연결 순서만 바꿀 예정
    vector<ItemNode*> nodes;

    ItemNode* current = head;

    while (current != nullptr) {
        nodes.push_back(current);
        current = current->next;
    }

    // 선택한 기준에 따라 두 아이템 비교
    auto compare = [field, ascending](ItemNode* a, ItemNode* b) {
        const Item& first = a->data;
        const Item& second = b->data;

        switch (field) {
        case SortField::Id:
            return ascending
                ? first.getId() < second.getId()
                : first.getId() > second.getId();

        case SortField::Name:
            return ascending
                ? first.getName() < second.getName()
                : first.getName() > second.getName();

        case SortField::Quantity:
            return ascending
                ? first.getQuantity() < second.getQuantity()
                : first.getQuantity() > second.getQuantity();

        case SortField::AddedTime:
            return ascending
                ? first.getAddedTime() < second.getAddedTime()
                : first.getAddedTime() > second.getAddedTime();

        case SortField::Rarity:
            return ascending
                ? first.getRarity() < second.getRarity()
                : first.getRarity() > second.getRarity();
        }

        return false;
        };

    // 값이 같은 아이템들의 기존 순서는 유지
    stable_sort(nodes.begin(), nodes.end(), compare);

    // 정렬 결과에 맞게 노드 연결을 다시 설정
    head = nodes.front();
    tail = nodes.back();

    for (int i = 0; i < size; i++) {
        nodes[i]->prev = i == 0 ? nullptr : nodes[i - 1];
        nodes[i]->next = i == size - 1 ? nullptr : nodes[i + 1];
    }
}
