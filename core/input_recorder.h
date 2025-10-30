#pragma once
// input_recorder.h
// 入力イベント記録システム - キー入力の詳細なタイミングを記録
// 
// 用語解説:
// - イベント(Event): キーを押した/離したなどの操作の記録
// - タイムスタンプ(Timestamp): その操作が行われた正確な時刻（マイクロ秒単位）
// - VK (Virtual Key): Windowsの仮想キーコード（キーボードの各キーを識別する番号）

#include <cstdint>
#include <vector>
#include <string>

namespace InputRecorder {

    // イベントの種類
    enum class EventType {
        KEY_DOWN,       // キーを押した瞬間
        KEY_UP,         // キーを離した瞬間
        BACKSPACE,      // Backspaceキーの押下
        CORRECTION      // 修正操作（Backspaceで文字を削除した確定イベント）
    };

    // 1つの入力イベントの詳細情報
    struct InputEvent {
        EventType type;              // イベントの種類
        uint64_t timestamp_us;       // イベント発生時刻（マイクロ秒）
        int vk_code;                 // 仮想キーコード（どのキーが押されたか）
        int scan_code;               // スキャンコード（物理的なキーの位置）
        char character;              // 入力された文字（該当する場合）
        bool is_correct;             // 正しい入力かどうか（判定ロジックで設定）
        uint64_t inter_key_time_us;  // 直前のキーアップからこのキーダウンまでの時間
        std::string note;            // 追加情報（デバッグやログ用）

        // コンストラクタ（初期化用）
        InputEvent();
        InputEvent(EventType t, uint64_t ts, int vk, int scan, char ch = '\0');
    };

    // 入力イベント記録クラス
    class Recorder {
    private:
        std::vector<InputEvent> events_;     // 記録したイベントのリスト
        uint64_t session_start_us_;          // セッション開始時刻
        uint64_t last_keyup_time_us_;        // 最後にキーが離された時刻
        bool recording_;                     // 記録中かどうかのフラグ

    public:
        Recorder();

        // セッション（1回の練習）の開始
        void startSession();

        // セッションの終了
        void endSession();

        // キーダウンイベントを記録
        // vk: 仮想キーコード, scan: スキャンコード, ch: 文字
        void recordKeyDown(int vk, int scan, char ch = '\0');

        // キーアップイベントを記録
        void recordKeyUp(int vk, int scan);

        // Backspaceイベントを記録
        void recordBackspace();

        // 修正イベントを記録（Backspaceで削除が確定した時）
        // correction_time_us: 修正にかかった時間
        void recordCorrection(uint64_t correction_time_us);

        // 最後に記録したイベントに正誤フラグを設定
        void setLastEventCorrectness(bool is_correct);

        // 記録中かどうか
        bool isRecording() const;

        // 記録したイベント数
        size_t getEventCount() const;

        // 全イベントの取得（読み取り専用）
        const std::vector<InputEvent>& getEvents() const;

        // セッションの経過時間（マイクロ秒）
        uint64_t getSessionDuration() const;

        // 記録のクリア
        void clear();
    };

} // namespace InputRecorder
