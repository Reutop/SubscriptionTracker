#include <iostream>
#include <locale>
#include <clocale>
#include "Menu.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    std::setlocale(LC_ALL, "ru_RU.UTF-8");

    std::cout << "Загрузка системы управления подписками...\n";

    Menu appMenu("data/subscriptions.txt");
    appMenu.run();

    return 0;
}