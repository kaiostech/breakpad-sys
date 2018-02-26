#include "client/linux/handler/exception_handler.h"
#include "rust_breakpad_linux.h"

struct WrapperContext {
    MinidumpCallback mcb;
    FilterCallback fcb;
    void *real_context;

    WrapperContext(FilterCallback fcb, MinidumpCallback mcb, void *context);
};

WrapperContext::WrapperContext(FilterCallback fcb, MinidumpCallback mcb, void *context) {
    this->mcb = mcb;
    this->fcb = fcb;
    this->real_context = context;
}

static bool filter_callback_wrapper(void *context) {
    WrapperContext *cont = reinterpret_cast<WrapperContext*>(context);

    return cont->fcb ? cont->fcb(cont->real_context) : true;
}

static bool minidump_callback_wrapper(const MinidumpDescriptor &desc, void *context, bool succeeded) {
    WrapperContext *cont = reinterpret_cast<WrapperContext*>(context);

    return cont->mcb ? cont->mcb(desc, cont->real_context, succeeded) : succeeded;
}

extern "C" {
    void *rust_breakpad_descriptor_new(const char *path) {
        return reinterpret_cast<void*>(new MinidumpDescriptor(path));
    }

    const char *rust_breakpad_descriptor_path(const MinidumpDescriptor *desc) {
        return reinterpret_cast<const MinidumpDescriptor*>(desc)->path();
    }

    void rust_breakpad_descriptor_free(MinidumpDescriptor *desc) {
        delete reinterpret_cast<MinidumpDescriptor*>(desc);
    }

    void *rust_breakpad_exceptionhandler_new(void *desc, FilterCallback fcb,
            MinidumpCallback mcb, void *context, int install_handler) {

        WrapperContext *wrapper_context = new WrapperContext(fcb, mcb, context);

        ExceptionHandler *eh = new ExceptionHandler(
            *reinterpret_cast<MinidumpDescriptor*>(desc),
            filter_callback_wrapper,
            minidump_callback_wrapper,
            reinterpret_cast<void*>(wrapper_context),
            install_handler,
            -1);

        return reinterpret_cast<void*>(eh);
    }

    bool rust_breakpad_exceptionhandler_write_minidump(void *eh) {
        return reinterpret_cast<ExceptionHandler*>(eh)->WriteMinidump();
    }

    void rust_breakpad_exceptionhandler_free(void *eh) {
        delete reinterpret_cast<ExceptionHandler*>(eh);
    }
}
