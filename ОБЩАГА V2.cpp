// ОБЩАГА V2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <cmath>

// ---------------- Структуры ----------------
typedef struct Resident {
    char name[50];
    struct Resident* prev;
    struct Resident* next;
} Resident;

typedef struct Room {
    int number;
    Resident* residents;
    struct Room* next;
} Room;

typedef struct Floor {
    int number;
    int total_places;
    int free_places;
    Room* rooms;
    struct Floor* prev;
    struct Floor* next;
} Floor;

char* my_strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0'; // не забываем про завершающий нуль
    return original_dest;
}



// ---------------- Функции ----------------
Resident* create_resident(const char* name) {
    Resident* r = (Resident*)malloc(sizeof(Resident));
    my_strcpy(r->name, name);
    r->prev = r->next = r;
    return r;
}

void add_resident(Room* room, const char* name) {
    if (room->residents) {
        printf("Комната %d уже занята.\n", room->number);
        return;
    }
    Resident* new_r = create_resident(name);
    room->residents = new_r;
}

void remove_resident(Room* room, const char* name) {
    if (!room->residents) return;
    Resident* current = room->residents;
    if (strcmp(current->name, name) == 0) {
        room->residents = NULL;
        free(current);
    }
}

Room* create_room(int number) {
    Room* room = (Room*)malloc(sizeof(Room));
    room->number = number;
    room->residents = NULL;
    room->next = NULL;
    return room;
}

void add_room_to_floor(Floor* floor, int number) {
    Room* cur = floor->rooms;
    while (cur) {
        if (cur->number == number) {
            printf("Комната %d уже существует на этаже %d.\n", number, floor->number);
            return;
        }
        cur = cur->next;
    }
    Room* new_room = create_room(number);
    if (!floor->rooms) {
        floor->rooms = new_room;
    }
    else {
        cur = floor->rooms;
        while (cur->next) cur = cur->next;
        cur->next = new_room;
    }
}

Floor* create_floor(int number, int room_count) {
    Floor* floor = (Floor*)malloc(sizeof(Floor));
    floor->number = number;
    floor->total_places = room_count;
    floor->free_places = room_count;
    floor->rooms = NULL;
    floor->prev = floor->next = NULL;
    int size = 0;
    if (room_count >= 100) {
        size = 1;
        int temp = room_count + 1;
        while (temp > 0) {
            temp /= 10;
            size *= 10;
        }
    }

    else{
       size = 100; 
    }
    
    for (int i = 0; i < room_count; ++i) {
        int room_number = number * size + i + 1;
        add_room_to_floor(floor, room_number);
    }
    return floor;
}

void add_floor(Floor** head, int number, int room_count) {
    Floor* current = *head;
    while (current) {
        if (current->number == number) {
            printf("Этаж %d уже существует.\n", number);
            return;
        }
        current = current->next;
    }
    Floor* new_floor = create_floor(number, room_count);
    if (!(*head)) {
        *head = new_floor;
    }
    else {
        Floor* last = *head;
        while (last->next) last = last->next;
        last->next = new_floor;
        new_floor->prev = last;
    }
    printf("Этаж %d с %d комнатами добавлен.\n", number, room_count);
}


void delete_floor(Floor** head, int number) {
    Floor* current = *head;
    while (current && current->number != number) {
        current = current->next;
    }

    if (!current) {
        printf("Этаж %d не найден.\n", number);
        return;
    }

    // Удалить всех жильцов и комнаты
    Room* room = current->rooms;
    while (room) {
        Resident* res = room->residents;
        if (res) {
            Resident* start = res;
            do {
                Resident* next = res->next;
                free(res);
                res = next;
            } while (res != start);
        }
        Room* temp_room = room;
        room = room->next;
        free(temp_room);
    }

    // Обновление связей в списке этажей
    if (current->prev) {
        current->prev->next = current->next;
    }
    else {
        *head = current->next;
    }
    if (current->next) {
        current->next->prev = current->prev;
    }

    free(current);
    printf("Этаж %d удалён.\n", number);
}


void delete_dormitory(Floor** head) {
    while (*head) {
        delete_floor(head, (*head)->number);
    }
    printf("Все этажи удалены. Общежитие очищено.\n");
}

