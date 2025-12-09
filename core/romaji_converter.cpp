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

    // かな→ローマ字変換テーブル（静的）
    std::map<std::string, std::string> Converter::kanaToRomajiMap;

    // かな→ローマ字変換テーブルの初期化
    void Converter::initKanaToRomaji() {
        if (!kanaToRomajiMap.empty()) return;

        // 清音
        kanaToRomajiMap["あ"] = "a"; kanaToRomajiMap["い"] = "i"; kanaToRomajiMap["う"] = "u"; kanaToRomajiMap["え"] = "e"; kanaToRomajiMap["お"] = "o";
        kanaToRomajiMap["か"] = "ka"; kanaToRomajiMap["き"] = "ki"; kanaToRomajiMap["く"] = "ku"; kanaToRomajiMap["け"] = "ke"; kanaToRomajiMap["こ"] = "ko";
        kanaToRomajiMap["さ"] = "sa"; kanaToRomajiMap["し"] = "shi"; kanaToRomajiMap["す"] = "su"; kanaToRomajiMap["せ"] = "se"; kanaToRomajiMap["そ"] = "so";
        kanaToRomajiMap["た"] = "ta"; kanaToRomajiMap["ち"] = "chi"; kanaToRomajiMap["つ"] = "tsu"; kanaToRomajiMap["て"] = "te"; kanaToRomajiMap["と"] = "to";
        kanaToRomajiMap["な"] = "na"; kanaToRomajiMap["に"] = "ni"; kanaToRomajiMap["ぬ"] = "nu"; kanaToRomajiMap["ね"] = "ne"; kanaToRomajiMap["の"] = "no";
        kanaToRomajiMap["は"] = "ha"; kanaToRomajiMap["ひ"] = "hi"; kanaToRomajiMap["ふ"] = "fu"; kanaToRomajiMap["へ"] = "he"; kanaToRomajiMap["ほ"] = "ho";
        kanaToRomajiMap["ま"] = "ma"; kanaToRomajiMap["み"] = "mi"; kanaToRomajiMap["む"] = "mu"; kanaToRomajiMap["め"] = "me"; kanaToRomajiMap["も"] = "mo";
        kanaToRomajiMap["や"] = "ya"; kanaToRomajiMap["ゆ"] = "yu"; kanaToRomajiMap["よ"] = "yo";
        kanaToRomajiMap["ら"] = "ra"; kanaToRomajiMap["り"] = "ri"; kanaToRomajiMap["る"] = "ru"; kanaToRomajiMap["れ"] = "re"; kanaToRomajiMap["ろ"] = "ro";
        kanaToRomajiMap["わ"] = "wa"; kanaToRomajiMap["を"] = "wo"; kanaToRomajiMap["ん"] = "nn";

        // 濁音
        kanaToRomajiMap["が"] = "ga"; kanaToRomajiMap["ぎ"] = "gi"; kanaToRomajiMap["ぐ"] = "gu"; kanaToRomajiMap["げ"] = "ge"; kanaToRomajiMap["ご"] = "go";
        kanaToRomajiMap["ざ"] = "za"; kanaToRomajiMap["じ"] = "ji"; kanaToRomajiMap["ず"] = "zu"; kanaToRomajiMap["ぜ"] = "ze"; kanaToRomajiMap["ぞ"] = "zo";
        kanaToRomajiMap["だ"] = "da"; kanaToRomajiMap["ぢ"] = "ji"; kanaToRomajiMap["づ"] = "zu"; kanaToRomajiMap["で"] = "de"; kanaToRomajiMap["ど"] = "do";
        kanaToRomajiMap["ば"] = "ba"; kanaToRomajiMap["び"] = "bi"; kanaToRomajiMap["ぶ"] = "bu"; kanaToRomajiMap["べ"] = "be"; kanaToRomajiMap["ぼ"] = "bo";

        // 半濁音
        kanaToRomajiMap["ぱ"] = "pa"; kanaToRomajiMap["ぴ"] = "pi"; kanaToRomajiMap["ぷ"] = "pu"; kanaToRomajiMap["ぺ"] = "pe"; kanaToRomajiMap["ぽ"] = "po";

        // 拗音
        kanaToRomajiMap["きゃ"] = "kya"; kanaToRomajiMap["きゅ"] = "kyu"; kanaToRomajiMap["きょ"] = "kyo";
        kanaToRomajiMap["しゃ"] = "sha"; kanaToRomajiMap["しゅ"] = "shu"; kanaToRomajiMap["しょ"] = "sho";
        kanaToRomajiMap["ちゃ"] = "cha"; kanaToRomajiMap["ちゅ"] = "chu"; kanaToRomajiMap["ちょ"] = "cho";
        kanaToRomajiMap["にゃ"] = "nya"; kanaToRomajiMap["にゅ"] = "nyu"; kanaToRomajiMap["にょ"] = "nyo";
        kanaToRomajiMap["ひゃ"] = "hya"; kanaToRomajiMap["ひゅ"] = "hyu"; kanaToRomajiMap["ひょ"] = "hyo";
        kanaToRomajiMap["みゃ"] = "mya"; kanaToRomajiMap["みゅ"] = "myu"; kanaToRomajiMap["みょ"] = "myo";
        kanaToRomajiMap["りゃ"] = "rya"; kanaToRomajiMap["りゅ"] = "ryu"; kanaToRomajiMap["りょ"] = "ryo";
        kanaToRomajiMap["ぎゃ"] = "gya"; kanaToRomajiMap["ぎゅ"] = "gyu"; kanaToRomajiMap["ぎょ"] = "gyo";
        kanaToRomajiMap["じゃ"] = "ja"; kanaToRomajiMap["じゅ"] = "ju"; kanaToRomajiMap["じょ"] = "jo";
        kanaToRomajiMap["びゃ"] = "bya"; kanaToRomajiMap["びゅ"] = "byu"; kanaToRomajiMap["びょ"] = "byo";
        kanaToRomajiMap["ぴゃ"] = "pya"; kanaToRomajiMap["ぴゅ"] = "pyu"; kanaToRomajiMap["ぴょ"] = "pyo";

        // 特殊
        kanaToRomajiMap["ー"] = "-";
        kanaToRomajiMap["、"] = ",";
        kanaToRomajiMap["。"] = ".";

        // カタカナ対応（主要なもの）
        kanaToRomajiMap["ア"] = "a"; kanaToRomajiMap["イ"] = "i"; kanaToRomajiMap["ウ"] = "u"; kanaToRomajiMap["エ"] = "e"; kanaToRomajiMap["オ"] = "o";
        kanaToRomajiMap["カ"] = "ka"; kanaToRomajiMap["キ"] = "ki"; kanaToRomajiMap["ク"] = "ku"; kanaToRomajiMap["ケ"] = "ke"; kanaToRomajiMap["コ"] = "ko";
        kanaToRomajiMap["サ"] = "sa"; kanaToRomajiMap["シ"] = "shi"; kanaToRomajiMap["ス"] = "su"; kanaToRomajiMap["セ"] = "se"; kanaToRomajiMap["ソ"] = "so";
        kanaToRomajiMap["タ"] = "ta"; kanaToRomajiMap["チ"] = "chi"; kanaToRomajiMap["ツ"] = "tsu"; kanaToRomajiMap["テ"] = "te"; kanaToRomajiMap["ト"] = "to";
        kanaToRomajiMap["ナ"] = "na"; kanaToRomajiMap["ニ"] = "ni"; kanaToRomajiMap["ヌ"] = "nu"; kanaToRomajiMap["ネ"] = "ne"; kanaToRomajiMap["ノ"] = "no";
        kanaToRomajiMap["ハ"] = "ha"; kanaToRomajiMap["ヒ"] = "hi"; kanaToRomajiMap["フ"] = "fu"; kanaToRomajiMap["ヘ"] = "he"; kanaToRomajiMap["ホ"] = "ho";
        kanaToRomajiMap["マ"] = "ma"; kanaToRomajiMap["ミ"] = "mi"; kanaToRomajiMap["ム"] = "mu"; kanaToRomajiMap["メ"] = "me"; kanaToRomajiMap["モ"] = "mo";
        kanaToRomajiMap["ヤ"] = "ya"; kanaToRomajiMap["ユ"] = "yu"; kanaToRomajiMap["ヨ"] = "yo";
        kanaToRomajiMap["ラ"] = "ra"; kanaToRomajiMap["リ"] = "ri"; kanaToRomajiMap["ル"] = "ru"; kanaToRomajiMap["レ"] = "re"; kanaToRomajiMap["ロ"] = "ro";
        kanaToRomajiMap["ワ"] = "wa"; kanaToRomajiMap["ヲ"] = "wo"; kanaToRomajiMap["ン"] = "nn";
        
        kanaToRomajiMap["ガ"] = "ga"; kanaToRomajiMap["ギ"] = "gi"; kanaToRomajiMap["グ"] = "gu"; kanaToRomajiMap["ゲ"] = "ge"; kanaToRomajiMap["ゴ"] = "go";
        kanaToRomajiMap["ザ"] = "za"; kanaToRomajiMap["ジ"] = "ji"; kanaToRomajiMap["ズ"] = "zu"; kanaToRomajiMap["ゼ"] = "ze"; kanaToRomajiMap["ゾ"] = "zo";
        kanaToRomajiMap["ダ"] = "da"; kanaToRomajiMap["ヂ"] = "ji"; kanaToRomajiMap["ヅ"] = "zu"; kanaToRomajiMap["デ"] = "de"; kanaToRomajiMap["ド"] = "do";
        kanaToRomajiMap["バ"] = "ba"; kanaToRomajiMap["ビ"] = "bi"; kanaToRomajiMap["ブ"] = "bu"; kanaToRomajiMap["ベ"] = "be"; kanaToRomajiMap["ボ"] = "bo";
        kanaToRomajiMap["パ"] = "pa"; kanaToRomajiMap["ピ"] = "pi"; kanaToRomajiMap["プ"] = "pu"; kanaToRomajiMap["ペ"] = "pe"; kanaToRomajiMap["ポ"] = "po";

        kanaToRomajiMap["キャ"] = "kya"; kanaToRomajiMap["キュ"] = "kyu"; kanaToRomajiMap["キョ"] = "kyo";
        kanaToRomajiMap["シャ"] = "sha"; kanaToRomajiMap["シュ"] = "shu"; kanaToRomajiMap["ショ"] = "sho";
        kanaToRomajiMap["チャ"] = "cha"; kanaToRomajiMap["チュ"] = "chu"; kanaToRomajiMap["チョ"] = "cho";
        kanaToRomajiMap["ニャ"] = "nya"; kanaToRomajiMap["ニュ"] = "nyu"; kanaToRomajiMap["ニョ"] = "nyo";
        kanaToRomajiMap["ヒャ"] = "hya"; kanaToRomajiMap["ヒュ"] = "hyu"; kanaToRomajiMap["ヒョ"] = "hyo";
        kanaToRomajiMap["ミャ"] = "mya"; kanaToRomajiMap["ミュ"] = "myu"; kanaToRomajiMap["ミョ"] = "myo";
        kanaToRomajiMap["リャ"] = "rya"; kanaToRomajiMap["リュ"] = "ryu"; kanaToRomajiMap["リョ"] = "ryo";
        kanaToRomajiMap["ギャ"] = "gya"; kanaToRomajiMap["ギュ"] = "gyu"; kanaToRomajiMap["ギョ"] = "gyo";
        kanaToRomajiMap["ジャ"] = "ja"; kanaToRomajiMap["ジュ"] = "ju"; kanaToRomajiMap["ジョ"] = "jo";
        kanaToRomajiMap["ビャ"] = "bya"; kanaToRomajiMap["ビュ"] = "byu"; kanaToRomajiMap["ビョ"] = "byo";
        kanaToRomajiMap["ピャ"] = "pya"; kanaToRomajiMap["ピュ"] = "pyu"; kanaToRomajiMap["ピョ"] = "pyo";
        
        // 外来語用（一部）
        kanaToRomajiMap["ティ"] = "ti"; kanaToRomajiMap["ディ"] = "di";
        kanaToRomajiMap["シェ"] = "she"; kanaToRomajiMap["ジェ"] = "je";
        kanaToRomajiMap["チェ"] = "che";
        kanaToRomajiMap["ファ"] = "fa"; kanaToRomajiMap["フィ"] = "fi"; kanaToRomajiMap["フェ"] = "fe"; kanaToRomajiMap["フォ"] = "fo";
        kanaToRomajiMap["ヴァ"] = "va"; kanaToRomajiMap["ヴィ"] = "vi"; kanaToRomajiMap["ヴ"] = "vu"; kanaToRomajiMap["ヴェ"] = "ve"; kanaToRomajiMap["ヴォ"] = "vo";
    }

    std::string Converter::toRomaji(const std::string& kana) {
        initKanaToRomaji();
        std::string result = "";
        std::string current = kana;

        while (!current.empty()) {
            // 2文字マッチ（拗音など）
            if (current.length() >= 6) { // UTF-8で2文字は6バイト以上
                // 先頭2文字（UTF-8文字境界を考慮する必要があるが、ここでは単純にバイト数で切り出すのは危険）
                // UTF-8の1文字のバイト数を判定する簡易ロジック
                int len1 = 1;
                unsigned char c1 = (unsigned char)current[0];
                if (c1 >= 0xF0) len1 = 4;
                else if (c1 >= 0xE0) len1 = 3;
                else if (c1 >= 0xC0) len1 = 2;

                if (current.length() >= len1) {
                    int len2 = 1;
                    unsigned char c2 = (unsigned char)current[len1];
                    if (c2 >= 0xF0) len2 = 4;
                    else if (c2 >= 0xE0) len2 = 3;
                    else if (c2 >= 0xC0) len2 = 2;

                    if (current.length() >= len1 + len2) {
                        std::string sub2 = current.substr(0, len1 + len2);
                        if (kanaToRomajiMap.count(sub2)) {
                            result += kanaToRomajiMap[sub2];
                            current = current.substr(len1 + len2);
                            continue;
                        }
                    }
                }
            }

            // 1文字マッチ
            int len1 = 1;
            unsigned char c1 = (unsigned char)current[0];
            if (c1 >= 0xF0) len1 = 4;
            else if (c1 >= 0xE0) len1 = 3;
            else if (c1 >= 0xC0) len1 = 2;

            std::string sub1 = current.substr(0, len1);
            
            // 促音（っ、ッ）の処理
            if (sub1 == "っ" || sub1 == "ッ") {
                // 次の文字の子音を重ねる
                if (current.length() > len1) {
                    // 次の文字を取得してローマ字変換
                    // 再帰的に次の文字を変換して、その先頭文字を取得するのはコストが高い
                    // ここでは簡易的に、次の文字が変換可能ならそのローマ字の先頭文字を使う
                    std::string nextStr = current.substr(len1);
                    // 次が2文字マッチするか確認
                    // ... (省略: 簡易実装のため、次の1文字だけ見る)
                    
                    int nextLen = 1;
                    unsigned char nc = (unsigned char)nextStr[0];
                    if (nc >= 0xF0) nextLen = 4;
                    else if (nc >= 0xE0) nextLen = 3;
                    else if (nc >= 0xC0) nextLen = 2;
                    
                    std::string nextChar = nextStr.substr(0, nextLen);
                    
                    // 次の文字が変換可能か
                    std::string nextRomaji = "";
                    // 拗音チェック
                    if (nextStr.length() >= nextLen) {
                         // 次の次の文字
                         // ... 複雑になるので、一旦次の1文字だけで判定
                         if (kanaToRomajiMap.count(nextChar)) {
                             nextRomaji = kanaToRomajiMap[nextChar];
                         }
                    }
                    
                    if (!nextRomaji.empty()) {
                        // 子音を重ねる（母音の場合は重ねないが、通常「っ」の後は子音）
                        // "a", "i", "u", "e", "o" でなければ重ねる
                        char firstChar = nextRomaji[0];
                        if (std::string("aiueo").find(firstChar) == std::string::npos) {
                            result += firstChar;
                            current = current.substr(len1); // "っ"だけ消費、次はループで処理
                            continue;
                        }
                    }
                }
                // 次がない、または母音の場合は "xtu"
                result += "xtu";
                current = current.substr(len1);
                continue;
            }

            if (kanaToRomajiMap.count(sub1)) {
                result += kanaToRomajiMap[sub1];
            } else {
                // 変換できない文字はそのまま（漢字など）
                // ただしTypingJudgeはASCIIを期待するので、これはエラーになる可能性が高い
                // ここでは無視するか、そのまま追加するか
                // result += sub1; 
                // 無視すると進まなくなるので、'?'などを入れるか
            }
            current = current.substr(len1);
        }
        return result;
    }

} // namespace RomajiConverter
