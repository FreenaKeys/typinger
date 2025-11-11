#pragma once

// csv_logger.h
// CSV出力ロガー
//
// 用語解説:
// - CSV (Comma-Separated Values): カンマ区切りのテキストファイル形式
// - イベントCSV: キー入力イベントの詳細を記録
// - サマリCSV: セッション全体の統計情報を記録

#include <string>
#include <vector>
#include "input_recorder.h"
#include "statistics.h"

namespace CSVLogger {

    // イベントCSV出力
    // recorder: InputRecorderインスタンス
    // outputDir: 出力ディレクトリ（デフォルト: "output"）
    // 戻り値: 出力ファイルパス（失敗時は空文字列）
    std::string writeEventCSV(const InputRecorder::Recorder& recorder, 
                              const std::string& outputDir = "output");

    // サマリCSV出力（Phase 4-2で実装）
    // stats: StatisticsDataインスタンス
    // outputDir: 出力ディレクトリ（デフォルト: "output"）
    // 戻り値: 出力ファイルパス（失敗時は空文字列）
    std::string writeSummaryCSV(const Statistics::StatisticsData& stats,
                                const std::string& outputDir = "output");

    // 現在時刻からファイル名を生成
    // prefix: ファイル名のプレフィックス（例: "typing_events"）
    // 戻り値: "prefix_YYYYMMDD_HHMMSS.csv"
    std::string generateFilename(const std::string& prefix);

} // namespace CSVLogger
