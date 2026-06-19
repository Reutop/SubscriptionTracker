#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <ctime>
#include "SubscriptionManager.h"

class Menu {
public:
    explicit Menu(const std::string& dataPath);
    void run();

private:
    SubscriptionManager manager;
    std::string dataFilePath;

    void displayMainMenu();
    void displaySubscriptions(const std::vector<Subscription>& subs);

    void addSubscriptionMenu();
    void editSubscriptionMenu();
    void deleteSubscriptionMenu();

    void searchMenu();
    void filterMenu();
    void sortMenu();
    void analyticsMenu();

    std::time_t parseDate(const std::string& dateStr);
    std::string getInput(const std::string& prompt);
    int getIntInput(const std::string& prompt, int min, int max);
    double getDoubleInput(const std::string& prompt, double min);
};

#endif