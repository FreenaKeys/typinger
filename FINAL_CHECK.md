# Typinger 最終動作確認手順書

## Phase 5: ポリッシュ - 最終確認

このドキュメントは、Typingerの全機能が正常に動作することを確認するための手順書です。

---

## 1. ビルド確認

### 1.1 クリーンビルド
```bash
cd /c/typinger
make clean
make
```

**期待結果:**
- ✅ エラーなくコンパイル完了
- ✅ `main.exe`が生成される
- ✅ 警告なし

---

## 2. 基本機能確認

### 2.1 プログラム起動
```bash
./main.exe
```

**期待結果:**
- ✅ シナリオファイル選択画面が表示される
- ✅ 利用可能なシナリオファイルのリストが表示される
  ```
  Select a file to view at startup (press number, or ESC to skip):
  1: beginner.json
  2: daily.json
  3: programming.json
  4: scenarioexample.json
  5: special_sounds.json
  ```

### 2.2 タイピング開始
**操作:** ESCキーを押してファイル選択をスキップ

**期待結果:**
- ✅ デフォルトシナリオ（scenarioexample.json）が読み込まれる
- ✅ シナリオテキスト「これはダミーです」が表示される
- ✅ ローマ字入力欄が表示される
- ✅ カーソルが正しい位置に表示される

**注意:** 
- 起動時のファイル選択で番号を押すと、そのJSONファイルの内容が画面に表示されます（プレビュー機能）
- タイピングを開始するには、ESCキーを押してファイル選択をスキップしてください

### 2.3 文字入力
デフォルトシナリオ「これはダミーです」をローマ字で入力
- "korehadami-desu" と入力

**期待結果:**
- ✅ 正しい文字を入力すると画面に表示される
- ✅ リアルタイムで判定される
- ✅ 入力した文字が画面に表示される
- ✅ カーソルが進む

**注意:** 
- 現在のバージョンでは、正解/不正解の色分け表示機能は実装されていない可能性があります
- 正しい入力順序で文字が入力されているか確認してください

### 2.4 Backspace機能
間違った文字を入力後、Backspaceキーを押す

**期待結果:**
- ✅ 最後に入力した文字が削除される
- ✅ カーソル位置が戻る
- ✅ 正しい文字を再入力できる

### 2.5 正常完了
最後まで入力して完了

**期待結果:**
- ✅ "*** COMPLETED! ***"メッセージが表示される
- ✅ 統計情報が表示される（Accuracy、WPM、CPM、キー間隔、Backspace数）
- ✅ かな別入力時間のTop 5が表示される
- ✅ CSVファイル保存パスが表示される（2つ）
  - Event CSV saved: output/typing_events_YYYYMMDD_HHMMSS.csv
  - Summary CSV saved: output/typing_summary_YYYYMMDD_HHMMSS.csv

### 2.6 ESC中断
タイピング中にESCキーを押す

**期待結果:**
- ✅ タイピングが中断される
- ✅ "=== Typing Statistics (Interrupted) ==="と表示される
- ✅ 統計情報が表示される
- ✅ CSVファイル保存パスが表示される（2つ）
- ✅ 3秒後に自動的に終了する

### 2.7 ハイフンキー完了
タイピング中にハイフンキー(-)を押す

**期待結果:**
- ✅ タイピングが完了する
- ✅ "*** COMPLETED! ***"メッセージが表示される
- ✅ 統計情報が表示される
- ✅ CSVファイル保存パスが表示される（2つ）

---

## 3. CSV出力確認

### 3.1 出力ファイル確認
```bash
ls output
```

**期待結果:**
- ✅ `typing_events_YYYYMMDD_HHMMSS.csv`ファイルが存在
- ✅ `typing_summary_YYYYMMDD_HHMMSS.csv`ファイルが存在
- ✅ `typing_kana_YYYYMMDD_HHMMSS.csv`ファイルが存在（かなデータがある場合）
- ✅ 同一セッションの3ファイルのタイムスタンプが一致

### 3.2 イベントCSV内容確認
```bash
head -n 10 output/typing_events_20251113_*.csv
```

**期待されるフォーマット:**
```csv
timestamp_us,event_type,vk_code,scan_code,character,is_correct,inter_key_time_us,note
32034289195,KEY_DOWN,75,0,k,1,1457023,
32034306591,KEY_UP,75,0,,0,0,
```

**確認ポイント:**
- ✅ ヘッダー行が正しい（8カラム）
- ✅ KEY_DOWN/KEY_UPイベントが記録されている
- ✅ タイムスタンプがマイクロ秒単位
- ✅ 仮想キーコードが記録されている
- ✅ 文字が記録されている
- ✅ 正誤フラグが記録されている

### 3.3 サマリCSV内容確認
```bash
head -n 20 output/typing_summary_20251113_*.csv
```

**期待されるフォーマット:**
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

**確認ポイント:**
- ✅ ヘッダー行が正しい（metric,value,unit）
- ✅ 15項目のメトリクスが記録されている
- ✅ 数値フォーマットが正しい（小数点2桁）
- ✅ 単位が正しい（microseconds、keys、percent、words_per_minute、chars_per_minute、milliseconds）

### 3.4 かな別CSV内容確認
```bash
cat output/typing_kana_20251113_*.csv
```

**期待されるフォーマット:**
```csv
kana,avg_input_time_ms
こ,363.47
だ,202.38
は,176.44
```

**確認ポイント:**
- ✅ ヘッダー行が正しい（kana,avg_input_time_ms）
- ✅ かなと平均時間が記録されている
- ✅ 時間が妥当な範囲（100ms～数秒）

---

