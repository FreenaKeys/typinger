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

    // 実験データ構造体（Phase 6で追加）
    struct ExperimentData {
        std::string subject_id;      // 被験者ID (例: "SK", "MT")
        int session_number;          // セッション番号 (1-14)
        std::string layout_type;     // 配列タイプ ("new" or "old")
        int test_number;             // テスト番号 (1-6)
        
        ExperimentData() 
            : subject_id(""), session_number(0), layout_type(""), test_number(0) {}
        
        ExperimentData(const std::string& id, int session, const std::string& layout, int test)
            : subject_id(id), session_number(session), layout_type(layout), test_number(test) {}
    };

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
    
    // サマリCSV出力（実験データ付き - Phase 6で追加）
    // stats: StatisticsDataインスタンス
    // expData: 実験データ（被験者ID、セッション番号、配列タイプ等）
    // outputDir: 出力ディレクトリ（デフォルト: "output"）
    // 戻り値: 出力ファイルパス（失敗時は空文字列）
    std::string writeSummaryCSV(const Statistics::StatisticsData& stats,
                                const ExperimentData& expData,
                                const std::string& outputDir = "output");

    // 現在時刻からファイル名を生成
    // prefix: ファイル名のプレフィックス（例: "typing_events"）
    // 戻り値: "prefix_YYYYMMDD_HHMMSS.csv"
    std::string generateFilename(const std::string& prefix);

    // サマリCSV追記保存（実験データ付き - Phase 6で追加）
    // stats: StatisticsDataインスタンス
    // expData: 実験データ（被験者ID、セッション番号、配列タイプ等）
    // outputDir: 出力ディレクトリ（デフォルト: "output"）
    // 戻り値: 出力ファイルパス（失敗時は空文字列）
    // 注意: 被験者IDごとに1つのCSVファイルに追記する
    std::string appendSummaryCSV(const Statistics::StatisticsData& stats,
                                 const ExperimentData& expData,
                                 const std::string& outputDir = "output");

    // 重複セッションチェック（Phase 6で追加）
    // filepath: チェック対象のCSVファイルパス
    // expData: 実験データ
    // 戻り値: 重複している場合true、重複していない場合false
    bool isDuplicateSession(const std::string& filepath,
                           const ExperimentData& expData);

    // 被験者IDから統合CSVファイル名を生成（Phase 6で追加）
    // subjectId: 被験者ID（例: "SK"）
    // 戻り値: "typing_summary_[ID].csv"（例: "typing_summary_SK.csv"）
    std::string getConsolidatedFilename(const std::string& subjectId);

} // namespace CSVLogger
