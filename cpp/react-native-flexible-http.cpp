#include "react-native-flexible-http.h"
#include <jsi/jsi.h>
#include <map>
#include <string>
//#include "libhttp/include/libhttp.h"
#include "cpp-httplib/httplib.h"
#include <ReactCommon/CallInvoker.h>

#define CALLBACK_CONTAINER_NAME "__httpServer_callback_container"

static int currentJSIRuntimeIteration = 0;

// Turbomodule implementation following here
// https://github.com/facebook/react-native/blob/bf2500e38ec06d2de501c5a3737e396fe43d1fae/ReactCommon/jsi/jsi.h#L80

// Example creation of class which implements jsi::HostObject
// https://gist.github.com/axemclion/e7faeb4ee9d89832040e96d6837b269d
// react-native-reanimated original code back in 2019 :: https://medium.com/@christian.falch/react-native-jsi-challenge-2-56fc4dd91613
// usage of shared_ptr to initialize objects taken from
// https://github.com/shirakaba/react-native-native-runtime/blob/master/ios/react-native-objc-runtime.mm

// String hash switch case implementation as seen here
// https://learnmoderncpp.com/2020/06/01/strings-as-switch-case-labels/
constexpr inline unsigned long long string_hash(const char *s) {
    unsigned long long hash{}, c{};
    for (auto p = s; *p; ++p, ++c) {
        hash += *p << c;
    }
    return hash;
}

constexpr inline unsigned long long operator"" _sh(const char *s, size_t) {
    return string_hash(s);
}

template <typename recordFromMultimapMapType>
facebook::jsi::Object recordFromMultimap(facebook::jsi::Runtime &runtime,
                                         recordFromMultimapMapType inmap
                                         ) {
    facebook::jsi::Object ret(runtime);
    for (auto const& entry : inmap)
    {
        ret.setProperty(runtime, facebook::jsi::String::createFromUtf8(runtime, entry.first), entry.second);
    }
    return ret;
}

//using namespace restinio;
using namespace facebook;
using namespace jsi;

static const std::string nullStr = "";

static int GlobalServerCallbackIncrement = 0;
class SKHTTPServerCallbackContainer {
public:
    std::string regex = nullStr;
    jsi::Function callback;
    int id;
    SKHTTPServerCallbackContainer(
                                  jsi::Function cb,
                                  std::string pathRegex = nullStr
                                  ) : callback(std::move(cb)), regex(pathRegex)
    {
        GlobalServerCallbackIncrement++;
        id = GlobalServerCallbackIncrement;
    }
};
struct SKHTTPServerCallbackContainerComparison {
    bool operator()(SKHTTPServerCallbackContainer const & a, SKHTTPServerCallbackContainer const & b) const {
        // Not <= because "For all a, comp(a,a) == false", according to https://stackoverflow.com/a/47860226/4469172
        return a.id < b.id;
    }
};

namespace example {
class HTTPServer;

class HTTPRequestWrapper : public jsi::HostObject {
public:
    const httplib::Request& req;
    HTTPRequestWrapper(const httplib::Request& _req): req(_req) {}
    jsi::Value get(jsi::Runtime& runtime, const jsi::PropNameID& name) {
        //        return new jsi::JSError(runtine, value);
        std::string methodName = name.utf8(runtime);
        
        // String hash switch case implementation as seen here
        // https://learnmoderncpp.com/2020/06/01/strings-as-switch-case-labels/
        switch (string_hash(methodName.c_str())) {
            case "method"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, req.method));
            } break;
            case "path"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, req.path));
            } break;
            case "headers"_sh: {
                return recordFromMultimap(runtime, req.headers);
            } break;
            case "body"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, req.body));
            } break;
            case "remote_addr"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, req.remote_addr));
            } break;
            case "remote_port"_sh: {
                return jsi::Value(req.remote_port);
            } break;
            case "version"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, req.version));
            } break;
            case "target"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, req.target));
            } break;
            case "params"_sh: {
                return recordFromMultimap(runtime, req.params);
                //                return jsi::Value(jsi::String::createFromUtf8(runtime, req.params));
            } break;
            default: break;
        }
        return jsi::Value::undefined();
    }
};

