#include "shared/app.h"

struct Sample : public MainLoop {};

int main() { return App{"Cube"}.Run(Sample{}); }
