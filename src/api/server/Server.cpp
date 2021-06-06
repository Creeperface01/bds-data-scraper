#include "Server.h"
#include <dirent.h>
#include <iostream>
#include "v8pp/convert.hpp"

using namespace bdsapi;
using namespace v8pp;
using namespace std;

const char *ToCString(const v8::String::Utf8Value &value) {
    return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::Isolate *isolate, v8::TryCatch *try_catch) {
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(isolate, try_catch->Exception());
    const char *exception_string = ToCString(exception);
    v8::Local<v8::Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        fprintf(stderr, "%s\n", exception_string);
    } else {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
        v8::Local<v8::Context> context(isolate->GetCurrentContext());
        const char *filename_string = ToCString(filename);
        int linenum = message->GetLineNumber(context).FromJust();
        fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
        // Print line of source code.
        v8::String::Utf8Value sourceline(
                isolate,
                message->GetSourceLine(context).ToLocalChecked());
        const char *sourceline_string = ToCString(sourceline);
        fprintf(stderr, "%s\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn(context).FromJust();
        for (int i = 0; i < start; i++) {
            fprintf(stderr, " ");
        }
        int end = message->GetEndColumn(context).FromJust();
        for (int i = start; i < end; i++) {
            fprintf(stderr, "^");
        }
        fprintf(stderr, "\n");
        v8::Local<v8::Value> stack_trace_string;
        if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
            v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
            const char *stack_trace_string = ToCString(stack_trace);
            fprintf(stderr, "%s\n", stack_trace_string);
        }
    }
}

vector<string> loadScripts() {
    auto dir = opendir("mods/scripts");

    if (dir == nullptr) {
        system("mkdir mods/scripts");
        dir = opendir("mods/scripts");
    }

    vector<string> scripts;
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
        auto name = string(ent->d_name);

        if (name[name.size() - 3] == '.' && name[name.size() - 2] == 'j' && name[name.size() - 1] == 's') {
            cout << "File: " << name << endl;
            scripts.push_back(name);
        }
    }

    return scripts;
}

vector<char> readFile(const char *filename) {
    std::ifstream input(filename, ios::binary);

    std::vector<char> bytes(
            (istreambuf_iterator<char>(input)),
            (istreambuf_iterator<char>()));

    input.close();

    return bytes;
}

Server::Server(Api *pApi) {
    this->pApi = pApi;
}

void Server::tick() {

}

void Server::loadPlugins() {
    Locker locker(pApi->isolate);

    Isolate::Scope isolate_scope(pApi->isolate);
    HandleScope handle_scope(pApi->isolate);

//    auto bytes = readFile("mods/scripts/test.js");
//    Local<String> source = String::NewFromUtf8(pApi->isolate, &bytes[0], String::NewStringType::kNormalString,
//                                               bytes.size());

//    cout << *(String::Utf8Value(pApi->isolate, source));
//    Local<Context> context = Context::New(pApi->isolate);
    Local<Context> context = Local<Context>::New(pApi->isolate, pApi->pContext);

    TryCatch tryCatch(pApi->isolate);
    Local<Script> script = Script::Compile(context, to_v8(pApi->isolate, "'Test'")).ToLocalChecked();

//    if(!Script::Compile(context, to_v8(pApi->isolate, "'Test'")).ToLocal(&script))  {
//        ReportException(pApi->isolate, &tryCatch);
//    }

    Context::Scope context_scope(context);
    Local<Value> r = script->Run(context).ToLocalChecked();

    cout << "script result: " << *(String::Utf8Value(pApi->isolate, r));
//    cout << "script result: " << from_v8<std::string>(pApi->isolate, r);

//    Handle<Value> value = pApi->context->Global()->Get(String::NewFromUtf8(pApi->isolate, "test_function"));
//
//    if (value->IsFunction()) {
//        Handle<Function> function = Handle<Function>::Cast(value);
//        Handle<Value> args[3]{
//                String::NewFromUtf8(pApi->isolate, "arg1"),
//                String::NewFromUtf8(pApi->isolate, "arg2"),
//                String::NewFromUtf8(pApi->isolate, "arg3"),
//        };
//
//        Handle<Value> result = function->Call(pApi->context, pApi->context->Global(), 3, args).ToLocalChecked();
//        cout << "result: " << *(String::Utf8Value(pApi->isolate, result)) << endl;
//    }

}
