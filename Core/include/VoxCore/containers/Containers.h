//
// Created by deril on 2/21/26.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include "VoxCore/Define.h"

VOX_NS
    template<typename T>
    using Vector = std::vector<T>;

    template<typename T>
    using HashSet = std::set<T>;

    template<typename K, typename V>
    using HashMap = std::unordered_map<K, V>;

    using String = std::string;
    using InternedString = std::string;

NS_END