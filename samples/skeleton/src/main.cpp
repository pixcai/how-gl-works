#include "framework/framework.h"

struct Sample : public framework::App {
    void Render() override {}
};

int main() {
    framework::Run<Sample>("Skeleton");
    return 0;
}
