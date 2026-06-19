#ifndef SUBSCRIPTION_MANAGER_H
#define SUBSCRIPTION_MANAGER_H

#include <vector>
#include <string>
#include <map>
#include <ctime>
#include "Subscription.h"

class SubscriptionManager {
private:
    std::vector<Subscription> subscriptions;
    int nextId;

public:
    SubscriptionManager();

    bool addSubscription(const Subscription& subscription);
    bool editSubscription(int id, const Subscription& updatedSubscription);
    bool deleteSubscriptionById(int id);
    bool deleteSubscriptionByName(const std::string& name);

    std::vector<Subscription> searchByName(const std::string& query) const;
    std::vector<Subscription> filterByCategory(Category category) const;
    std::vector<Subscription> filterByCostRange(double min, double max) const;
    std::vector<Subscription> filterByDateRange(std::time_t start, std::time_t end) const;

    void sortByMonthlyCost(bool ascending = true);
    void sortByNextPaymentDate();
    void sortByServiceName();
    double getTotalMonthlyExpenses() const;
    std::vector<Subscription> getUpcomingPayments(int days = 7) const;
    std::map<std::string, double> getTopCategoriesByCost(int top = 3) const;

    const std::vector<Subscription>& getAllSubscriptions() const { return subscriptions; }
    Subscription* findSubscriptionById(int id);
    bool isIdUnique(int id) const;
    int getNextId() const { return nextId; }
    void setNextId(int id) { nextId = id; }
    void incrementNextId() { nextId++; }
    void clear();
    size_t getSize() const { return subscriptions.size(); }
};

#endif