Floor* find_floor(Floor* head, int number) {
    while (head) {
        if (head->number == number) return head;
        head = head->next;
    }
    return NULL;
}

Room* find_room(Floor* floor, int number) {
    Room* r = floor->rooms;
    while (r) {
        if (r->number == number) return r;
        r = r->next;
    }
    return NULL;
}


void find_free_place(Floor* head) {
    while (head) {
        printf("Этаж %d: %d свободных мест\n", head->number, head->free_places);
        Room* room = head->rooms;
        printf("+------+---------------+\n");
        printf("| ЭТАЖ | НОМЕРА КОМНАТ |\n");
        printf("+------+---------------+\n");
        int found = 0;
        while (room) {
            if (!room->residents) {
                printf("|  %d   |       %d     |\n", head->number, room->number);
                printf("+------+---------------+\n");
                found = 1;
            }
            room = room->next;
        }
        if (!found) printf("нет");
        printf("\n");
        head = head->next;
    }
}

void settle(Floor* floor, int room_number, const char* name) {
    if (floor->free_places <= 0) {
        printf("Нет свободных мест на этаже %d\n", floor->number);
        return;
    }
    Room* room = find_room(floor, room_number);
    if (!room) {
        printf("Комната %d не найдена.\n", room_number);
        return;
    }
    if (room->residents) {
        printf("Комната %d уже занята.\n", room_number);
        return;
    }
    add_resident(room, name);
    floor->free_places--;
    printf("%s заселён(а) в комнату %d на этаже %d\n", name, room_number, floor->number);
}

void add_extra_room(Floor* floor, int custom_room_number, int floor_num) {
    
    Floor* f = find_floor(floor, floor_num);
    int example_room_number = f->rooms->number;
    printf("%d", example_room_number);

    // Проверка на существование комнаты с таким номером
    Room* existing = find_room(floor, custom_room_number);
    if (existing) {
        printf("Комната с номером %d уже существует на этаже %d.\n", custom_room_number, floor->number);
        return;
    }

    // Добавление комнаты
    add_room_to_floor(floor, example_room_number - 1 + custom_room_number);
    floor->total_places++;
    floor->free_places++;
    printf("Комната %d успешно добавлена на этаж %d.\n", example_room_number - 1 + custom_room_number, floor->number);
}

void move_resident(Floor* floor, int from_room_num, int to_room_num) {
    Room* from = find_room(floor, from_room_num);
    Room* to = find_room(floor, to_room_num);
    if (!from || !to) {
        printf("Одна из комнат не найдена.\n");
        return;
    }
    if (!from->residents) {
        printf("В комнате %d нет жильца для перевода.\n", from_room_num);
        return;
    }
    if (to->residents) {
        printf("Комната %d уже занята.\n", to_room_num);
        return;
    }
    to->residents = from->residents;
    from->residents = NULL;
    printf("Жилец переведён из комнаты %d в комнату %d на этаже %d\n", from_room_num, to_room_num, floor->number);
}

void list_all_residents(Floor* head) {
    printf("\n==== Список всех жильцов ====\n");
    printf("+------+---------------+-------------------+\n");
    printf("| ЭТАЖ | НОМЕР КОМНАТЫ |     ИМЯ ЖЕЛЬЦА    |\n");
    printf("+------+---------------+-------------------+\n");
    while (head) {
        Room* room = head->rooms;
        while (room) {
            if (room->residents) {

                printf("|  %d   |      %d      |         %s        \n", head->number, room->number, room->residents->name);
            }
            room = room->next;
        }
        
        head = head->next;
    }
    printf("+------+---------------+-------------------+\n");
}

void menu() {
    printf("\n=== Меню ===\n");
    printf("1. Добавить этаж с комнатами\n");
    printf("2. Заселить жильца\n");
    printf("3. РЕЖИМ АДМИНИСТРИРОВАНИЯ\n");
    printf("4. Найти свободные места\n");
    printf("5. Вывести всех жильцов\n");
    printf("6. Перевести жильца в другую комнату\n");
    printf("0. Выход\n");
    printf("Выберите действие: ");
}


