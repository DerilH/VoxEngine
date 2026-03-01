//
// Created by deril on 2/21/26.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <set>
#include <queue>
#include "VoxCore/Define.h"
#include <llvm/ADT/SmallVector.h>

VOX_NS
    template<typename T>
    using SmallVector = std::vector<T>;

    template<typename T>
    using Vector = std::vector<T>;

    template<typename T>
    using Queue = std::queue<T>;

    template<typename T>
    using HashSet = std::unordered_set<T>;

    template<typename K, typename V>
    using HashMap = std::unordered_map<K, V>;

    template<typename K, typename V>
    using Pair = std::tuple<K, V>;

    template<typename T>
    using Optional = std::optional<T>;
    inline constexpr auto NullOpt = std::nullopt;


    using String = std::string;
    using InternedString = std::string;


NS_END