#include "client/linux/handler/exception_handler.h"

using namespace google_breakpad;

typedef ExceptionHandler::MinidumpCallback MinidumpCallback;
typedef ExceptionHandler::FilterCallback FilterCallback;

extern "C" {
	void *rust_breakpad_descriptor_new(const char *path);

	const char *rust_breakpad_descriptor_path(const MinidumpDescriptor *desc);

	void rust_breakpad_descriptor_free(MinidumpDescriptor *desc);

	void *rust_breakpad_exceptionhandler_new(void *desc, FilterCallback fcb,
	    MinidumpCallback mcb, void *context, int install_handler);

	bool rust_breakpad_exceptionhandler_write_minidump(void *eh);

	void rust_breakpad_exceptionhandler_free(void *eh);
}