## 4. シナリオファイル確認

### 4.1 シナリオファイル一覧
```bash
ls scenario
```

**期待結果:**
- ✅ `scenarioexample.json` - デフォルトシナリオ
- ✅ `beginner.json` - 初心者向け練習（50音）
- ✅ `daily.json` - 日常会話練習
- ✅ `programming.json` - プログラミング用語
- ✅ `special_sounds.json` - 拗音・促音練習

### 4.2 各シナリオの動作確認
各シナリオファイルを読み込んでタイピングが正常に動作することを確認

**確認方法:**
（現在のプログラムは自動でシナリオを選択するため、手動でのシナリオ選択機能は未実装）

**期待結果:**
- ✅ すべてのシナリオが正しいJSON形式
- ✅ テキストとルビが対応している

---

## 5. 単体テスト確認

### 5.1 CSVロガーテスト
```bash
make csv-logger-test
./csv_logger_test.exe
```

**期待結果:**
```
=== CSV Logger Unit Tests ===
Test: Filename generation... PASS
Test: Empty event CSV... PASS
Test: Event CSV with data... PASS
Test: CSV format validation... PASS
Test: Directory auto-creation... PASS
Test: Empty summary CSV... PASS
Test: Summary CSV with data... PASS
Test: Summary CSV with kana data... PASS
Test: Summary CSV format validation... PASS

All tests passed! ✓
```

**確認:** ✅ 9/9 PASS

### 5.2 統計モジュールテスト
```bash
make statistics-test
./statistics_test.exe
```

**期待結果:**
```
=== Statistics Module Unit Tests ===
...
All 12 tests passed! ✓
```

**確認:** ✅ 12/12 PASS

### 5.3 ローマ字変換テスト
```bash
make romaji-test
./romaji_converter_test.exe
```

**期待結果:**
```
=== Romaji Converter Unit Tests ===
...
All 10 tests passed! ✓
```

**確認:** ✅ 10/10 PASS

### 5.4 タイピング判定テスト
```bash
make typing-test
./typing_judge_test.exe
```

**期待結果:**
```
=== Typing Judge Unit Tests ===
...
All 9 tests passed! ✓
```

**確認:** ✅ 9/9 PASS

---

## 6. ドキュメント確認

### 6.1 README.md
```bash
cat README.md
```

**確認ポイント:**
- ✅ プロジェクト概要が記載されている
- ✅ 特徴が記載されている
- ✅ インストール手順が記載されている
- ✅ 使い方が記載されている
- ✅ シナリオファイルの形式が記載されている
- ✅ CSV出力の説明が記載されている
- ✅ プロジェクト構造が記載されている
- ✅ 開発履歴が記載されている
- ✅ トラブルシューティングが記載されている

---

## 7. エラー処理確認

### 7.1 不正な入力
タイピング中に無効なキーを押す

**期待結果:**
- ✅ プログラムがクラッシュしない
- ✅ エラーメッセージが表示されるか、無視される

### 7.2 CSV出力エラー
（outputディレクトリの権限を変更してテスト）

**期待結果:**
- ✅ プログラムがクラッシュしない
- ✅ CSVパスが空文字列になる（エラー処理）

---

## 8. パフォーマンス確認

### 8.1 高速入力
非常に速くキーを入力する

**期待結果:**
- ✅ すべてのキーイベントが正しく記録される
- ✅ 画面表示が追従する
- ✅ CSVに全イベントが記録される

### 8.2 長時間セッション
長い文章を入力する

**期待結果:**
- ✅ メモリリークが発生しない
- ✅ パフォーマンスが劣化しない
- ✅ 正しく統計が計算される

---

## 9. 最終チェックリスト

### ビルド
- [ ] `make clean && make`: エラーなし
- [ ] 警告なし
- [ ] `main.exe`生成

### 基本機能
- [ ] プログラム起動
- [ ] タイピング開始
- [ ] 文字入力（正解/不正解判定）
- [ ] Backspace機能
- [ ] 正常完了
- [ ] ESC中断
- [ ] ハイフンキー完了

### CSV出力
- [ ] イベントCSV生成
- [ ] サマリCSV生成
- [ ] かな別CSV生成
- [ ] 同一セッションのタイムスタンプ一致
- [ ] CSVフォーマット正確

### シナリオ
- [ ] scenarioexample.json動作
- [ ] beginner.json存在
- [ ] daily.json存在
- [ ] programming.json存在
- [ ] special_sounds.json存在

### テスト
- [ ] csv_logger_test: 9/9 PASS
- [ ] statistics_test: 12/12 PASS
- [ ] romaji_converter_test: 10/10 PASS
- [ ] typing_judge_test: 9/9 PASS

### ドキュメント
- [ ] README.md充実
- [ ] シナリオファイル説明
- [ ] CSV出力説明
- [ ] トラブルシューティング

---

## 10. 最終確認報告

全ての確認が完了したら、以下のフォーマットで報告してください:

```
Phase 5 最終動作確認結果:

【ビルド】
✅ クリーンビルド成功

【基本機能】
✅ タイピング動作確認（正常完了、ESC中断、ハイフンキー完了）
✅ 文字入力・判定動作確認
✅ Backspace動作確認

【CSV出力】
✅ 3種類のCSV正常生成
✅ CSVフォーマット正確

【シナリオ】
✅ 5つのシナリオファイル作成

【単体テスト】
✅ 全テストPASS（9+12+10+9 = 40テスト）

【ドキュメント】
✅ README.md充実完了

【備考】
なし
```

---

## おめでとうございます！

全ての確認が完了すれば、Typinger v1.0のリリース準備が完了です。
