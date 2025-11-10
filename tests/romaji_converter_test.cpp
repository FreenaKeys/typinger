// romaji_converter_test.cpp
// ローマ字変換器のユニットテスト

#include "../core/romaji_converter.h"
#include <iostream>
#include <cassert>

using namespace RomajiConverter;

void test_basic_conversion() {
    std::cout << "Test: Basic conversion (基本変換)..." << std::endl;
    
    Converter conv;
    
    // 清音
    ConvertResult r1 = conv.convert("a");
    assert(r1.status == ConvertStatus::MATCHED);
    assert(r1.kana == "あ");
    assert(r1.consumed == "a");
    
    ConvertResult r2 = conv.convert("ka");
    assert(r2.status == ConvertStatus::MATCHED);
    assert(r2.kana == "か");
    
    ConvertResult r3 = conv.convert("shi");
    assert(r3.status == ConvertStatus::MATCHED);
    assert(r3.kana == "し");
    
    std::cout << "  PASS" << std::endl;
}

void test_multi_variant() {
    std::cout << "Test: Multi-variant (複数表記対応)..." << std::endl;
    
    Converter conv;
    
    // し = shi / si
    ConvertResult r1 = conv.convert("shi");
    ConvertResult r2 = conv.convert("si");
    assert(r1.kana == "し");
    assert(r2.kana == "し");
    assert(r1.kana == r2.kana);
    
    // ち = chi / ti
    ConvertResult r3 = conv.convert("chi");
    ConvertResult r4 = conv.convert("ti");
    assert(r3.kana == "ち");
    assert(r4.kana == "ち");
    
    // つ = tsu / tu
    ConvertResult r5 = conv.convert("tsu");
    ConvertResult r6 = conv.convert("tu");
    assert(r5.kana == "つ");
    assert(r6.kana == "つ");
    
    // ふ = fu / hu
    ConvertResult r7 = conv.convert("fu");
    ConvertResult r8 = conv.convert("hu");
    assert(r7.kana == "ふ");
    assert(r8.kana == "ふ");
    
    // じ = ji / zi
    ConvertResult r9 = conv.convert("ji");
    ConvertResult r10 = conv.convert("zi");
    assert(r9.kana == "じ");
    assert(r10.kana == "じ");
    
    std::cout << "  PASS" << std::endl;
}

void test_dakuon_handakuon() {
    std::cout << "Test: Dakuon/Handakuon (濁音・半濁音)..." << std::endl;
    
    Converter conv;
    
    // 濁音
    ConvertResult r1 = conv.convert("ga");
    assert(r1.kana == "が");
    
    ConvertResult r2 = conv.convert("ji");
    assert(r2.kana == "じ");
    
    // 半濁音
    ConvertResult r3 = conv.convert("pa");
    assert(r3.kana == "ぱ");
    
    ConvertResult r4 = conv.convert("po");
    assert(r4.kana == "ぽ");
    
    std::cout << "  PASS" << std::endl;
}

void test_youon() {
    std::cout << "Test: Youon (拗音)..." << std::endl;
    
    Converter conv;
    
    // きゃ、しゃ、ちゃ
    ConvertResult r1 = conv.convert("kya");
    assert(r1.kana == "きゃ");
    
    ConvertResult r2 = conv.convert("sha");
    assert(r2.kana == "しゃ");
    
    ConvertResult r3 = conv.convert("cha");
    assert(r3.kana == "ちゃ");
    
    // 複数表記
    ConvertResult r4 = conv.convert("sya");
    assert(r4.kana == "しゃ");
    
    ConvertResult r5 = conv.convert("tya");
    assert(r5.kana == "ちゃ");
    
    std::cout << "  PASS" << std::endl;
}