class HTTPResponseWrapper : public jsi::HostObject {
public:
    httplib::Response& res;
    HTTPResponseWrapper(httplib::Response& _res): res(_res) {}
    jsi::Value get(jsi::Runtime& runtime, const jsi::PropNameID& name) {
        //        return new jsi::JSError(runtine, value);
        std::string methodName = name.utf8(runtime);
        
        // String hash switch case implementation as seen here
        // https://learnmoderncpp.com/2020/06/01/strings-as-switch-case-labels/
        switch (string_hash(methodName.c_str())) {
            case "version"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, res.version));
            } break;
            case "status"_sh: {
                return jsi::Value(res.status);
            } break;
            case "reason"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, res.reason));
            } break;
            case "headers"_sh: {
                return recordFromMultimap(runtime, res.headers);
            } break;
            case "body"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, res.body));
            } break;
            case "location"_sh: {
                return jsi::Value(jsi::String::createFromUtf8(runtime, res.location));
            } break;
            case "has_header"_sh: {
                return jsi::Function::createFromHostFunction
                (runtime, name, 1,
                 [&](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                     size_t count) -> jsi::Value
                 {
                    std::string key = arguments[0].asString(runtime).utf8(runtime);
                    return jsi::Value(res.has_header(key.c_str()));
                }
                 );
            } break;
            case "set_header"_sh: {
                return jsi::Function::createFromHostFunction
                (runtime, name, 2,
                 [&](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                     size_t count) -> jsi::Value
                 {
                    std::string key = arguments[0].asString(runtime).utf8(runtime);
                    std::string value = arguments[1].asString(runtime).utf8(runtime);
                    res.set_header(key.c_str(), value);
                    return jsi::Value::undefined();
                }
                 );
            } break;
            case "set_redirect"_sh: {
                return jsi::Function::createFromHostFunction
                (runtime, name, 2,
                 [&](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                     size_t count) -> jsi::Value
                 {
                    std::string url = arguments[0].asString(runtime).utf8(runtime);
                    if(arguments[1].isNumber()) {
                        int status = arguments[1].asNumber();
                        res.set_redirect(url, status);
                    }
                    else { res.set_redirect(url); }
                    return jsi::Value::undefined();
                }
                 );
            } break;
            case "set_content"_sh: {
                printf("set content called");
                return jsi::Function::createFromHostFunction
                (runtime, name, 2,
                 [&](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                     size_t count) -> jsi::Value
                 {
                    std::string content = arguments[0].asString(runtime).utf8(runtime);
                    if(arguments[1].isString()) {
                        std::string content_type = arguments[1].asString(runtime).utf8(runtime);
                        res.set_content(content, content_type.c_str());
                    }
                    else { res.set_content(content, "text/html"); }
                    printf("set content to %s", content.c_str());
                    return jsi::Value::undefined();
                }
                 );
            } break;
            default: break;
        }
        return jsi::Value::undefined();
    }
};

static jsi::Value genHttpCallbackStorageObject(jsi::Runtime &runtime) {
    jsi::Object obj = jsi::Object(runtime);
    obj.setProperty(runtime, "get", runtime.global().getPropertyAsFunction(runtime, "Map").callAsConstructor(runtime));
    obj.setProperty(runtime, "put", runtime.global().getPropertyAsFunction(runtime, "Map").callAsConstructor(runtime));
    obj.setProperty(runtime, "post", runtime.global().getPropertyAsFunction(runtime, "Map").callAsConstructor(runtime));
    obj.setProperty(runtime, "delete", runtime.global().getPropertyAsFunction(runtime, "Map").callAsConstructor(runtime));
    return obj;
}

static int NewHTTPServerID = 0;
class HTTPServer : public jsi::HostObject, public std::enable_shared_from_this<HTTPServer> {
    jsi::Runtime &jsiRuntime;
    httplib::Server svr;
    int instanceID;
    int jsiRuntimeIteration;
    std::thread httpThread;
    std::shared_ptr<react::CallInvoker> invoker;
public:
    HTTPServer(std::shared_ptr<react::CallInvoker> _invoker, jsi::Runtime &runtime) :
    invoker(_invoker), jsiRuntime(runtime)  {
        NewHTTPServerID++;
        instanceID = NewHTTPServerID;
        jsiRuntimeIteration = currentJSIRuntimeIteration;
    }
    // Called when runtime garbage collector finalizes this object
    ~HTTPServer() {
        // TODO: CLEANUP
        stop();
        if(jsiRuntimeIteration == currentJSIRuntimeIteration) {
            removeGlobalCallback();
        }
        // Clear all callbacks;
        printf("Destroyed HTTPServer");
    }
    void removeGlobalCallback() {
        jsi::Runtime &runtime = jsiRuntime;
        jsi::Object globalCBs = runtime.global().getPropertyAsObject(runtime, CALLBACK_CONTAINER_NAME);
        globalCBs.getPropertyAsFunction(runtime, "delete").callWithThis(runtime, globalCBs, jsi::Value(instanceID));
        runtime.global().setProperty(runtime, CALLBACK_CONTAINER_NAME, globalCBs);
    }
    
