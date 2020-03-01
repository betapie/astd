#pragma once
#ifndef ASTD_CSTDDEF
#define ASTD_CSTDDEF

namespace astd
{
    using nullptr_t = decltype(nullptr);

    constexpr auto npos = static_cast<size_t>(-1);
}

#endif // ASTD_CSTDDEF