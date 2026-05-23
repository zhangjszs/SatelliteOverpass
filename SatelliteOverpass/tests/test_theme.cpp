#include "Visualization/Theme.h"
#include <iostream>
#include <sstream>
#include <cassert>

using namespace SatelliteOverpass::Visualization;

int main()
{
    static_assert(ANSI::RESET[0] == '\033');

    static_assert(Compat::RESET[0] == '\033');
    static_assert(Compat::BRIGHT_GREEN[0] == '\033');
    static_assert(Compat::BRIGHT_CYAN[0] == '\033');
    static_assert(Compat::BRIGHT_YELLOW[0] == '\033');
    static_assert(Compat::BRIGHT_RED[0] == '\033');
    static_assert(Compat::BRIGHT_WHITE[0] == '\033');
    static_assert(Compat::BRIGHT_BLUE[0] == '\033');
    static_assert(Compat::BOLD[0] == '\033');
    static_assert(Compat::DIM[0] == '\033');

    static_assert(Color::RESET == Compat::RESET);
    static_assert(Color::BRIGHT_GREEN == Compat::BRIGHT_GREEN);
    static_assert(Color::BRIGHT_CYAN == Compat::BRIGHT_CYAN);

    static_assert(UI::TITLE_BOLD != nullptr);
    static_assert(UI::VALUE != nullptr);
    static_assert(UI::SUCCESS_BOLD != nullptr);
    static_assert(UI::WARNING != nullptr);
    static_assert(UI::ERR_BOLD != nullptr);

    static_assert(Data::TRACK != nullptr);
    static_assert(Data::START_MARK != nullptr);
    static_assert(Data::END_MARK != nullptr);
    static_assert(Data::PEAK != nullptr);
    static_assert(Data::DIRECTION_BOLD != nullptr);
    static_assert(Data::GRID != nullptr);

    static_assert(Table::HEADER != nullptr);
    static_assert(Table::ROW_PRIMARY != nullptr);
    static_assert(Table::ROW_SECONDARY != nullptr);
    static_assert(Table::ROW_TERTIARY != nullptr);

    auto& mgr = ThemeManager::instance();
    assert(mgr.useAnsiColors() == true);
    mgr.disableColors();
    assert(mgr.useAnsiColors() == false);
    assert(mgr.apply(Compat::BRIGHT_GREEN)[0] == '\0');
    assert(mgr.reset()[0] == '\0');
    mgr.enableColors();
    assert(mgr.useAnsiColors() == true);
    assert(mgr.apply(Compat::BRIGHT_GREEN)[0] == '\033');
    assert(mgr.reset()[0] == '\033');

    {
        std::ostringstream oss;
        {
            ColorGuard guard(oss, UI::TITLE_BOLD);
            oss << "test";
        }
        std::string result = oss.str();
        assert(result.find("test") != std::string::npos);
    }

    std::cout << UI::TITLE_BOLD << "All Theme.h tests passed!" << Style::RESET << std::endl;
    return 0;
}
