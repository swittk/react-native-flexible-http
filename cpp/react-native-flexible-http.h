#ifndef EXAMPLE_H
#define EXAMPLE_H

namespace facebook {
namespace jsi {
class Runtime;
}
}
namespace example {
int multiply(float a, float b);
void install(facebook::jsi::Runtime &jsiRuntime);
}

#endif /* EXAMPLE_H */
