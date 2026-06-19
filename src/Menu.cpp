#include "Menu.h"
#include "FileIO.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <sstream>

Menu::Menu(const std::string& dataPath) : dataFilePath(dataPath) {
    FileIO::loadFromFile(dataFilePath, manager);
}

void Menu::run() {
    int choice = 0;
    do {
        displayMainMenu();
        choice = getIntInput("Выберите действие", 0, 8);

        switch (choice) {
        case 1: addSubscriptionMenu(); break;
        case 2: editSubscriptionMenu(); break;
        case 3: deleteSubscriptionMenu(); break;
        case 4: searchMenu(); break;
        case 5: filterMenu(); break;
        case 6: sortMenu(); break;
        case 7: analyticsMenu(); break;
        case 8:
            if (FileIO::saveToFile(dataFilePath, manager)) {
                std::cout << "Данные сохранены в " << dataFilePath << std::endl;
            }
            std::cout << "До свидания!\n";
            break;
        case 0:
            std::cout << "Выход выполнен без сохранения.\n";
            break;
        default:
            break;
        }
    } while (choice != 0 && choice != 8);
}

void Menu::displayMainMenu() {
    std::cout << "\n========================================\n";
    std::cout << "   СИСТЕМА УПРАВЛЕНИЯ ПОДПИСКАМИ\n";
    std::cout << "========================================\n";
    std::cout << "1. Добавить подписку\n"
        << "2. Редактировать подписку\n"
        << "3. Удалить подписку\n"
        << "4. Поиск по названию\n"
        << "5. Фильтрация\n"
        << "6. Сортировка\n"
        << "7. Аналитика\n"
        << "8. Сохранить и выйти\n"
        << "0. Выход без сохранения\n";
    std::cout << "========================================\n";
    std::cout << "Всего подписок: " << manager.getSize() << "\n";
}

void Menu::displaySubscriptions(const std::vector<Subscription>& subs) {
    if (subs.empty()) {
        std::cout << "Подписки не найдены.\n";
        return;
    }

    std::cout << "\n--------------------------------------------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(5) << "ID"
        << std::setw(20) << "Название"
        << std::setw(15) << "Категория"
        << std::setw(15) << "Стоимость/мес"
        << std::setw(12) << "Цикл"
        << std::setw(15) << "Дата платежа"
        << std::setw(10) << "Статус" << "\n";
    std::cout << "--------------------------------------------------------------------------------------------------------\n";

    for (const auto& sub : subs) {
        std::cout << std::left << std::setw(5) << sub.getId()
            << std::setw(20) << sub.getServiceName()
            << std::setw(15) << sub.getCategoryString()
            << std::setw(15) << std::fixed << std::setprecision(2) << sub.getEffectiveMonthlyCost()
            << std::setw(12) << sub.getBillingCycleString()
            << std::setw(15) << sub.getNextPaymentDateString()
            << std::setw(10) << (sub.isActiveSubscription() ? "Активна" : "Неактивна") << "\n";
    }
    std::cout << "--------------------------------------------------------------------------------------------------------\n";
}

void Menu::addSubscriptionMenu() {
    std::cout << "\n--- ДОБАВЛЕНИЕ ПОДПИСКИ ---\n";

    std::string name = getInput("Название сервиса");
    if (name.empty()) {
        std::cout << "Ошибка: название не может быть пустым.\n";
        return;
    }

    std::cout << "Категория:\n"
        << "1. Развлечения\n2. Софт\n3. Обучение\n4. Коммунальные\n5. Другое\n";
    int catChoice = getIntInput("Выберите категорию", 1, 5);
    Category category = static_cast<Category>(catChoice - 1);

    double cost = getDoubleInput("Стоимость (в рублях за период)", 0.01);

    std::cout << "Цикл оплаты:\n"
        << "1. Ежемесячно\n2. Ежегодно\n3. Еженедельно\n4. Ежеквартально\n";
    int cycleChoice = getIntInput("Выберите цикл", 1, 4);
    BillingCycle cycle = static_cast<BillingCycle>(cycleChoice - 1);

    std::string dateStr = getInput("Дата следующего платежа (ГГГГ-ММ-ДД)");
    std::time_t nextDate = parseDate(dateStr);
    if (nextDate == -1) {
        std::cout << "Ошибка: неверный формат даты.\n";
        return;
    }

    Subscription newSub(manager.getNextId(), name, category, cost, cycle, nextDate, true);
    if (manager.addSubscription(newSub)) {
        manager.incrementNextId();
        std::cout << "Подписка успешно добавлена с ID: " << newSub.getId() << "\n";
    }
    else {
        std::cout << "Ошибка при добавлении подписки.\n";
    }
}

