// EPOS colored by tecnicolor ;)

#ifndef __color_h
#define __color_h

struct Color
{
    static const char * RED() { return "\033[91m"; }
    static const char * YELLOW() { return "\033[93m"; }
    static const char * BLUE() { return "\033[94m"; }
    static const char * GREEN() { return "\033[92m"; }

    static const char * HEADER() { return "\033[95m"; }
    static const char * OK_BLUE() { return BLUE(); }
    static const char * OK_GREEN() { return GREEN(); }
    static const char * WARNING() { return YELLOW(); }
    static const char * FAIL() { return RED(); }
    static const char * END_COLOR() { return "\033[0m"; }
    static const char * BOLD() { return "\033[1m"; }
    static const char * UNDERLINE() { return "\033[4m"; }
};

#endif
