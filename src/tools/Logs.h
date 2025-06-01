#ifndef LOGS_H
#define LOGS_H

#include <string>

class Logs {
   public:
    void static log();

   private:
    std::string static exceptions();
    std::string static getFileName();
    void static createFile(const std::string &content);
};

#endif  // LOGS_H
