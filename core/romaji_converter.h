#pragma once
// romaji_converter.h
// ローマ字→かな変換器
// 
// 用語解説:
// - ローマ字(Romaji): キーボードで入力するアルファベット表記（例: "ka", "shi"）
// - かな(Kana): 日本語のひらがな（例: "か", "し"）
// - 変換テーブル(Conversion Table): ローマ字とかなの対応表
// - 複数表記(Multi-variant): 1つのかなに複数のローマ字表記がある（例: し=shi/si）

#include <string>
#include <vector>
#include <map>

namespace RomajiConverter {

    // 変換結果の状態
    enum class ConvertStatus {
        MATCHED,        // 完全一致（変換成功）
        PARTIAL,        // 部分一致（まだ入力途中）
        NO_MATCH        // 不一致（該当なし）
    };

    // 変換結果
    struct ConvertResult {
        ConvertStatus status;       // 変換状態
        std::string kana;           // 変換後のかな（MATCHEDの場合のみ）
        std::string consumed;       // 消費されたローマ字
        std::string remaining;      // 残りのローマ字
        
        ConvertResult();
        ConvertResult(ConvertStatus s, const std::string& k = "", 
                     const std::string& c = "", const std::string& r = "");
    };

    // ローマ字→かな変換器クラス
    class Converter {
    private:
        // 変換テーブル: ローマ字 → かな
        // 複数のローマ字表記を持つかなに対応
        std::map<std::string, std::string> conversionTable_;
        
        // 初期化用ヘルパー
        void initializeTable();
        
        // 部分一致チェック（入力途中かどうか）
        bool hasPartialMatch(const std::string& romaji) const;

    public:
        Converter();

        // ローマ字をかなに変換
        // input: 変換対象のローマ字文字列
        // 戻り値: 変換結果（status, kana, consumed, remaining）
        ConvertResult convert(const std::string& input);

        // 最長一致変換（貪欲マッチ）
        // input: 変換対象のローマ字文字列
        // 戻り値: 変換できた部分のかな + 残りのローマ字
        std::string convertGreedy(const std::string& input, std::string& remaining);

        // 特定のローマ字が変換可能かチェック
        bool canConvert(const std::string& romaji) const;

        // デバッグ用: 変換テーブルのサイズを取得
        size_t getTableSize() const;
    };

} // namespace RomajiConverter
