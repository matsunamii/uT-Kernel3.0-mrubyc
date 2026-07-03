# 開発者向けガイド

## 1. 本書について

本書では、`μT-Kernel 3.0 × mruby/c リアルタイムスクリプト実行環境` を拡張するための方法を説明します。

主に以下の内容を扱います。

- 新しい Ruby プログラムを追加する方法
- Ruby から呼び出す C メソッドを追加する方法
- Ruby プログラムを RTOS タスクとして追加する方法
- mruby/c VM のタイムスライス実行の仕組み
- アラームによるプリエンプション制御の仕組み

---

## 2. プロジェクト構成

本プロジェクトで拡張時に主に編集するファイル・ディレクトリは以下です。

| ファイル / ディレクトリ | 役割 |
|---|---|
| `Application/app_main.c` | mruby/c VM の生成、Ruby タスクの起動、RTOSタスク管理 |
| `Application/RubyScripts/` | Ruby スクリプトを配置する場所 |
| `Application/mrubyc_ext/` | Ruby から呼び出す C 拡張メソッドやクラスを実装する場所 |

---

## 3. Ruby プログラムを追加・組み込む方法

本プロジェクトでは、Ruby プログラムを `Application/RubyScripts/` に配置すると、ビルド時に自動で mruby/c 用のバイトコードへ変換され、C プログラム側から参照できる形で組み込まれます。

利用者が `mrbc` コマンドを手動で実行する必要はありません。

ここでは、同梱済みの `led.rb` を例に、Ruby プログラムがどのように組み込まれているかを説明します。

### 3.1 Ruby スクリプトを用意する

`led.rb` は、`Application/RubyScripts/` に配置されています。

```text
Application/RubyScripts/led.rb
```

内容は以下の通りです。

```ruby
$sleep_time = 1000

led = GPIO.new("PA5", GPIO::OUT)

while true
  led.write(1)
  sleep_ms $sleep_time
  led.write(0)
  sleep_ms $sleep_time
end
```

このスクリプトでは、GPIO クラスを用いて PA5 に接続されたボード上 LED を周期的に点滅させています。

### 3.2 ビルド時の自動変換

`Application/RubyScripts/` に配置された Ruby スクリプトは、プロジェクトのビルド時に自動で mruby/c 用のバイトコードへ変換されます。

そのため、`led.rb` や `print.rb` を個別に実行したり、手動で `mrbc` コマンドを実行したりする必要はありません。

変換後、C 側では以下のようなバイトコード配列として参照できるようになります。

```c
extern const uint8_t led[];
```

### 3.3 `app_main.c` でバイトコード配列を参照する

`Application/app_main.c` では、同梱済みの `led.rb` に対応するバイトコード配列を以下のように宣言しています。

```c
extern const uint8_t led[];
```

この `led` 配列を `mrbc_load_mrb()` に渡すことで、mruby/c VM に Ruby プログラムを読み込ませています。

```c
mrbc_load_mrb(mruby_tasks[i].vm, mruby_tasks[i].script);
```

### 3.4 `mruby_tasks[]` に登録する

`led.rb` を RTOS タスクとして実行するため、`mruby_tasks[]` に以下のように登録しています。

```c
{
  .script = led,
  .vm = NULL,
  .task_id = 0,
  .alarm_id = 0,
  .timeslice = 100,

  .ctsk = {
    .itskpri = 10,
    .stksz   = 1024,
    .task    = mruby_task_entry,
    .tskatr  = TA_HLNG | TA_RNG3,
  },

  .calm = {
    .almatr = TA_HLNG,
    .almhdr = alarm_handler,
    .exinf  = NULL,
  }
}
```
基本的に多くの項目は既存タスクの設定を引き継いで構いませんが、タスクごとに変更すべき主な項目は以下の2つです。

| 項目 | 内容 | 例 |
|---|---|---|
| `.script` | 実行する Ruby プログラムのバイトコード配列 | `.script = led,` |
| `.timeslice` | ラウンドロビン方式においてそのタスクに割り当てられる実行時間[ms] | `.timeslice = 100,` |

この登録により、`led.rb` は mruby/c VM に読み込まれ、μT-Kernel 3.0 の RTOS タスクとして実行されます。

### 3.5 新しい Ruby プログラムを追加する場合

新しい Ruby プログラムを追加する場合も、`led.rb` と同じ流れで行うことができます。

## 4. Ruby から呼び出すメソッド・クラスの追加方法

本プロジェクトでは、C 言語で実装した関数やクラスを mruby/c に登録することで、Ruby スクリプト側から呼び出せるようにしています。

代表例として、以下の2種類があります。

| 種類 | 例 | 内容 |
|---|---|---|
| メソッド | `sleep_ms` | Ruby から C 関数を呼び出す |
| クラス | `GPIO` | Ruby から GPIO 操作を行うクラスを使う |

---

### 4.1 C メソッドの追加例

`sleep_ms` は、Ruby 側から指定した時間だけ RTOS タスクを待機させるメソッドです。

