#include "json_helper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace JsonHelper {
    
    // JsonValueのメソッド実装
    JsonValue& JsonValue::operator[](const std::string& key) {
        if (type != JsonType::OBJECT) {
            type = JsonType::OBJECT;
            objectValue.clear();
        }
        return objectValue[key];
    }

    const JsonValue& JsonValue::operator[](const std::string& key) const {
        static JsonValue nullValue;
        if (type != JsonType::OBJECT) return nullValue;
        auto it = objectValue.find(key);
        return (it != objectValue.end()) ? it->second : nullValue;
    }

    void JsonValue::pushBack(const JsonValue& value) {
        if (type != JsonType::ARRAY) {
            type = JsonType::ARRAY;
            arrayValue.clear();
        }
        arrayValue.push_back(value);
    }

    size_t JsonValue::size() const {
        if (type == JsonType::ARRAY) return arrayValue.size();
        if (type == JsonType::OBJECT) return objectValue.size();
        return 0;
    }

    // パーサーのヘルパー関数
    void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.length() && std::isspace(str[pos])) {
            pos++;
        }
    }

    std::string parseString(const std::string& str, size_t& pos) {
        if (pos >= str.length() || str[pos] != '"') {
            throw std::runtime_error("Expected '\"' at position " + std::to_string(pos));
        }
        pos++; // skip opening quote
        
        std::string result;
        while (pos < str.length() && str[pos] != '"') {
            if (str[pos] == '\\' && pos + 1 < str.length()) {
                pos++;
                switch (str[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += str[pos]; break;
                }
            } else {
                result += str[pos];
            }
            pos++;
        }
        
        if (pos >= str.length() || str[pos] != '"') {
            throw std::runtime_error("Unterminated string");
        }
        pos++; // skip closing quote
        return result;
    }

    double parseNumber(const std::string& str, size_t& pos) {
        size_t start = pos;
        if (pos < str.length() && str[pos] == '-') pos++;
        
        while (pos < str.length() && std::isdigit(str[pos])) pos++;
        
        if (pos < str.length() && str[pos] == '.') {
            pos++;
            while (pos < str.length() && std::isdigit(str[pos])) pos++;
        }
        
        if (pos < str.length() && (str[pos] == 'e' || str[pos] == 'E')) {
            pos++;
            if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) pos++;
            while (pos < str.length() && std::isdigit(str[pos])) pos++;
        }
        
        return std::stod(str.substr(start, pos - start));
    }

    JsonValue parseValue(const std::string& str, size_t& pos);

    JsonValue parseObject(const std::string& str, size_t& pos) {
        JsonValue obj;
        obj.type = JsonType::OBJECT;
        
        if (pos >= str.length() || str[pos] != '{') {
            throw std::runtime_error("Expected '{' at position " + std::to_string(pos));
        }
        pos++; // skip '{'
        
        skipWhitespace(str, pos);
        
        if (pos < str.length() && str[pos] == '}') {
            pos++;
            return obj;
        }
        
        while (pos < str.length()) {
            skipWhitespace(str, pos);
            
            // パースキー
            std::string key = parseString(str, pos);
            
            skipWhitespace(str, pos);
            
            if (pos >= str.length() || str[pos] != ':') {
                throw std::runtime_error("Expected ':' after key");
            }
            pos++; // skip ':'
            
            skipWhitespace(str, pos);
            
            // パース値
            JsonValue value = parseValue(str, pos);
            obj.objectValue[key] = value;
            
            skipWhitespace(str, pos);
            
            if (pos >= str.length()) break;
            
            if (str[pos] == '}') {
                pos++;
                break;
            } else if (str[pos] == ',') {
                pos++;
                continue;
            } else {
                throw std::runtime_error("Expected ',' or '}' in object");
            }
        }
        
        return obj;
    }

    JsonValue parseArray(const std::string& str, size_t& pos) {
        JsonValue arr;
        arr.type = JsonType::ARRAY;
        
        if (pos >= str.length() || str[pos] != '[') {
            throw std::runtime_error("Expected '[' at position " + std::to_string(pos));
        }
        pos++; // skip '['
        
        skipWhitespace(str, pos);
        
        if (pos < str.length() && str[pos] == ']') {
            pos++;
            return arr;
        }
        
        while (pos < str.length()) {
            skipWhitespace(str, pos);
            
            JsonValue value = parseValue(str, pos);
            arr.arrayValue.push_back(value);
            
            skipWhitespace(str, pos);
            
            if (pos >= str.length()) break;
            
            if (str[pos] == ']') {
                pos++;
                break;
            } else if (str[pos] == ',') {
                pos++;
                continue;
            } else {
                throw std::runtime_error("Expected ',' or ']' in array");
            }
        }
        
        return arr;
    }

    JsonValue parseValue(const std::string& str, size_t& pos) {
        skipWhitespace(str, pos);
        
        if (pos >= str.length()) {
            throw std::runtime_error("Unexpected end of input");
        }
        
        char c = str[pos];
        
        if (c == '"') {
            return JsonValue(parseString(str, pos));
        } else if (c == '{') {
            return parseObject(str, pos);
        } else if (c == '[') {
            return parseArray(str, pos);
        } else if (c == 't' && str.substr(pos, 4) == "true") {
            pos += 4;
            return JsonValue(true);
        } else if (c == 'f' && str.substr(pos, 5) == "false") {
            pos += 5;
            return JsonValue(false);
        } else if (c == 'n' && str.substr(pos, 4) == "null") {
            pos += 4;
            JsonValue nullVal;
            nullVal.type = JsonType::JSON_NULL;
            return nullVal;
        } else if (c == '-' || std::isdigit(c)) {
            return JsonValue(parseNumber(str, pos));
        } else {
            throw std::runtime_error("Unexpected character '" + std::string(1, c) + "' at position " + std::to_string(pos));
        }
    }

    // 公開関数の実装
    JsonValue parseJson(const std::string& jsonStr) {
        size_t pos = 0;
        try {
            return parseValue(jsonStr, pos);
        } catch (const std::exception& e) {
            std::cerr << "JSON Parse Error: " << e.what() << std::endl;
            return JsonValue();
        }
    }

    std::string escapeString(const std::string& str) {
        std::string result = "\"";
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        result += "\"";
        return result;
    }

    std::string jsonToString(const JsonValue& json, int indent) {
        std::string indentStr(indent * 2, ' ');
        std::string nextIndentStr((indent + 1) * 2, ' ');
        
        switch (json.type) {
            case JsonType::STRING:
                return escapeString(json.stringValue);
            case JsonType::NUMBER:
                return std::to_string(json.numberValue);
            case JsonType::BOOLEAN:
                return json.boolValue ? "true" : "false";
            case JsonType::JSON_NULL:
                return "null";
            case JsonType::ARRAY: {
                if (json.arrayValue.empty()) return "[]";
                std::string result = "[\n";
                for (size_t i = 0; i < json.arrayValue.size(); ++i) {
                    result += nextIndentStr + jsonToString(json.arrayValue[i], indent + 1);
                    if (i < json.arrayValue.size() - 1) result += ",";
                    result += "\n";
                }
                result += indentStr + "]";
                return result;
            }
            case JsonType::OBJECT: {
                if (json.objectValue.empty()) return "{}";
                std::string result = "{\n";
                auto it = json.objectValue.begin();
                while (it != json.objectValue.end()) {
                    result += nextIndentStr + escapeString(it->first) + ": " + jsonToString(it->second, indent + 1);
                    ++it;
                    if (it != json.objectValue.end()) result += ",";
                    result += "\n";
                }
                result += indentStr + "}";
                return result;
            }
        }
        return "null";
    }

    JsonValue loadJsonFromFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return JsonValue();
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        return parseJson(buffer.str());
    }

    bool saveJsonToFile(const JsonValue& json, const std::string& filePath) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << filePath << std::endl;
            return false;
        }
        
        file << jsonToString(json, 0);
        file.close();
        return true;
    }

    JsonValue createObject() {
        JsonValue obj;
        obj.type = JsonType::OBJECT;
        return obj;
    }

    JsonValue createArray() {
        JsonValue arr;
        arr.type = JsonType::ARRAY;
        return arr;
    }
}