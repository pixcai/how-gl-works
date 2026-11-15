#include "framework/framework.h"

struct Sample : public framework::App {
    void Render() override {}
};

int main() { return Sample().Run(); }
