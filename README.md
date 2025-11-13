# Typinger

Typingerは、自作キーボードのテスト用に開発されたオープンソースのタイピング練習ソフトウェアです。
日本語のローマ字入力練習に対応し、詳細な統計情報とCSV出力機能を備えています。

## 特徴

### 🎯 タイピング機能
- **日本語ローマ字入力対応**: ひらがな文をローマ字で入力
- **リアルタイム判定**: 正誤判定とフィードバック
- **カスタムシナリオ**: JSON形式でタイピング文を自由に設定

### 📊 統計機能
- **基本統計**: WPM（Words Per Minute）、CPM（Characters Per Minute）、正答率
- **詳細分析**: キー間隔、Backspace回数、かな別入力時間
- **リアルタイム表示**: タイピング完了時に統計情報を画面表示

### 📁 CSV出力機能
- **イベントCSV**: 全キーイベント（KEY_DOWN/KEY_UP/BACKSPACE）をマイクロ秒単位で記録
- **サマリCSV**: セッション全体の統計情報を出力
- **かな別CSV**: 各かなの平均入力時間を記録

## 動作環境

### 対象システム
- **Windows 10/11** (Windows APIを使用)
- Mac OS、Linux、その他のOSは現在未対応

### ビルド要件
- **MSYS2 / MinGW-w64 / UCRT64** 環境
- **g++** (C++17対応)
- **make**

## インストール

