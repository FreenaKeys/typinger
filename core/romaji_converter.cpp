// romaji_converter.cpp
// ローマ字→かな変換テーブルと変換ロジックの実装

#include "romaji_converter.h"
#include <algorithm>
#include <iostream>

namespace RomajiConverter {

    // ConvertResult コンストラクタ
    ConvertResult::ConvertResult() 
        : status(ConvertStatus::NO_MATCH), kana(""), consumed(""), remaining("") {}

    ConvertResult::ConvertResult(ConvertStatus s, const std::string& k, 
                                 const std::string& c, const std::string& r)
        : status(s), kana(k), consumed(c), remaining(r) {}

    // Converter コンストラクタ
    Converter::Converter() {
        initializeTable();
    }

    // 変換テーブル初期化
    void Converter::initializeTable() {
        // 清音(Seion/Basic Sounds) - 50音
        conversionTable_["a"] = "あ";
        conversionTable_["i"] = "い";
        conversionTable_["u"] = "う";
        conversionTable_["e"] = "え";
        conversionTable_["o"] = "お";

        conversionTable_["ka"] = "か";
        conversionTable_["ki"] = "き";
        conversionTable_["ku"] = "く";
        conversionTable_["ke"] = "け";
        conversionTable_["ko"] = "こ";

        conversionTable_["sa"] = "さ";
        conversionTable_["si"] = "し";  // 複数表記対応
        conversionTable_["shi"] = "し";
        conversionTable_["su"] = "す";
        conversionTable_["se"] = "せ";
        conversionTable_["so"] = "そ";

        conversionTable_["ta"] = "た";
        conversionTable_["ti"] = "ち";  // 複数表記対応
        conversionTable_["chi"] = "ち";
        conversionTable_["tu"] = "つ";  // 複数表記対応
        conversionTable_["tsu"] = "つ";
        conversionTable_["te"] = "て";
        conversionTable_["to"] = "と";

        conversionTable_["na"] = "な";
        conversionTable_["ni"] = "に";
        conversionTable_["nu"] = "ぬ";
        conversionTable_["ne"] = "ね";
        conversionTable_["no"] = "の";

        conversionTable_["ha"] = "は";
        conversionTable_["hi"] = "ひ";
        conversionTable_["hu"] = "ふ";  // 複数表記対応
        conversionTable_["fu"] = "ふ";
        conversionTable_["he"] = "へ";
        conversionTable_["ho"] = "ほ";

        conversionTable_["ma"] = "ま";
        conversionTable_["mi"] = "み";
        conversionTable_["mu"] = "む";
        conversionTable_["me"] = "め";
        conversionTable_["mo"] = "も";

        conversionTable_["ya"] = "や";
        conversionTable_["yu"] = "ゆ";
        conversionTable_["yo"] = "よ";

        conversionTable_["ra"] = "ら";
        conversionTable_["ri"] = "り";
        conversionTable_["ru"] = "る";
        conversionTable_["re"] = "れ";
        conversionTable_["ro"] = "ろ";

        conversionTable_["wa"] = "わ";
        conversionTable_["wo"] = "を";
        conversionTable_["nn"] = "ん";  // nnは必ず「ん」

        // 濁音(Dakuon/Voiced Sounds)
        conversionTable_["ga"] = "が";
        conversionTable_["gi"] = "ぎ";
        conversionTable_["gu"] = "ぐ";
        conversionTable_["ge"] = "げ";
        conversionTable_["go"] = "ご";

        conversionTable_["za"] = "ざ";
        conversionTable_["zi"] = "じ";  // 複数表記対応
        conversionTable_["ji"] = "じ";
        conversionTable_["zu"] = "ず";
        conversionTable_["ze"] = "ぜ";
        conversionTable_["zo"] = "ぞ";

        conversionTable_["da"] = "だ";
        conversionTable_["di"] = "ぢ";
        conversionTable_["du"] = "づ";
        conversionTable_["de"] = "で";
        conversionTable_["do"] = "ど";

        conversionTable_["ba"] = "ば";
        conversionTable_["bi"] = "び";
        conversionTable_["bu"] = "ぶ";
        conversionTable_["be"] = "べ";
        conversionTable_["bo"] = "ぼ";

        // 半濁音(Handakuon/Semi-voiced Sounds)
        conversionTable_["pa"] = "ぱ";
        conversionTable_["pi"] = "ぴ";
        conversionTable_["pu"] = "ぷ";
        conversionTable_["pe"] = "ぺ";
        conversionTable_["po"] = "ぽ";

        // 拗音(Youon/Contracted Sounds) - きゃ、しゃ等
        conversionTable_["kya"] = "きゃ";
        conversionTable_["kyu"] = "きゅ";
        conversionTable_["kyo"] = "きょ";

        conversionTable_["sya"] = "しゃ";  // 複数表記対応
        conversionTable_["sha"] = "しゃ";
        conversionTable_["syu"] = "しゅ";
        conversionTable_["shu"] = "しゅ";
        conversionTable_["syo"] = "しょ";
        conversionTable_["sho"] = "しょ";

        conversionTable_["tya"] = "ちゃ";  // 複数表記対応
        conversionTable_["cha"] = "ちゃ";
        conversionTable_["tyu"] = "ちゅ";
        conversionTable_["chu"] = "ちゅ";
        conversionTable_["tyo"] = "ちょ";
        conversionTable_["cho"] = "ちょ";

        conversionTable_["nya"] = "にゃ";
        conversionTable_["nyu"] = "にゅ";
        conversionTable_["nyo"] = "にょ";

        conversionTable_["hya"] = "ひゃ";
        conversionTable_["hyu"] = "ひゅ";
        conversionTable_["hyo"] = "ひょ";

        conversionTable_["mya"] = "みゃ";
        conversionTable_["myu"] = "みゅ";
        conversionTable_["myo"] = "みょ";

        conversionTable_["rya"] = "りゃ";
        conversionTable_["ryu"] = "りゅ";
        conversionTable_["ryo"] = "りょ";

        conversionTable_["gya"] = "ぎゃ";
        conversionTable_["gyu"] = "ぎゅ";
        conversionTable_["gyo"] = "ぎょ";

        conversionTable_["zya"] = "じゃ";  // 複数表記対応
        conversionTable_["ja"] = "じゃ";
        conversionTable_["zyu"] = "じゅ";
        conversionTable_["ju"] = "じゅ";
        conversionTable_["zyo"] = "じょ";
        conversionTable_["jo"] = "じょ";

        conversionTable_["bya"] = "びゃ";
        conversionTable_["byu"] = "びゅ";
        conversionTable_["byo"] = "びょ";

        conversionTable_["pya"] = "ぴゃ";
        conversionTable_["pyu"] = "ぴゅ";
        conversionTable_["pyo"] = "ぴょ";

        // 促音(Sokuon/Geminate Consonant) - っ
        conversionTable_["xtu"] = "っ";
        conversionTable_["xtsu"] = "っ";
        conversionTable_["ltu"] = "っ";
        conversionTable_["ltsu"] = "っ";

        // 小書き文字
        conversionTable_["xa"] = "ぁ";
        conversionTable_["xi"] = "ぃ";
        conversionTable_["xu"] = "ぅ";
        conversionTable_["xe"] = "ぇ";
        conversionTable_["xo"] = "ぉ";
        conversionTable_["xya"] = "ゃ";
        conversionTable_["xyu"] = "ゅ";
        conversionTable_["xyo"] = "ょ";
        conversionTable_["xwa"] = "ゎ";

        conversionTable_["la"] = "ぁ";
        conversionTable_["li"] = "ぃ";
        conversionTable_["lu"] = "ぅ";
        conversionTable_["le"] = "ぇ";
        conversionTable_["lo"] = "ぉ";
        conversionTable_["lya"] = "ゃ";
        conversionTable_["lyu"] = "ゅ";
        conversionTable_["lyo"] = "ょ";
        conversionTable_["lwa"] = "ゎ";

        // 特殊な組み合わせ
        // 注: "n"単独は特殊処理で対応（n+子音→ん、nn→ん）
        conversionTable_["wha"] = "うぁ";
        conversionTable_["whi"] = "うぃ";
        conversionTable_["whe"] = "うぇ";
        conversionTable_["who"] = "うぉ";
    }