```c
void c_sleep_ms(struct VM *vm, mrbc_value v[], int argc)
{
  if (argc != 1) return;

  int ms = GET_INT_ARG(1);

  if (ms < 0) ms = 0;

  tk_dly_tsk(ms);
}
```

この関数は、`app_main.c` の初期化処理で Ruby メソッドとして登録しています。

```c
mrbc_define_method(0, MRBC_CLASS(Object), "sleep_ms", c_sleep_ms);
```

Ruby 側では以下のように使用できます。

```ruby
sleep_ms 100
```

---

### 4.2 C クラスの追加例

本プロジェクトでは、STM32 HAL を用いた GPIO 操作を Ruby から扱えるようにするため、`GPIO` クラスを C 言語で実装しています。

Ruby 側では以下のように使用できます。

```ruby
led = GPIO.new("PA5", GPIO::OUT)

led.write(1)
sleep_ms 100

led.write(0)
sleep_ms 100
```

`GPIO` クラスでは、主に以下のメソッドを使用できます。

| Ruby メソッド | 内容 |
|---|---|
| `GPIO.new` | GPIO オブジェクトを生成する |
| `write` | GPIO に値を出力する |
| `read` | GPIO の状態を読み取る |
| `high?` | GPIO が High か判定する |
| `low?` | GPIO が Low か判定する |
| `GPIO.read_at` | 指定したピンの状態を直接読み取る |
| `GPIO.write_at` | 指定したピンへ直接出力する |

GPIO クラスの詳細な実装は、以下のファイルを参照してください。

```text
Application/mrubyc_ext/stm32h5_gpio.c
Application/mrubyc_ext/stm32h5_gpio.h
```

---

### 4.3 ヘッダファイルへの登録

C メソッドや C クラスを追加した場合は、必要に応じて `mrubyc_ext.h` にプロトタイプ宣言やヘッダの `include` を追加します。

```c
#ifndef MRUBYC_EXT_MRUBYC_EXT_H_
#define MRUBYC_EXT_MRUBYC_EXT_H_

#include "mrubyc.h"
#include "stm32h5_gpio.h"

/* sleep */
void c_sleep_ms(struct VM *vm, mrbc_value v[], int argc);

/* print */
void c_print(struct VM *vm, mrbc_value v[], int argc);

#endif /* MRUBYC_EXT_MRUBYC_EXT_H_ */
```

---

### 4.4 メソッド・クラスの登録

C 側で実装したメソッドやクラスは、mruby/c の初期化処理で登録します。

`app_main.c` では、以下のように `sleep_ms`、`print`、`GPIO` クラスを登録しています。

```c
mrbc_init_class_gpio();

mrbc_define_method(0, MRBC_CLASS(Object), "print", c_print);
mrbc_define_method(0, MRBC_CLASS(Object), "sleep_ms", c_sleep_ms);
```

`mrbc_define_method()` を使うことで、C 関数を Ruby メソッドとして呼び出せるようになります。

また、`GPIO` クラスのようにクラス全体を追加する場合は、専用の初期化関数を用意し、その中で `mrbc_define_class()` や `mrbc_define_method()` を使ってクラスやメソッドを登録します。

GPIO クラスの場合は、`mrbc_init_class_gpio()` の中で `GPIO` クラスと各種メソッドを登録しています。

---

### 4.5 追加時の流れ

新しいメソッドやクラスを追加する場合の基本的な流れは以下です。

1. C 側に関数またはクラス処理を実装する
2. 必要に応じて `mrubyc_ext.h` に宣言を追加する
3. `mrbc_define_method()` や `mrbc_define_class()` で mruby/c に登録する
4. `app_main.c` の初期化処理で登録処理を呼び出す

---

### 4.6 補足

単純な処理を Ruby から呼び出したい場合は、`sleep_ms` のように C 関数を Ruby メソッドとして登録します。

一方で、GPIO のように状態や複数の操作をまとめて扱いたい場合は、`GPIO` クラスのように C 側でクラスとして実装します。
---

## 5. タイムスライス実行の仕組み

本プロジェクトでは、Ruby スクリプトを無制限に実行し続けるのではなく、μT-Kernel のアラーム機能を利用して mruby/c VM にプリエンプション要求を出しています。

各 Ruby タスクでは、`mrbc_vm_run()` を実行する前にアラームを開始します。

```c
tk_sta_alm(info->alarm_id, info->timeslice);

int ret = mrbc_vm_run(info->vm);

tk_stp_alm(info->alarm_id);
```

アラームが発生すると、`alarm_handler()` が呼び出され、対象 VM の `flag_preemption` を `1` に設定します。

```c
LOCAL void alarm_handler(void *exinf)
{
  mruby_task_info_t *info = (mruby_task_info_t *)exinf;

  if (info != NULL && info->vm != NULL) {
    info->vm->flag_preemption = 1;
  }
}
```

これにより、mruby/c VM は実行を一度中断し、RTOS タスクへ制御を戻します。

---