    void addCallbackForPath(std::string method, std::string path, jsi::Function callback) {
        /*
         think structure of
         global.CALLBACK_CONTAINER_NAME = Record<
         number, // instanceID
         Record<HTTPMethod, Map<PathName, Function>> // Record mapping method to pathname maps
         >
         */
        jsi::Runtime &runtime = jsiRuntime;
        jsi::Object globalCBs = runtime.global().getPropertyAsObject(runtime, CALLBACK_CONTAINER_NAME);
        jsi::Value myCallbackContainerValue = globalCBs.getPropertyAsFunction(runtime, "get").callWithThis(runtime, globalCBs, jsi::Value(instanceID));
        if(myCallbackContainerValue.isUndefined()) {
            // set new empty map if undefined
            // Create new map
            myCallbackContainerValue = genHttpCallbackStorageObject(runtime);
            globalCBs.getPropertyAsFunction(runtime, "set").callWithThis(runtime, globalCBs, jsi::Value(instanceID), myCallbackContainerValue);
            runtime.global().setProperty(runtime, CALLBACK_CONTAINER_NAME, globalCBs);
        }
        jsi::Object myCallbackContainer = myCallbackContainerValue.asObject(runtime);
        jsi::Object meMap = myCallbackContainer.getProperty(runtime, jsi::String::createFromUtf8(runtime, method)).asObject(runtime);
        meMap.getPropertyAsFunction(runtime, "set").callWithThis(runtime, meMap, jsi::String::createFromUtf8(runtime, path), callback);
        globalCBs.setProperty(runtime, CALLBACK_CONTAINER_NAME, meMap);
        runtime.global().setProperty(runtime, CALLBACK_CONTAINER_NAME, globalCBs);
    }
    
    void executeCallbackForPath(std::string method, std::string path, const httplib::Request& req, httplib::Response& res) {
        std::mutex m;
        // Lock this thread's mutex
        m.lock();
        invoker.get()->invokeAsync([&](){
            jsi::Runtime& runtime = jsiRuntime;
            jsi::Object globalCBs = runtime.global().getPropertyAsObject(runtime, CALLBACK_CONTAINER_NAME);
            jsi::Value meContainerValue = globalCBs.getPropertyAsFunction(runtime, "get").callWithThis(jsiRuntime, globalCBs, jsi::Value(instanceID));
            jsi::Value meValue = meContainerValue.asObject(runtime).getProperty(runtime, jsi::String::createFromUtf8(runtime, method));
            jsi::Object meMap = meValue.asObject(runtime);
            jsi::Value callbackValue = meMap.getPropertyAsFunction(runtime, "get").callWithThis(jsiRuntime, meMap, jsi::String::createFromUtf8(runtime, path));
            jsi::Function callback = callbackValue.asObject(runtime).asFunction(runtime);
            printf("invoking invoker");
            if(!callback.isFunction(jsiRuntime)) {
                printf("is not function");
                return;
            }
            jsi::Object request = jsi::Object::createFromHostObject(jsiRuntime, std::make_shared<HTTPRequestWrapper>(req));
            jsi::Object response = jsi::Object::createFromHostObject(jsiRuntime, std::make_shared<HTTPResponseWrapper>(res));
            callback.call(runtime, request, response);
            m.unlock();
        });
        
        // "try" to lock this thread's mutex again, but it's not going to be unlocked until
        // m.unlock() is called when the asynchronous Get call is finished!
        m.lock();
        m.unlock();
        switch(string_hash(method.c_str())) {
            case "GET"_sh: {
                
            } break;
            case "POST"_sh: {
                
            } break;
        }
    }
    