void Menu::editSubscriptionMenu() {
    std::cout << "\n--- РЕДАКТИРОВАНИЕ ПОДПИСКИ ---\n";
    int id = getIntInput("Введите ID подписки для редактирования", 1, 999999);

    Subscription* sub = manager.findSubscriptionById(id);
    if (!sub) {
        std::cout << "Подписка с ID " << id << " не найдена.\n";
        return;
    }

    std::cout << "Текущая информация:\n";
    displaySubscriptions({ *sub });

    std::string name = getInput("Новое название (оставьте пустым для сохранения)");
    if (!name.empty()) {
        sub->setServiceName(name);
    }

    std::cout << "Новая категория (0-пропустить, 1-Развлечения, 2-Софт, 3-Обучение, 4-Коммунальные, 5-Другое): ";
    int catChoice = getIntInput("", 0, 5);
    if (catChoice > 0) {
        sub->setCategory(static_cast<Category>(catChoice - 1));
    }

    double cost = getDoubleInput("Новая стоимость (0 для пропуска)", 0.0);
    if (cost > 0) {
        sub->setMonthlyCost(cost);
    }

    std::cout << "Новый цикл оплаты (0-пропустить, 1-месяц, 2-год, 3-неделя, 4-квартал): ";
    int cycleChoice = getIntInput("", 0, 4);
    if (cycleChoice > 0) {
        sub->setBillingCycle(static_cast<BillingCycle>(cycleChoice - 1));
    }

    std::string dateStr = getInput("Новая дата платежа (ГГГГ-ММ-ДД) (оставьте пустым для пропуска)");
    if (!dateStr.empty()) {
        std::time_t newDate = parseDate(dateStr);
        if (newDate != -1) {
            sub->setNextPaymentDate(newDate);
        }
    }

    std::cout << "Изменить статус? (1-Активна, 2-Неактивна, 0-Пропустить): ";
    int statusChoice = getIntInput("", 0, 2);
    if (statusChoice == 1) {
        sub->setActive(true);
    }
    else if (statusChoice == 2) {
        sub->setActive(false);
    }

    std::cout << "Параметры подписки обновлены.\n";
}

void Menu::deleteSubscriptionMenu() {
    std::cout << "\n--- УДАЛЕНИЕ ПОДПИСКИ ---\n"
        << "1. Удалить по ID\n"
        << "2. Удалить по названию\n";
    int choice = getIntInput("Выберите способ", 1, 2);

    if (choice == 1) {
        int id = getIntInput("Введите ID", 1, 999999);
        if (manager.deleteSubscriptionById(id)) {
            std::cout << "Подписка удалена.\n";
        }
        else {
            std::cout << "Подписка не найдена.\n";
        }
    }
    else {
        std::string name = getInput("Введите название сервиса");
        if (manager.deleteSubscriptionByName(name)) {
            std::cout << "Соответствующие подписки удалены.\n";
        }
        else {
            std::cout << "Подписки с таким названием не найдены.\n";
        }
    }
}

void Menu::searchMenu() {
    std::cout << "\n--- ПОИСК ПО НАЗВАНИЮ ---\n";
    std::string query = getInput("Введите название для поиска");
    auto results = manager.searchByName(query);
    displaySubscriptions(results);
}

