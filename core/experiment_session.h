#ifndef EXPERIMENT_SESSION_H
#define EXPERIMENT_SESSION_H

#include <string>

/**
 * @brief 実験セッション情報を管理する構造体
 */
struct ExperimentSession {
    std::string subject_id;      // 被験者ID (SK, MT, IK, KS, KM)
    int session_number;          // セッション番号 (1-14)
    std::string date;            // 実施日 (YYYYMMDD)
    std::string layout_type;     // 配列タイプ (new/old)
    int test_order;              // テスト順序 (1-6)
};

/**
 * @brief 被験者IDの入力と検証
 * @return 検証済みのID文字列
 */
std::string inputSubjectID();

/**
 * @brief セッション番号の入力と検証
 * @return 検証済みのセッション番号 (1-14)
 */
int inputSessionNumber();

/**
 * @brief 実施日の入力
 * @return 日付文字列 (YYYYMMDD形式)
 */
std::string inputDate();

/**
 * @brief 最初に実施する配列タイプの入力と検証
 * @return "new" または "old"
 */
std::string inputFirstLayoutType();

/**
 * @brief 被験者IDの妥当性チェック
 * @param id 被験者ID
 * @return 妥当ならtrue
 */
bool isValidSubjectID(const std::string& id);

/**
 * @brief セッション番号の妥当性チェック
 * @param session_num セッション番号
 * @return 妥当ならtrue
 */
bool isValidSessionNumber(int session_num);

/**
 * @brief 配列タイプ番号の妥当性チェック
 * @param type_num 配列タイプ番号 (1 or 2)
 * @return 妥当ならtrue
 */
bool isValidLayoutTypeNumber(int type_num);

/**
 * @brief 指定されたテスト順序での配列タイプを取得
 * @param first_layout 最初の配列タイプ ("new" or "old")
 * @param test_order テスト順序 (1-6)
 * @return 該当する配列タイプ
 */
std::string getLayoutTypeForTest(const std::string& first_layout, int test_order);

#endif // EXPERIMENT_SESSION_H
