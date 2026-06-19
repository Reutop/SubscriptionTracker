#include "SubscriptionManager.h"
#include <algorithm>
#include <cctype>

SubscriptionManager::SubscriptionManager() : nextId(1) {}

bool SubscriptionManager::addSubscription(const Subscription& subscription) {
    if (!isIdUnique(subscription.getId())) {
        return false;
    }
    subscriptions.push_back(subscription);
    if (subscription.getId() >= nextId) {
        nextId = subscription.getId() + 1;
    }
    return true;
}

bool SubscriptionManager::editSubscription(int id, const Subscription& updatedSubscription) {
    for (auto& sub : subscriptions) {
        if (sub.getId() == id) {
            sub = updatedSubscription;
            return true;
        }
    }
    return false;
}

bool SubscriptionManager::deleteSubscriptionById(int id) {
    auto it = std::remove_if(subscriptions.begin(), subscriptions.end(),
        [id](const Subscription& sub) { return sub.getId() == id; });

    if (it != subscriptions.end()) {
        subscriptions.erase(it, subscriptions.end());
        return true;
    }
    return false;
}

bool SubscriptionManager::deleteSubscriptionByName(const std::string& name) {
    std::string searchName = name;
    std::transform(searchName.begin(), searchName.end(), searchName.begin(),
        [](unsigned char c) { return std::tolower(c); });

    auto it = std::remove_if(subscriptions.begin(), subscriptions.end(),
        [&searchName](const Subscription& sub) {
            std::string subName = sub.getServiceName();
            std::transform(subName.begin(), subName.end(), subName.begin(),
                [](unsigned char c) { return std::tolower(c); });
            return subName == searchName;
        });

    if (it != subscriptions.end()) {
        subscriptions.erase(it, subscriptions.end());
        return true;
    }
    return false;
}

std::vector<Subscription> SubscriptionManager::searchByName(const std::string& query) const {
    std::vector<Subscription> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (const auto& sub : subscriptions) {
        std::string subName = sub.getServiceName();
        std::transform(subName.begin(), subName.end(), subName.begin(),
            [](unsigned char c) { return std::tolower(c); });
        if (subName.find(lowerQuery) != std::string::npos) {
            results.push_back(sub);
        }
    }
    return results;
}

std::vector<Subscription> SubscriptionManager::filterByCategory(Category category) const {
    std::vector<Subscription> results;
    for (const auto& sub : subscriptions) {
        if (sub.getCategory() == category) {
            results.push_back(sub);
        }
    }
    return results;
}

std::vector<Subscription> SubscriptionManager::filterByCostRange(double min, double max) const {
    std::vector<Subscription> results;
    for (const auto& sub : subscriptions) {
        double cost = sub.getEffectiveMonthlyCost();
        if (cost >= min && cost <= max) {
            results.push_back(sub);
        }
    }
    return results;
}

std::vector<Subscription> SubscriptionManager::filterByDateRange(std::time_t start, std::time_t end) const {
    std::vector<Subscription> results;
    for (const auto& sub : subscriptions) {
        std::time_t date = sub.getNextPaymentDate();
        if (date >= start && date <= end) {
            results.push_back(sub);
        }
    }
    return results;
}

void SubscriptionManager::sortByMonthlyCost(bool ascending) {
    std::sort(subscriptions.begin(), subscriptions.end(),
        [ascending](const Subscription& a, const Subscription& b) {
            return ascending ? (a.getEffectiveMonthlyCost() < b.getEffectiveMonthlyCost())
                : (a.getEffectiveMonthlyCost() > b.getEffectiveMonthlyCost());
        });
}

void SubscriptionManager::sortByNextPaymentDate() {
    std::sort(subscriptions.begin(), subscriptions.end(),
        [](const Subscription& a, const Subscription& b) {
            return a.getNextPaymentDate() < b.getNextPaymentDate();
        });
}

void SubscriptionManager::sortByServiceName() {
    std::sort(subscriptions.begin(), subscriptions.end(),
        [](const Subscription& a, const Subscription& b) {
            return a.getServiceName() < b.getServiceName();
        });
}

double SubscriptionManager::getTotalMonthlyExpenses() const {
    double total = 0.0;
    for (const auto& sub : subscriptions) {
        if (sub.isActiveSubscription()) {
            total += sub.getEffectiveMonthlyCost();
        }
    }
    return total;
}

std::vector<Subscription> SubscriptionManager::getUpcomingPayments(int days) const {
    std::vector<Subscription> upcoming;
    std::time_t now = std::time(nullptr);
    std::time_t future = now + (static_cast<std::time_t>(days) * 24 * 3600);

    for (const auto& sub : subscriptions) {
        if (sub.isActiveSubscription()) {
            std::time_t paymentDate = sub.getNextPaymentDate();
            if (paymentDate >= now && paymentDate <= future) {
                upcoming.push_back(sub);
            }
        }
    }

    std::sort(upcoming.begin(), upcoming.end(),
        [](const Subscription& a, const Subscription& b) {
            return a.getNextPaymentDate() < b.getNextPaymentDate();
        });

    return upcoming;
}

std::map<std::string, double> SubscriptionManager::getTopCategoriesByCost(int top) const {
    std::map<std::string, double> categoryTotals;

    for (const auto& sub : subscriptions) {
        if (sub.isActiveSubscription()) {
            std::string cat = sub.getCategoryString();
            categoryTotals[cat] += sub.getEffectiveMonthlyCost();
        }
    }

    std::vector<std::pair<std::string, double>> sortedCategories(
        categoryTotals.begin(), categoryTotals.end());

    std::sort(sortedCategories.begin(), sortedCategories.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    std::map<std::string, double> topCategories;
    int limit = std::min(top, static_cast<int>(sortedCategories.size()));
    for (int i = 0; i < limit; ++i) {
        topCategories[sortedCategories[i].first] = sortedCategories[i].second;
    }

    return topCategories;
}

Subscription* SubscriptionManager::findSubscriptionById(int id) {
    for (auto& sub : subscriptions) {
        if (sub.getId() == id) {
            return &sub;
        }
    }
    return nullptr;
}

bool SubscriptionManager::isIdUnique(int id) const {
    for (const auto& sub : subscriptions) {
        if (sub.getId() == id) {
            return false;
        }
    }
    return true;
}

void SubscriptionManager::clear() {
    subscriptions.clear();
}