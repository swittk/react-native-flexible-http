#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <memory>

namespace facebook {
namespace jsi {
class Runtime;
}
namespace react {
class CallInvoker;
}
}
namespace example {
int multiply(float a, float b);
void install(facebook::jsi::Runtime &jsiRuntime, std::shared_ptr<facebook::react::CallInvoker> invoker);
void cleanup(facebook::jsi::Runtime &jsiRuntime);
}

#endif /* EXAMPLE_H */