    // 部分一致チェック（入力途中かどうか判定）
    bool Converter::hasPartialMatch(const std::string& romaji) const {
        for (const auto& pair : conversionTable_) {
            // テーブルのキーがromajiで始まるかチェック
            if (pair.first.find(romaji) == 0 && pair.first.length() > romaji.length()) {
                return true;
            }
        }
        return false;
    }

    // ローマ字をかなに変換（最長一致優先）
    ConvertResult Converter::convert(const std::string& input) {
        if (input.empty()) {
            return ConvertResult(ConvertStatus::NO_MATCH);
        }

        // nの特殊処理を最優先: n + (子音) → ん（母音・y・n以外）
        // "nni"を"n"+"ni"として処理し、"nn"+"i"と誤らないようにする
        if (input.length() >= 2 && input[0] == 'n' && input[1] != 'a' && 
            input[1] != 'i' && input[1] != 'u' && input[1] != 'e' && 
            input[1] != 'o' && input[1] != 'y' && input[1] != 'n') {
            // n + (子音) → ん
            std::string remaining = input.substr(1);
            return ConvertResult(ConvertStatus::MATCHED, "ん", "n", remaining);
        }

        // 最長一致を探す
        std::string longestMatch = "";
        std::string matchedKana = "";

        for (size_t len = input.length(); len > 0; --len) {
            std::string prefix = input.substr(0, len);
            auto it = conversionTable_.find(prefix);
            if (it != conversionTable_.end()) {
                longestMatch = prefix;
                matchedKana = it->second;
                break;
            }
        }

        if (!longestMatch.empty()) {
            // 完全一致
            std::string remaining = input.substr(longestMatch.length());
            return ConvertResult(ConvertStatus::MATCHED, matchedKana, longestMatch, remaining);
        }

        // 部分一致チェック
        if (hasPartialMatch(input)) {
            return ConvertResult(ConvertStatus::PARTIAL, "", "", input);
        }

        // 促音の特殊処理: 子音の重複 → っ
        if (input.length() >= 2 && input[0] == input[1] && 
            std::string("kgsztdhbpmyrwn").find(input[0]) != std::string::npos) {
            std::string remaining = input.substr(1);
            return ConvertResult(ConvertStatus::MATCHED, "っ", std::string(1, input[0]), remaining);
        }

        // 一致なし
        return ConvertResult(ConvertStatus::NO_MATCH);
    }

    // 貪欲変換（文字列全体を可能な限り変換）
    std::string Converter::convertGreedy(const std::string& input, std::string& remaining) {
        std::string result = "";
        std::string current = input;

        while (!current.empty()) {
            ConvertResult res = convert(current);
            
            if (res.status == ConvertStatus::MATCHED) {
                // DEBUG: 変換途中経過を出力
                // std::cout << "  [DEBUG] '" << res.consumed << "' -> '" << res.kana << "', remaining: '" << res.remaining << "'" << std::endl;
                result += res.kana;
                current = res.remaining;
            } else if (res.status == ConvertStatus::PARTIAL) {
                // 入力途中なので残りとして返す
                remaining = current;
                return result;
            } else {
                // 変換不可能な文字があれば残りとして返す
                remaining = current;
                return result;
            }
        }

        remaining = "";
        return result;
    }

    // 変換可能かチェック
    bool Converter::canConvert(const std::string& romaji) const {
        return conversionTable_.find(romaji) != conversionTable_.end();
    }

    // テーブルサイズ取得（デバッグ用）
    size_t Converter::getTableSize() const {
        return conversionTable_.size();
    }

} // namespace RomajiConverter