### 1. MSYS2のインストール
[MSYS2公式サイト](https://www.msys2.org/)からダウンロードしてインストール

### 2. 開発ツールのインストール
```bash
# UCRT64環境を起動
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
```

### 3. リポジトリのクローン
```bash
git clone https://github.com/FreenaKeys/typinger.git
cd typinger
```

### 4. ビルド
```bash
make
```

ビルドが成功すると`main.exe`が生成されます。

## 使い方

### 基本的な使い方

```bash
./main.exe
```

メニューが表示されるので、`1`を選択してタイピングを開始します。

### タイピング中の操作
- **文字入力**: 表示されたひらがなをローマ字で入力
- **ESCキー**: タイピングを中断して統計表示
- **ハイフンキー (-)**: タイピングを完了して統計表示
- **Backspace**: 入力ミスを修正

### 完了パターン
1. **正常完了**: 全文字を入力し終える
2. **ESC中断**: ESCキーで途中終了
3. **ハイフンキー完了**: ハイフンキーで任意のタイミングで完了

## シナリオファイル

タイピングするテキストは`scenario/`ディレクトリ内のJSONファイルで管理されます。

### シナリオファイルの形式

```json
{
  "title": "シナリオのタイトル",
  "sentences": [
    "これはダミーです",
    "こんにちは世界",
    "タイピング練習"
  ]
}
```

### カスタムシナリオの作成

`scenario/`ディレクトリに新しいJSONファイルを作成してください。

**例: `scenario/mytext.json`**
```json
{
  "title": "オリジナル練習",
  "sentences": [
    "あいうえお",
    "かきくけこ",
    "さしすせそ"
  ]
}
```

## CSV出力

タイピング完了時に自動的に`output/`ディレクトリにCSVファイルが生成されます。

### 出力ファイル

#### 1. イベントCSV (`typing_events_YYYYMMDD_HHMMSS.csv`)
全キーイベントの詳細記録

**フォーマット:**
```csv
timestamp_us,event_type,vk_code,scan_code,character,is_correct,inter_key_time_us,note
32034289195,KEY_DOWN,75,0,k,1,1457023,
32034306591,KEY_UP,75,0,,0,0,
```

**カラム説明:**
- `timestamp_us`: タイムスタンプ（マイクロ秒）
- `event_type`: イベントタイプ（KEY_DOWN/KEY_UP/BACKSPACE）
- `vk_code`: 仮想キーコード
- `scan_code`: スキャンコード
- `character`: 入力文字
- `is_correct`: 正誤フラグ（1=正解, 0=不正解）
- `inter_key_time_us`: 前のキーからの時間間隔（マイクロ秒）
- `note`: 備考

#### 2. サマリCSV (`typing_summary_YYYYMMDD_HHMMSS.csv`)
セッション全体の統計情報

**フォーマット:**
```csv
metric,value,unit
total_duration,9056419,microseconds
total_duration_sec,9.05642,seconds
total_key_count,16,keys
correct_key_count,15,keys
incorrect_key_count,1,keys
backspace_count,1,keys
accuracy,93.75,percent
wpm_total,21.20,words_per_minute
wpm_correct,19.88,words_per_minute
cpm_total,106.00,chars_per_minute
cpm_correct,99.38,chars_per_minute
avg_inter_key_interval,506.60,milliseconds
min_inter_key_interval,138.97,milliseconds
max_inter_key_interval,2565.54,milliseconds
```

#### 3. かな別CSV (`typing_kana_YYYYMMDD_HHMMSS.csv`)
各かなの平均入力時間

**フォーマット:**
```csv
kana,avg_input_time_ms
こ,363.47
だ,202.38
は,176.44
```

### CSV活用例
- **入力パターン分析**: イベントCSVから打鍵リズムを分析
- **弱点特定**: かな別CSVから入力が遅いかなを特定
- **成長記録**: サマリCSVで日々の進捗を追跡
- **キーボード評価**: 自作キーボードの使用感を定量評価

## 開発

### プロジェクト構造

```
typinger/
├── main.cpp              # メインプログラム
├── makefile              # ビルド設定
├── README.md             # このファイル
├── core/                 # コアモジュール
│   ├── csv_logger.cpp/h      # CSV出力
│   ├── input_recorder.cpp/h  # 入力記録
│   ├── romaji_converter.cpp/h # ローマ字変換
│   ├── statistics.cpp/h      # 統計計算
│   └── typing_judge.cpp/h    # タイピング判定
├── helper/               # ヘルパーモジュール
│   ├── json_helper.cpp/h     # JSON解析
│   └── WinAPI/
│       ├── terminal.cpp/h    # ターミナル制御
│       └── timer.cpp/h       # タイマー
├── scenario/             # シナリオファイル
│   └── scenarioexample.json
├── tests/                # 単体テスト
│   ├── csv_logger_test.cpp
│   ├── romaji_converter_test.cpp
│   ├── statistics_test.cpp
│   └── typing_judge_test.cpp
└── output/               # CSV出力先（自動生成）
```

### テストの実行

```bash
# CSVロガーテスト
make csv-logger-test
./csv_logger_test.exe

# 統計モジュールテスト
make statistics-test
./statistics_test.exe

# ローマ字変換テスト
make romaji-test
./romaji_converter_test.exe

# タイピング判定テスト
make typing-test
./typing_judge_test.exe
```

### ビルドターゲット

```bash
make            # メインプログラムのビルド
make clean      # ビルド成果物を削除
make csv-logger-test    # CSVロガーテストをビルド
make statistics-test    # 統計テストをビルド
make romaji-test        # ローマ字変換テストをビルド
make typing-test        # タイピング判定テストをビルド
```

## 開発履歴

### Phase 0: 仕様策定
- プロジェクト全体の設計

### Phase 1: 基盤実装
- タイマー、入力記録、UI統合

### Phase 2: ローマ字入力対応
- Phase 2-1: ローマ字変換エンジン
- Phase 2-2: タイピング判定ロジック
- Phase 2-3: typing_mode統合

### Phase 3: 統計機能
- Phase 3-1: 基本統計（WPM/CPM/キー間隔）
- Phase 3-2: かな別入力時間データ構造
- Phase 3-3: 統計統合（基本）
- Phase 3-4: かな別入力時間記録とUI改善

### Phase 4: CSV出力
- Phase 4-1: イベントCSV出力
- Phase 4-2: セッション統計CSV出力
- Phase 4-3: CSV統合

### Phase 5: ポリッシュ
- ドキュメント整備
- 追加シナリオ
- 最終調整

## トラブルシューティング

### ビルドエラー: `make: command not found`
**原因:** makeがインストールされていない  
**解決策:**
```bash
pacman -S mingw-w64-ucrt-x86_64-make
```

### ビルドエラー: `fatal error: windows.h: No such file or directory`
**原因:** MinGW/UCRT64環境が正しく設定されていない  
**解決策:** MSYS2のUCRT64シェルを使用してください

### 実行エラー: シナリオファイルが見つからない
**原因:** `scenario/`ディレクトリまたはJSONファイルが存在しない  
**解決策:**
```bash
mkdir -p scenario
# scenarioexample.jsonを作成
```

### CSV出力されない
**原因:** `output/`ディレクトリの作成権限がない  
**解決策:** プログラムが自動作成しますが、手動で作成することもできます
```bash
mkdir output
```

## 貢献

プルリクエストを歓迎します！以下の方針に従ってください：

1. フィーチャーブランチを作成
2. 変更をコミット
3. テストを実行して全てPASSすることを確認
4. プルリクエストを作成

## ライセン

現在、ライセンスは未設定です。

## 作者

- **FreenaKeys** - [GitHub](https://github.com/FreenaKeys)

## 謝辞

このプロジェクトは自作キーボードのテストを目的として開発されました。
タイピング練習ソフトウェアとしても活用できます。