void delete_room(Floor* floor, int room_number) {
    if (!floor) return;
    Room* current = floor->rooms;
    Room* prev = NULL;

    while (current && current->number != room_number) {
        prev = current;
        current = current->next;
    }

    if (!current) {
        printf("Комната %d не найдена на этаже %d.\n", room_number, floor->number);
        return;
    }

    Resident* res = current->residents;
    if (res) {
        Resident* start = res;
        do {
            Resident* next = res->next;
            free(res);
            res = next;
        } while (res != start);
    }

    if (prev) {
        prev->next = current->next;
    }
    else {
        floor->rooms = current->next;
    }
    free(current);
    floor->total_places--;
    floor->free_places--;

    printf("Комната %d удалена с этажа %d.\n", room_number, floor->number);
}

void delete_menu(Floor** dorm) {
    int del_choice,custom_room_number;
    int floor_num, room_num;

    do {
        printf("\n======== РЕЖИМ АДМИНИСТРИРОВАНИЯ ========\n");
        printf("1. Удаление комнаты\n");
        printf("2. Удаление этажа\n");
        printf("3. Удаление всего общежития\n");
        printf("4. Добавление комнаты\n");
        printf("0. Назад\n");
        printf("Выбор: ");
        scanf_s("%d", &del_choice);

        switch (del_choice) {
        case 1:
            printf("Введите номер этажа и номер комнаты: ");
            scanf_s("%d %d", &floor_num, &room_num);
            {
                Floor* f = find_floor(*dorm, floor_num);
                if (f) delete_room(f, room_num);
                else printf("Этаж не найден.\n");
            }
            break;
        case 2:
            printf("Введите номер этажа для удаления: ");
            scanf_s("%d", &floor_num);
            delete_floor(dorm, floor_num);
            break;
        case 3:
            delete_dormitory(dorm);
            printf("Общежитие удалено.\n");
            break;
        case 4:
            int floor_num, room_num;
            printf("Введите номер этажа и номер новой комнаты: ");
            scanf_s("%d %d", &floor_num, &room_num);  // scanf_s можно заменить, если не используете MSVC
            {
                Floor* f = find_floor(*dorm, floor_num);
                if (f) {
                    add_extra_room(f, room_num,floor_num);
                }
                else {
                    printf("Этаж не найден.\n");
                }
            }
            break;
        case 0:
            break;
        default:
            printf("Неверный выбор.\n");
        }
    } while (del_choice != 0);
}


// Функция обработки пользовательского ввода
void process_user_input(Floor** dorm) {
    int choice;
    do {
        menu();
        scanf_s("%d", &choice);
        int floor_num, room_count, room_num, to_room;
        char name[50];
        getchar();

        switch (choice) {
        case 1:
            printf("Введите номер этажа и количество комнат: ");
            scanf_s("%d %d", &floor_num, &room_count);
            add_floor(dorm, floor_num, room_count);
            break;
        case 2:
            printf("Введите номер этажа и номер комнаты: ");
            scanf_s("%d %d", &floor_num, &room_num);
            getchar();
            printf("Введите имя жильца: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            {
                Floor* f = find_floor(*dorm, floor_num);
                if (f) settle(f, room_num, name);
                else printf("Этаж не найден.\n");
            }
            break;
        case 3:
            delete_menu(dorm);
            break;
        case 4:
            find_free_place(*dorm);
            break;
        case 5:
            list_all_residents(*dorm);
            break;
        case 6:
            printf("Введите номер этажа, номер текущей комнаты и номер новой комнаты: ");
            scanf_s("%d %d %d", &floor_num, &room_num, &to_room);
            {
                Floor* f = find_floor(*dorm, floor_num);
                if (f) move_resident(f, room_num, to_room);
                else printf("Этаж не найден.\n");
            }
            break;
        case 0:
            delete_dormitory(dorm);
            printf("Выход.\n");
            break;
        default:
            printf("Неверный выбор.\n");
        }
    } while (choice != 0);
}



int main() {
    setlocale(LC_ALL, "Rus");
    Floor* dorm = NULL;
    process_user_input(&dorm);
    return 0;
 }


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
