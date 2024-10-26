#pragma once

#include <iostream>
#include <map>       
#include <string>  
#include <vector>
#include <filesystem>

//json
#include "json/json.h"

static int countSubstrOccurrences(const std::string& str, const std::string& substr) {
    int count = 0;
    std::size_t pos = 0;

    // Search for the substring in 'str' starting from position 'pos'
    while ((pos = str.find(substr, pos)) != std::string::npos) {
        ++count; // Increment count for each occurrence found
        pos += substr.length(); // Move position index forward
    }

    return count;
}

static bool stringExistsInVector(const std::vector<std::string>& vec, const std::string& str) {
    // Usa std::find para verificar se a string existe no vetor
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}