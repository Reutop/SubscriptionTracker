#ifndef FILE_IO_H
#define FILE_IO_H

#include <string>
#include "Subscription.h"
#include "SubscriptionManager.h"

class FileIO {
public:
    static bool loadFromFile(const std::string& filename, SubscriptionManager& manager);
    
    static bool saveToFile(const std::string& filename, const SubscriptionManager& manager);
};

#endif