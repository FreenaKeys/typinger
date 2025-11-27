#ifndef WINDOWMAKER_H
#define WINDOWMAKER_H

#include <string>

/**
 * @brief UTF-8 文字列を UTF-16 (wchar_t) に変換
 * @param utf8 UTF-8 エンコードされた std::string
 * @return UTF-16 エンコードされた std::wstring
 */
std::wstring utf8ToUtf16(const std::string& utf8);

/**
 * @brief Windows API に準拠した確認ダイアログを生成（日本語対応）
 * @param title ダイアログのタイトル（UTF-8）
 * @param content ダイアログの内容（メッセージ、UTF-8）
 * @return true if OK button pressed, false if Cancel button pressed
 */
bool showConfirmDialog(const std::string& title, const std::string& content);

#endif // WINDOWMAKER_H
