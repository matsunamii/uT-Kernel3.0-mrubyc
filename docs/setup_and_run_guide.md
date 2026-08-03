# セットアップ手順書

## 1. 概要
本書では、本プロジェクトを STM32CubeIDE にインポートし、NUCLEO-H533RE 上で実行するまでの手順を説明します。

本プロジェクトには、動作確認用のサンプルプログラムおよびデモアプリケーションとして、以下のRubyプログラムを用意しています。

Application/RubyScripts/
├── clock.rb
├── sevenseg_led.rb
├── stop_watch.rb
├── led.rb
└── print.rb

## 2. 必要なもの

| 項目 | 内容 |
|---|---|
| マイコンボード | NUCLEO-H533RE |
| IDE | STM32CubeIDE |
| RTOS | μT-Kernel 3.0 BSP2 |
| ターミナルソフト | Tera Term など |
| 外部ハードウェア | 7セグメントLED |

## 3. ダウンロードと展開

GitHub のリポジトリページから ZIP ファイルをダウンロードします。

1. `Code` ボタンを押す
2. `Download ZIP` を選択する
3. ZIP ファイルを任意のディレクトリに展開する

展開先のパスには、日本語や空白を含めないことを推奨します。

例：

```text
C:\workspace\mtk3bsp2_stm32h533
```

## 4. STM32CubeIDE へのインポート

1. STM32CubeIDE を起動する
2. `File` → `Import` を選択する
3. `General` → `Existing Projects into Workspace` を選択する
4. `Select root directory` に展開したプロジェクトフォルダを指定する
5. プロジェクト名が表示されたことを確認し、`Finish` を押す

## 5. ビルド

1. Project Explorer でプロジェクト名を右クリックする
2. `Build Project` を選択する
3. コンソールに `Build Finished.` と表示されれば成功

## 6. 実行

1. NUCLEO-H533RE を PC に USB 接続する
2. `Run` → `Debug Configurations` を開く
3. `STM32 C/C++ Application` から対象プロジェクトを選択する
4. `Debug` を押して書き込む
5. デバッグ画面で `Resume` を押して実行する

実行後、ボード上 LED が点滅します。

## 7. シリアル出力の確認

Tera Term などで NUCLEO-H533RE の仮想 COM ポートを開きます。

| 項目 | 設定 |
|---|---|
| 速度 | 115200 |
| データ | 8 bit |
| パリティ | none |
| ストップビット | 1 bit |
| フロー制御 | none |

## 8. サンプルプログラム
本プロジェクトが正常に動作することを確認するため、`led.rb`と`print.rb`を用意しています。

初期状態では、この2つのRubyプログラムがRTOSタスクとして実行されます。

- `led.rb`：ボード上LEDを一定周期で点滅させます。
- `print.rb`：LEDの状態をシリアルポートへ出力します。

プロジェクトをそのままSTM32CubeIDEへインポートしてビルド・実行することで、これらの動作を確認できます。

## 9. デモ

本プロジェクトには、以下のデモアプリケーションを用意しています。

### デジタル時計
現在時刻をRubyプログラムから設定してそこからの時刻を7セグメントLEDへ表示します。
以下のファイルを次のように変更してビルド・実行してください。
/Application/app_main.c
mruby_task_info_t mruby_tasks[] = {
  {
    .script = sevenseg_led,
    ・・・
  },
  {
    .script = clock,
    ・・・
    }
};

### ストップウォッチ
ボタン操作により開始・停止・リセットを行います。
以下のファイルを次のように変更してビルド・実行してください。
/Application/app_main.c
mruby_task_info_t mruby_tasks[] = {
  {
    .script = sevenseg_led,
    ・・・
  },
  {
    .script = stop_watch,
    ・・・
  }
};

これらのデモを通して、Rubyによる簡潔な記述で、μT-Kernel 3.0のタスク管理や時間管理を活用した組込みアプリケーションを開発できることを確認できます。