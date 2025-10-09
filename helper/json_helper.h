#pragma once
#include <string>
#include <map>
#include <vector>

namespace JsonHelper {
    // JSONの値の型
    enum class JsonType {
        STRING,
        NUMBER,
        BOOLEAN,
        ARRAY,
        OBJECT,
        JSON_NULL
    };

    // JSON値を表現するクラス
    class JsonValue {
    public:
        JsonType type;
        std::string stringValue;
        double numberValue;
        bool boolValue;
        std::vector<JsonValue> arrayValue;
        std::map<std::string, JsonValue> objectValue;

        JsonValue() : type(JsonType::JSON_NULL), numberValue(0), boolValue(false) {}
        JsonValue(const std::string& str) : type(JsonType::STRING), stringValue(str), numberValue(0), boolValue(false) {}
        JsonValue(double num) : type(JsonType::NUMBER), numberValue(num), boolValue(false) {}
        JsonValue(bool b) : type(JsonType::BOOLEAN), numberValue(0), boolValue(b) {}

        // 型チェック
        bool isString() const { return type == JsonType::STRING; }
        bool isNumber() const { return type == JsonType::NUMBER; }
        bool isBool() const { return type == JsonType::BOOLEAN; }
        bool isArray() const { return type == JsonType::ARRAY; }
        bool isObject() const { return type == JsonType::OBJECT; }
        bool isNull() const { return type == JsonType::JSON_NULL; }

        // 値取得
        std::string asString() const { return stringValue; }
        double asNumber() const { return numberValue; }
        bool asBool() const { return boolValue; }
        const std::vector<JsonValue>& asArray() const { return arrayValue; }
        const std::map<std::string, JsonValue>& asObject() const { return objectValue; }

        // オブジェクト操作
        JsonValue& operator[](const std::string& key);
        const JsonValue& operator[](const std::string& key) const;
        
        // 配列操作
        void pushBack(const JsonValue& value);
        size_t size() const;
    };

    // JSON文字列をパース
    JsonValue parseJson(const std::string& jsonStr);
    
    // JSONオブジェクトを文字列に変換
    std::string jsonToString(const JsonValue& json, int indent = 0);
    
    // ファイルからJSONを読み込み
    JsonValue loadJsonFromFile(const std::string& filePath);
    
    // JSONをファイルに保存
    bool saveJsonToFile(const JsonValue& json, const std::string& filePath);
    
    // 便利な作成関数
    JsonValue createObject();
    JsonValue createArray();
}