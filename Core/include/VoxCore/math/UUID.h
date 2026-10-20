#pragma once
#include <cstdint>
#include <random>
#include <cstdio>

#include <VoxCore/Define.h>

VOX_NS
    struct Xoshiro {
        static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
        uint64_t s[4];

        Xoshiro() {
            std::random_device rd;
            do {
                for (auto &v: s) v = (static_cast<uint64_t>(rd()) << 32) | rd();
            } while (!(s[0] | s[1] | s[2] | s[3]));
        }

        uint64_t next() {
            uint64_t r = std::rotl(s[1] * 5, 7) * 9, t = s[1] << 17;
            s[2] ^= s[0];
            s[3] ^= s[1];
            s[1] ^= s[2];
            s[0] ^= s[3];
            s[2] ^= t;
            s[3] = rotl(s[3], 45);
            return r;
        }
    };


    class UUID {
        uint64_t hi, lo;

        UUID uuid4() {
            thread_local Xoshiro rng;
            UUID u{rng.next(), rng.next()};
            u.hi = (u.hi & ~0xF000ull) | 0x4000ull;
            u.lo = (u.lo & 0x3FFFFFFFFFFFFFFFull) | 0x8000000000000000ull;
            return u;
        }

    public:
        constexpr std::string toString() const {
            return std::format("{:08x}-{:04x}-{:04x}-{:04x}-{:012x}",
                static_cast<uint32_t>(hi >> 32),
                static_cast<uint16_t>(hi >> 16),
                static_cast<uint16_t>(hi),
                static_cast<uint16_t>(lo >> 48),
                lo & 0xFFFFFFFFFFFFULL
            );
        }

        constexpr friend bool operator==(const UUID &a, const UUID &b) { return a.hi == b.hi && a.lo == b.lo; }
        constexpr friend bool operator!=(const UUID &a, const UUID &b) { return !(a == b); }
        constexpr friend bool operator<(const UUID &a, const UUID &b) { return a.hi != b.hi ? a.hi < b.hi : a.lo < b.lo; }

        constexpr bool isNil() const { return (hi | lo) == 0; }
    };

NS_END

template<>
struct std::hash<Vox::UUID> {
    constexpr size_t operator()(const Vox::UUID &u) const noexcept {
        return (size_t) (u.hi ^ (u.lo * 0x9E3779B97F4A7C15ull));
    }
};
