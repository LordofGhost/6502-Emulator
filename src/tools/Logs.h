#ifndef LOGS_H
#define LOGS_H

#include <string>

class Logs {
   public:
    void static log();
    std::string static folderName();
    static bool getClockHistoryConfig();

   private:
    std::string static exceptions();
    void static createFile(const std::string &content);
};

#endif  // LOGS_H
