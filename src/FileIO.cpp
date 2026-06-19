#include "FileIO.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <filesystem>

bool FileIO::loadFromFile(const std::string& filename, SubscriptionManager& manager) {
    std::filesystem::path filePath(filename);
    std::filesystem::path dirPath = filePath.parent_path();

    if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);
        std::cout << "Создана директория: " << dirPath << std::endl;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Файл не найден: " << filename << std::endl;
        std::cerr << "Новый файл будет создан при автоматическом сохранении данных." << std::endl;
        return false;
    }

    manager.clear();
    std::string line;
    int maxId = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string idStr, serviceName, categoryStr, monthlyCostStr, billingCycleStr, nextPaymentDateStr, statusStr;

        std::getline(ss, idStr, ',');
        std::getline(ss, serviceName, ',');
        std::getline(ss, categoryStr, ',');
        std::getline(ss, monthlyCostStr, ',');
        std::getline(ss, billingCycleStr, ',');
        std::getline(ss, nextPaymentDateStr, ',');
        std::getline(ss, statusStr, ',');

        try {
            int id = std::stoi(idStr);
            double monthlyCost = std::stod(monthlyCostStr);

            // Удаляем символ возврата каретки '\r', если файл имеет Windows-окончания строк (CRLF)
            if (!statusStr.empty() && statusStr.back() == '\r') {
                statusStr.pop_back();
            }
            bool isActive = (statusStr == "active");

            // Надежный ручной разбор даты для совместимости со всеми компиляторами
            int year = 0, month = 0, day = 0;
            char dash1 = 0, dash2 = 0;
            std::stringstream dateStream(nextPaymentDateStr);
            if (!(dateStream >> year >> dash1 >> month >> dash2 >> day) || dash1 != '-' || dash2 != '-') {
                std::cerr << "Предупреждение: Неверный формат даты в файле: " << nextPaymentDateStr << std::endl;
                continue;
            }

            std::tm tm = {};
            tm.tm_year = year - 1900;
            tm.tm_mon = month - 1;
            tm.tm_mday = day;
            tm.tm_isdst = -1;

            std::time_t nextPaymentDate = std::mktime(&tm);
            if (nextPaymentDate == -1) {
                std::cerr << "Предупреждение: Некорректное календарное значение даты: " << nextPaymentDateStr << std::endl;
                continue;
            }

            Category category = Subscription::stringToCategory(categoryStr);
            BillingCycle billingCycle = Subscription::stringToBillingCycle(billingCycleStr);

            Subscription sub(id, serviceName, category, monthlyCost, billingCycle, nextPaymentDate, isActive);
            manager.addSubscription(sub);

            if (id > maxId) {
                maxId = id;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при разборе строки: \"" << line << "\" -> " << e.what() << std::endl;
        }
    }

    file.close();

    if (maxId > 0) {
        manager.setNextId(maxId + 1);
    }

    std::cout << "Успешно импортировано подписок: " << manager.getSize() << std::endl;
    return true;
}

bool FileIO::saveToFile(const std::string& filename, const SubscriptionManager& manager) {
    std::filesystem::path filePath(filename);
    std::filesystem::path dirPath = filePath.parent_path();

    if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);
        std::cout << "Создана директория: " << dirPath << std::endl;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка экспорта: не удалось открыть файл для записи " << filename << std::endl;
        return false;
    }

    for (const auto& sub : manager.getAllSubscriptions()) {
        file << sub.getId() << ","
            << sub.getServiceName() << ","
            << sub.getCategoryString() << ","
            << sub.getMonthlyCost() << ","
            << sub.getBillingCycleString() << ","
            << sub.getNextPaymentDateString() << ","
            << (sub.isActiveSubscription() ? "active" : "inactive") << "\n";
    }

    file.close();
    std::cout << "Успешно сохранено подписок: " << manager.getAllSubscriptions().size() << " в " << filename << std::endl;
    return true;
}