void Menu::filterMenu() {
    std::cout << "\n--- ФИЛЬТРАЦИЯ ---\n"
        << "1. По категории\n"
        << "2. По диапазону стоимости\n"
        << "3. По диапазону дат платежа\n";
    int choice = getIntInput("Выберите тип фильтра", 1, 3);

    std::vector<Subscription> results;

    switch (choice) {
    case 1: {
        std::cout << "1. Развлечения\n2. Софт\n3. Обучение\n4. Коммунальные\n5. Другое\n";
        int catChoice = getIntInput("Выберите категорию", 1, 5);
        Category category = static_cast<Category>(catChoice - 1);
        results = manager.filterByCategory(category);
        break;
    }
    case 2: {
        double min = getDoubleInput("Минимальная стоимость", 0.0);
        double max = getDoubleInput("Максимальная стоимость", min);
        results = manager.filterByCostRange(min, max);
        break;
    }
    case 3: {
        std::string startStr = getInput("Начальная дата (ГГГГ-ММ-ДД)");
        std::string endStr = getInput("Конечная дата (ГГГГ-ММ-ДД)");
        std::time_t start = parseDate(startStr);
        std::time_t end = parseDate(endStr);
        if (start != -1 && end != -1) {
            results = manager.filterByDateRange(start, end);
        }
        break;
    }
    default:
        break;
    }

    displaySubscriptions(results);
}

void Menu::sortMenu() {
    std::cout << "\n--- СОРТИРОВКА ---\n"
        << "1. По стоимости (возрастание)\n"
        << "2. По стоимости (убывание)\n"
        << "3. По дате платежа\n"
        << "4. По названию\n";
    int choice = getIntInput("Выберите тип сортировки", 1, 4);

    switch (choice) {
    case 1: manager.sortByMonthlyCost(true); break;
    case 2: manager.sortByMonthlyCost(false); break;
    case 3: manager.sortByNextPaymentDate(); break;
    case 4: manager.sortByServiceName(); break;
    default: break;
    }

    std::cout << "Порядок сортировки успешно применен.\n";
    displaySubscriptions(manager.getAllSubscriptions());
}

void Menu::analyticsMenu() {
    std::cout << "\n--- АНАЛИТИКА ---\n";

    double total = manager.getTotalMonthlyExpenses();
    std::cout << "Суммарные ежемесячные расходы: " << std::fixed << std::setprecision(2) << total << " руб.\n";

    std::cout << "\nПодписки с оплатой в ближайшие 7 дней:\n";
    auto upcoming = manager.getUpcomingPayments(7);
    displaySubscriptions(upcoming);

    std::cout << "\nТоп-3 категории по суммарной стоимости:\n";
    auto topCategories = manager.getTopCategoriesByCost(3);
    int rank = 1;
    for (const auto& [catName, catCost] : topCategories) {
        std::cout << rank++ << ". " << catName << ": " << std::fixed << std::setprecision(2) << catCost << " руб.\n";
    }
}

std::time_t Menu::parseDate(const std::string& dateStr) {
    int year = 0, month = 0, day = 0;
    char dash1 = 0, dash2 = 0;
    std::stringstream ss(dateStr);

    // Ручной разбор формата ГГГГ-ММ-ДД
    if (!(ss >> year >> dash1 >> month >> dash2 >> day) || dash1 != '-' || dash2 != '-') {
        return -1;
    }

    // Базовая валидация диапазона значений
    if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
        return -1;
    }

    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_isdst = -1; // Даем системе возможность автоматически определить летнее время

    return std::mktime(&tm);
}


std::string Menu::getInput(const std::string& prompt) {
    std::cout << prompt << ": ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int Menu::getIntInput(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        if (!prompt.empty()) {
            std::cout << prompt << " (" << min << "-" << max << "): ";
        }
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка: некорректное число. Введите значение от " << min << " до " << max << "\n";
    }
}

double Menu::getDoubleInput(const std::string& prompt, double min) {
    double value;
    while (true) {
        std::cout << prompt << " (мин. " << min << "): ";
        if (std::cin >> value && value >= min) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка: введите числовое значение >= " << min << "\n";
    }
}