    // When JS wants a property with a given name from the HostObject,
    // it will call this method.  If it throws an exception, the call
    // will throw a JS \c Error object. By default this returns undefined.
    // \return the value for the property.
    jsi::Value get(jsi::Runtime& runtime, const jsi::PropNameID& name) {
        //        return new jsi::JSError(runtine, value);
        std::string methodName = name.utf8(runtime);
        
        // String hash switch case implementation as seen here
        // https://learnmoderncpp.com/2020/06/01/strings-as-switch-case-labels/
        switch (string_hash(methodName.c_str())) {
            case "myFunc"_sh:
            {
                
            }break;
            case "listen"_sh: {
                return jsi::Function::createFromHostFunction
                (runtime, name, 1,
                 [=](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                     size_t count) -> jsi::Value
                 {
                    int port = 8080;
                    if(count > 0) {
                        port = arguments[0].asNumber();
                    }
                    start(port);
                    return jsi::Value::undefined();
                }
                 );
            } break;
            case "get"_sh: {
                // Mimicking "express.get(url, (req, res, next)=>void)"
                return jsi::Function::createFromHostFunction
                (runtime,
                 name,
                 2,
                 [&, this](jsi::Runtime &_runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                           size_t count) -> jsi::Value
                 {
                    // confirmed: `thisValue` is equal to this `HTTPServer` object
                    jsi::Runtime &runtime = jsiRuntime;
                    std::string path = arguments[0].asString(runtime).utf8(runtime);
                    jsi::Function callback = arguments[1].asObject(runtime).asFunction(runtime);
                    if(!callback.isFunction(runtime)) {
                        printf("Second argument not a function");
                        return jsi::Value::undefined();
                    }
                    addCallbackForPath("get", path, std::move(callback));
                    svr.Get(path, [&, this, path](const httplib::Request& req, httplib::Response& res) {
                        executeCallbackForPath("get", path, req, res);
                    });
                    return thisValue.asObject(runtime);
                }
                 );
            } break;
            case "put"_sh: {
                return jsi::Function::createFromHostFunction
                (runtime,
                 name,
                 2,
                 [&, this](jsi::Runtime &_runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                           size_t count) -> jsi::Value
                 {
                    // confirmed: `thisValue` is equal to this `HTTPServer` object
                    jsi::Runtime &runtime = jsiRuntime;
                    std::string path = arguments[0].asString(runtime).utf8(runtime);
                    jsi::Function callback = arguments[1].asObject(runtime).asFunction(runtime);
                    if(!callback.isFunction(runtime)) {
                        printf("Second argument not a function");
                        return jsi::Value::undefined();
                    }
                    addCallbackForPath("get", path, std::move(callback));
                    svr.Put(path, [&, this, path](const httplib::Request& req, httplib::Response& res) {
                        executeCallbackForPath("put", path, req, res);
                    });
                    return thisValue.asObject(runtime);
                }
                 );
            } break;
            case "post"_sh: {
                return jsi::Function::createFromHostFunction
                (runtime,
                 name,
                 2,
                 [&, this](jsi::Runtime &_runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                           size_t count) -> jsi::Value
                 {
                    // confirmed: `thisValue` is equal to this `HTTPServer` object
                    jsi::Runtime &runtime = jsiRuntime;
                    std::string path = arguments[0].asString(runtime).utf8(runtime);
                    jsi::Function callback = arguments[1].asObject(runtime).asFunction(runtime);
                    if(!callback.isFunction(runtime)) {
                        printf("Second argument not a function");
                        return jsi::Value::undefined();
                    }
                    addCallbackForPath("post", path, std::move(callback));
                    svr.Post(path, [&, this, path](const httplib::Request& req, httplib::Response& res) {
                        executeCallbackForPath("post", path, req, res);
                    });
                    return thisValue.asObject(runtime);
                }
                 );
            } break;
            case "delete"_sh: {
                return jsi::Function::createFromHostFunction
                (runtime,
                 name,
                 2,
                 [&, this](jsi::Runtime &_runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                           size_t count) -> jsi::Value
                 {
                    // confirmed: `thisValue` is equal to this `HTTPServer` object
                    jsi::Runtime &runtime = jsiRuntime;
                    std::string path = arguments[0].asString(runtime).utf8(runtime);
                    jsi::Function callback = arguments[1].asObject(runtime).asFunction(runtime);
                    if(!callback.isFunction(runtime)) {
                        printf("Second argument not a function");
                        return jsi::Value::undefined();
                    }
                    addCallbackForPath("delete", path, std::move(callback));
                    svr.Delete(path, [&, this, path](const httplib::Request& req, httplib::Response& res) {
                        executeCallbackForPath("delete", path, req, res);
                    });
                    return thisValue.asObject(runtime);
                }
                 );
            } break;
                
                
            case "stop"_sh: {
                return jsi::Function::createFromHostFunction
                (runtime,
                 name,
                 0,
                 [&](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *arguments,
                     size_t count) -> jsi::Value
                 {
                    stop();
                    return thisValue.asObject(runtime);
                }
                 );
            } break;
                
            default:
                break;
        }
        return jsi::Value(runtime, jsi::String::createFromUtf8(runtime, "hello"));
        //        return new jsi::Value::string(std::string("hello"));
        //        return new jsi::Value(2);
    }
    
