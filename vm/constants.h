/* file contains constant strings */

#define START       "Ninja Virtual Machine started"
#define STOP        "Ninja Virtual Machine stopped"
#define VERSION     "Ninja Virtual Machine version 2 (compiled " __DATE__ " " __TIME__ " )\n"
#define VERSION_NO  4
#define HELP        "Usage: " __FILE__ " [options] <code file>\n  --debug \t start virtual machine in debug mode\n  --version \t show version and exit \n  --help \t show this help and exit\n"

// Text colors for debugger
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"