#pragma once

namespace Terminal {
    struct TerminalSize {
        int width;
        int height;
    };

    TerminalSize getTerminalSize();
}
