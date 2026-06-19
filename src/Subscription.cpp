#define _CRT_SECURE_NO_WARNINGS
#include "Subscription.h"
#include <sstream>
#include <iomanip>
#include <ctime>

Subscription::Subscription()
    : id(0), category(Category::OTHER), monthlyCost(0.0),
    billingCycle(BillingCycle::MONTHLY), nextPaymentDate(0), isActive(true) {
}

Subscription::Subscription(int id, const std::string& serviceName, Category category,
    double monthlyCost, BillingCycle billingCycle,
    std::time_t nextPaymentDate, bool isActive)
    : id(id), serviceName(serviceName), category(category), monthlyCost(monthlyCost),
    billingCycle(billingCycle), nextPaymentDate(nextPaymentDate), isActive(isActive) {
}

double Subscription::getEffectiveMonthlyCost() const {
    switch (billingCycle) {
    case BillingCycle::YEARLY:
        return monthlyCost / 12.0;
    case BillingCycle::QUARTERLY:
        return monthlyCost / 3.0;
    case BillingCycle::WEEKLY:
        return monthlyCost * 4.0;
    case BillingCycle::MONTHLY:
    default:
        return monthlyCost;
    }
}

std::string Subscription::getCategoryString() const {
    switch (category) {
    case Category::ENTERTAINMENT: return "Развлечения";
    case Category::SOFTWARE:      return "Софт";
    case Category::EDUCATION:     return "Обучение";
    case Category::UTILITIES:     return "Коммунальные";
    case Category::OTHER:
    default:                      return "Другое";
    }
}

std::string Subscription::getBillingCycleString() const {
    switch (billingCycle) {
    case BillingCycle::MONTHLY:   return "monthly";
    case BillingCycle::YEARLY:    return "yearly";
    case BillingCycle::WEEKLY:    return "weekly";
    case BillingCycle::QUARTERLY: return "quarterly";
    default:                      return "monthly";
    }
}

std::string Subscription::getNextPaymentDateString() const {
    std::tm tmInfo{};
#ifdef _WIN32
    localtime_s(&tmInfo, &nextPaymentDate);
#else
    if (const auto* tempTm = std::localtime(&nextPaymentDate)) {
        tmInfo = *tempTm;
    }
#endif
    char buffer[12] = { 0 };
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tmInfo);
    return std::string(buffer);
}

Category Subscription::stringToCategory(const std::string& str) {
    if (str == "Развлечения") return Category::ENTERTAINMENT;
    if (str == "Софт")         return Category::SOFTWARE;
    if (str == "Обучение")     return Category::EDUCATION;
    if (str == "Коммунальные") return Category::UTILITIES;
    return Category::OTHER;
}

BillingCycle Subscription::stringToBillingCycle(const std::string& str) {
    if (str == "monthly")   return BillingCycle::MONTHLY;
    if (str == "yearly")    return BillingCycle::YEARLY;
    if (str == "weekly")    return BillingCycle::WEEKLY;
    if (str == "quarterly") return BillingCycle::QUARTERLY;
    return BillingCycle::MONTHLY;
}