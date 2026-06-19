#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <string>
#include <chrono>
#include <ctime>

enum class Category {
    ENTERTAINMENT,
    SOFTWARE,
    EDUCATION,
    UTILITIES,
    OTHER
};

enum class BillingCycle {
    MONTHLY,
    YEARLY,
    WEEKLY,
    QUARTERLY
};

class Subscription {
private:
    int id;
    std::string serviceName;
    Category category;
    double monthlyCost;
    BillingCycle billingCycle;
    std::time_t nextPaymentDate;
    bool isActive;

public:
    Subscription();
    Subscription(int id, const std::string& serviceName, Category category,
        double monthlyCost, BillingCycle billingCycle,
        std::time_t nextPaymentDate, bool isActive = true);

    int getId() const { return id; }
    std::string getServiceName() const { return serviceName; }
    Category getCategory() const { return category; }
    double getMonthlyCost() const { return monthlyCost; }
    BillingCycle getBillingCycle() const { return billingCycle; }
    std::time_t getNextPaymentDate() const { return nextPaymentDate; }
    bool isActiveSubscription() const { return isActive; }

    void setServiceName(const std::string& name) { serviceName = name; }
    void setCategory(Category cat) { category = cat; }
    void setMonthlyCost(double cost) { monthlyCost = cost; }
    void setBillingCycle(BillingCycle cycle) { billingCycle = cycle; }
    void setNextPaymentDate(std::time_t date) { nextPaymentDate = date; }
    void setActive(bool active) { isActive = active; }

    double getEffectiveMonthlyCost() const;
    std::string getCategoryString() const;
    std::string getBillingCycleString() const;
    std::string getNextPaymentDateString() const;

    static Category stringToCategory(const std::string& str);
    static BillingCycle stringToBillingCycle(const std::string& str);
};

#endif