#include "workqueue.h"

#include <iostream>
#include <variant>

#ifdef DEBUG
#include <cstdio>
#define LOG(msg) printf(msg)
#define LOG_VA(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define LOG(msg)
#define LOG_VA(fmt, ...)
#endif

struct Custom {
    Custom() : i_(0), str_("") {
        LOG("Default Constructor\n");
    }

    Custom(int i, const std::string& s) : i_(i), str_(s) {
        LOG("Primary Constructor\n");
    }

    Custom(const Custom& other) : i_(other.i_), str_(other.str_) {
        LOG("Copy Constructor\n");
    }

    Custom(Custom&& other) {
        i_ = other.i_;
        str_ = std::move(other.str_);
        LOG("Move Constructor\n");
    }

    Custom& operator=(const Custom& other) {
        if (this == &other) return *this;
        i_ = other.i_;
        str_ = other.str_;
        LOG("Copy Assignment\n");
        return *this;
    }

    Custom& operator=(Custom&& other) {
        if (this == &other) return *this;
        i_ = other.i_;
        str_ = std::move(other.str_);
        LOG("Move Assignment\n");
        return *this;
    }

    int i_;
    std::string str_;
};

using Info = std::variant<int, std::string, Custom>;

class InfoProcessor {
public:
    InfoProcessor() : wq_(this) {
        wq_.Start();
    }

    void Process(int i) {
        std::cout << i << std::endl;
    }

    void Process(std::string&& s) {
        std::cout << s << std::endl;
    }

    void Process(Custom&& c) {
        std::cout << c.i_ << " " << c.str_ << std::endl;
    }

    void ProcessWorkItem(Info&& info) {
        std::visit([this](auto&& i){ Process(std::forward<decltype(i)>(i)); }, std::forward<Info>(info));
    }

    void DoWork(Info&& i) {
        wq_.Push(std::move(i));
    }

private:
    ZLib::WorkQueue<Info, InfoProcessor> wq_;
};

int main() {
    InfoProcessor ip;
    ip.DoWork(1);
    ip.DoWork("Test");
    ip.DoWork(Custom{});
    ip.DoWork(Custom{29, "string"});
    ip.DoWork(2);
    ip.DoWork(3);
    ip.DoWork(4);
    ip.DoWork("String");
    ip.DoWork(Custom{23, "custom"});

    std::string input;
    std::getline(std::cin, input);
    std::cout << "DONE" << std::endl;
}