    // When JS wants to set a property with a given name on the HostObject,
    // it will call this method. If it throws an exception, the call will
    // throw a JS \c Error object. By default this throws a type error exception
    // mimicking the behavior of a frozen object in strict mode.
    void set(Runtime& runtime, const PropNameID& name, const Value& value) {
        std::string methodName = name.utf8(runtime);
        // String hash switch case implementation as seen here
        // https://learnmoderncpp.com/2020/06/01/strings-as-switch-case-labels/
        switch (string_hash(methodName.c_str())) {
            case "listen"_sh:
            case "stop"_sh:
            case "get"_sh:
            case "post"_sh:
            case "put"_sh:
            case "delete"_sh: {
                // Do nothing.
                //                return jsi::JSError(runtime, jsi::String::createFromAscii(runtime, "Cannot assign to property"));
                jsi::HostObject::set(runtime, name, value);
            } break;
            default: {
                //                auto obj = jsi::Object::createFromHostObject(runtime, shared_from_this());
                //                runtime.setPropertyValue(obj, jsi::String::createFromUtf8(runtime, methodName), jsi::Value::undefined());
            } break;
        }
    }
    
    void start(int port = 8080) {
        
        //        if(!svr.is_valid()) {
        //            printf("Invalid server");
        //            return;
        //        }
        //        svr.Get("/", [this](const httplib::Request& req, httplib::Response& res) {
        //            res.set_content("ohello world", "text/html");
        ////            invoker.get()->invokeAsync([&](){
        ////
        ////                jsi::Object request = jsi::Object::createFromHostObject(runtime, std::make_shared<HTTPRequestWrapper>(req));
        ////                jsi::Object response = jsi::Object::createFromHostObject(runtime, std::make_shared<HTTPResponseWrapper>(res));
        ////                for(const SKHTTPServerCallbackContainer &c : getCallbacks) {
        ////                    c.callback.call(runtime, request, response);
        ////                    //                    c.callback.call(runtime, (jsi::Value *)args, 2);
        ////                }
        ////            });
        //        });
        //        svr.Get(".*", [](const httplib::Request& req, httplib::Response& res) {
        //            res.set_content("ohello catch all", "text/html");
        //        });
        //        // TODO: HandlerWithContentReader
        //        svr.Post(".*", [](const httplib::Request& req, httplib::Response& res) {
        //        });
        //        svr.Put(".*", [](const httplib::Request& req, httplib::Response& res) {
        //        });
        //        svr.Delete(".*", [](const httplib::Request& req, httplib::Response& res) {
        //        });
        // Run servers
        // Need to run on another thread because otherwise crash lol
        httpThread = std::thread([&]() { svr.listen("localhost", port); });
        //        svr.listen("localhost", port);
        printf("started listening at port %d", port);
    }
    
    void stop() {
        svr.stop();
        // Join the httpThread because if not joined then it'll crash.
        if(httpThread.joinable()) {
            httpThread.join();
        }
    }
    
    void thing() {
        //        auto router = std::make_unique<router::express_router_t<>>();
    }
};
//    std::map

int multiply(float a, float b) {
    return a * b;
}

void install(jsi::Runtime &jsiRuntime, std::shared_ptr<react::CallInvoker> invoker) {
    auto createHTTPServer =
    jsi::Function::createFromHostFunction(
                                          jsiRuntime,
                                          PropNameID::forAscii(jsiRuntime, "createHTTPServer"),
                                          0,
                                          [&, invoker](Runtime &runtime, const Value &thisValue, const Value *arguments,
                                                       size_t count) -> Value
                                          {
                                              auto obj = std::make_shared<HTTPServer>(invoker, runtime);
                                              jsi::Object object = jsi::Object::createFromHostObject(runtime, obj);
                                              return object;
                                          });
    jsiRuntime.global().setProperty(jsiRuntime, "createHTTPServer",
                                    std::move(createHTTPServer));
    jsi::Function mapFunction = jsiRuntime.global().getPropertyAsFunction(jsiRuntime, "Map");
    jsi::Value callbackMap = mapFunction.callAsConstructor(jsiRuntime);
    jsiRuntime.global().setProperty(jsiRuntime, CALLBACK_CONTAINER_NAME, callbackMap);
}

void cleanup(jsi::Runtime &jsiRuntime) {
    currentJSIRuntimeIteration += 1;
}

};