void test_sokuon() {
    std::cout << "Test: Sokuon (促音 - っ)..." << std::endl;
    
    Converter conv;
    
    // x/l表記
    ConvertResult r1 = conv.convert("xtu");
    assert(r1.kana == "っ");
    
    ConvertResult r2 = conv.convert("ltu");
    assert(r2.kana == "っ");
    
    // 子音重複 → っ
    ConvertResult r3 = conv.convert("kk");
    assert(r3.status == ConvertStatus::MATCHED);
    assert(r3.kana == "っ");
    assert(r3.consumed == "k");
    assert(r3.remaining == "k");
    
    std::cout << "  PASS" << std::endl;
}

void test_n_special() {
    std::cout << "Test: N special cases (ん の特殊処理)..." << std::endl;
    
    Converter conv;
    
    // nn → ん
    ConvertResult r1 = conv.convert("nn");
    assert(r1.kana == "ん");
    
    // n + 子音 → ん
    ConvertResult r2 = conv.convert("nk");
    assert(r2.status == ConvertStatus::MATCHED);
    assert(r2.kana == "ん");
    assert(r2.consumed == "n");
    assert(r2.remaining == "k");
    
    std::cout << "  PASS" << std::endl;
}

void test_partial_match() {
    std::cout << "Test: Partial match (部分一致・入力途中)..." << std::endl;
    
    Converter conv;
    
    // k → PARTIAL (ka, ki, ku等が候補)
    ConvertResult r1 = conv.convert("k");
    assert(r1.status == ConvertStatus::PARTIAL);
    
    // sh → PARTIAL (sha, shi, sho等が候補)
    ConvertResult r2 = conv.convert("sh");
    assert(r2.status == ConvertStatus::PARTIAL);
    
    // ky → PARTIAL (kya, kyu, kyo等が候補)
    ConvertResult r3 = conv.convert("ky");
    assert(r3.status == ConvertStatus::PARTIAL);
    
    std::cout << "  PASS" << std::endl;
}

void test_greedy_conversion() {
    std::cout << "Test: Greedy conversion (貪欲変換)..." << std::endl;
    
    Converter conv;
    std::string remaining;
    
    // "aiueo" → "あいうえお"
    std::string result1 = conv.convertGreedy("aiueo", remaining);
    assert(result1 == "あいうえお");
    assert(remaining == "");
    
    // "arigatou" → "ありがとう"
    std::string result2 = conv.convertGreedy("arigatou", remaining);
    assert(result2 == "ありがとう");
    assert(remaining == "");
    
    // "kyouto" → "きょうと"
    std::string result3 = conv.convertGreedy("kyouto", remaining);
    assert(result3 == "きょうと");
    assert(remaining == "");
    
    // "gakkou" → "がっこう" (kが2つで促音)
    std::string result4 = conv.convertGreedy("gakkou", remaining);
    assert(result4 == "がっこう");
    assert(remaining == "");
    
    // "sannpo" → "さんぽ" (n+子音→ん)
    std::string result5 = conv.convertGreedy("sannpo", remaining);
    assert(result5 == "さんぽ");
    assert(remaining == "");
    
    std::cout << "  PASS" << std::endl;
}

void test_no_match() {
    std::cout << "Test: No match (変換不可)..." << std::endl;
    
    Converter conv;
    
    // 存在しない組み合わせ
    ConvertResult r1 = conv.convert("xyz");
    assert(r1.status == ConvertStatus::NO_MATCH);
    
    std::cout << "  PASS" << std::endl;
}

void test_table_size() {
    std::cout << "Test: Table size (テーブルサイズ)..." << std::endl;
    
    Converter conv;
    size_t size = conv.getTableSize();
    
    std::cout << "  Table size: " << size << " entries" << std::endl;
    assert(size > 100); // 最低でも100エントリ以上あるはず
    
    std::cout << "  PASS" << std::endl;
}

int main() {
    std::cout << "=== Romaji Converter Unit Tests ===" << std::endl;
    std::cout << std::endl;

    test_basic_conversion();
    test_multi_variant();
    test_dakuon_handakuon();
    test_youon();
    test_sokuon();
    test_n_special();
    test_partial_match();
    test_greedy_conversion();
    test_no_match();
    test_table_size();

    std::cout << std::endl;
    std::cout << "All tests passed! ✓" << std::endl;
    return 0;
}
