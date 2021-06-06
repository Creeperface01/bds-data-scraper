#include "Api.h"
#include <iostream>
#include <modloader/log.h>
#include <v8pp/module.hpp>
#include <v8pp/class.hpp>
#include <bds/bedrock/ServerInstance.h>

using namespace std;
using namespace modloader;
using namespace v8;
using namespace v8pp;

namespace bdsapi {

    class TestClass {
    public:
        float x;
        float y;
        float z;

        TestClass(float x, float y, float z) : x(x), y(y), z(z) {

        }

        float sum() {
            return x + y + z;
        }
    };

    const char *Api::PREFIX = "BDS-API";

    void apiTest(FunctionCallbackInfo<Value> const &args) {
        HandleScope handle_scope(args.GetIsolate());

        for (int i = 0; i < args.Length(); ++i) {
            if (i > 0) std::cout << ' ';
            cout << *String::Utf8Value(args.GetIsolate(), args[i]);
        }

        cout << std::endl;
    }

    Api::Api() {
        Log::info(PREFIX, "initializing api");
//        API_INSTANCE = this;

        v8Platform = platform::NewDefaultPlatform();
        V8::InitializePlatform(v8Platform.get());
        V8::Initialize();

        Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
        isolate = Isolate::New(create_params);

        Locker locker(isolate);

        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);

        Local<Context> context = Context::New(isolate);
        pContext.Reset(isolate, context);

        //enter scope
        Context::Scope context_scope(context);

//        module m(isolate);
//        m.set("testApi", &apiTest);
//
//        class_ <TestClass> clazz(isolate);
//        clazz
//                .set("x", &TestClass::x)
//                .set("y", &TestClass::y)
//                .set("z", &TestClass::z)
//                .set("sum", &TestClass::sum);
//
//        m.set("TestClass", clazz);
//        m.new_instance();
//
//        context->Global()->Set(
//                to_v8(isolate, "testInstance"),
//                class_<TestClass>::import_external(isolate, new TestClass(1, 2, 3))
//                );

        Local<Script> script = Script::Compile(context, to_v8(isolate, "'Test'")).ToLocalChecked();

        Local<Value> r = script->Run(context).ToLocalChecked();

        cout << "script result: " << *(String::Utf8Value(isolate, r));
    }

    void Api::initApi() {
        this->server = new Server(this);
        this->server->loadPlugins();
    }

    void Api::shutdown() {
        Log::info("BDS-API", "Stopping API");
        isolate->Dispose();
        V8::Dispose();
        V8::ShutdownPlatform();
    }

    void Api::start() {

    }
}