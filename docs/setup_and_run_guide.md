# セットアップ手順書

## 1. 概要
本書では、本プロジェクトを STM32CubeIDE にインポートし、NUCLEO-H533RE 上で実行するまでの手順を説明します。

本プロジェクトには、動作確認用の Ruby プログラムとして led.rb と print.rb が同梱されています。

Application/RubyScripts/
├── led.rb
└── print.rb

STM32CubeIDE でプロジェクトをビルドし、NUCLEO-H533RE に書き込んで実行すると、以下の2つの Ruby プログラムがRTOSのタスクとして並列実行されます。

led.rb	PA5 に接続されたボード上 LED を周期的に点滅させる
print.rb	PA5 の状態を読み取り、LED の ON/OFF 状態をシリアル出力に表示する

## 2. 必要なもの

| 項目 | 内容 |
|---|---|
| マイコンボード | NUCLEO-H533RE |
| IDE | STM32CubeIDE |
| RTOS | μT-Kernel 3.0 BSP2 |
| ターミナルソフト | Tera Term など |
| 外部ハードウェア | なし |

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

表示例：

```text
LED: ON
LED: OFF
LED: ON
LED: OFF
```

## 8. 動作確認

以下を確認してください。

1. ボード上 LED が点滅する
2. シリアル出力に `LED: ON` と `LED: OFF` が表示される
3. LED の点灯状態とシリアル出力の内容が対応している