# 水耕栽培の水タンクでメダカを飼う計画

水タンクの水質取得、ファンやポンプの制御を行うもの。

[センサーデータ](https://ambidata.io/bd/board.html?id=61415)

## build
`env.local.ini` に WiFi の接続情報と [Ambient](https://ambidata.io/) のチャンネルID, ライトキーを入力してからビルドする必要がある。
```ini
[env:esp32dev]
build_flags =
    ${env.build_flags}
    '-DWIFI_SSID="WIFI SSID"'
    -D WIFI_PASSWORD='"WIFI PASSWORD"'
    -D AMBIENT_CLIENT_ID=123456
    -D AMBIENT_WRITE_KEY='"AMBIENT WRITE KEY"'
```

```shell
$ pio -c clion init
$ pio -c clion run --target upload -e esp32dev
$ pio -c clion run --target uploadfs -e esp32dev
```

## feature

- [x] センサ
    * [x] 水温(サーミスタ)
    * [x] 気温(BME280)
    * [x] TDS([keyestudio TDS Meter](https://wiki.keyestudio.com/KS0429_keyestudio_TDS_Meter_V1.0))

- [x] 制御系
    * [x] ファン制御(pwm)
    * [x] 水中ポンプ制御(pwm)

- [x] HTTP API
    * [x] Ambientへ送信
    * [x] 状態確認
    * [x] 制御系

- [x] 生体
    * [x] 大葉
    * [x] メダカ
    * [x] ミナミヌマエビ

### HTTP API

#### GET /

Web UI

#### GET /status

現在の状態を取得する。

```shell
$ curl -s http://esp32-ip-address/status | json_pp -f json
{
  "timestamp":1690372285,
  "water_temperature":27.88784686,
  "temperature":32.50999832,
  "humidity":52.78125,
  "atmospheric_pressure":1016.731875,
  "tds":596.6633259,
  "devices":[
    {
      "name":"fan",
      "on":false,
      "duty":70
    },
    ...
  ]
}
```

| name                 | description  |
|----------------------|--------------|
| timestamp            | 時刻           |
| water_temperature    | 水温(℃)        |
| temperature          | 気温(℃)        |
| humidity             | 湿度(%)        |
| atmospheric_pressure | 大気圧(hPa)     |
| tds                  | 総溶解固形分(ppm)  |
| devices              | 制御装置の状態      |
| devices.name         | 制御装置の名前      |
| devices.on           | 動作中の場合に true |
| devices.duty         | duty比        |

### GET /(fan|pump)/

ファン、水中ポンプの現在の状態を取得する。

```shell
$ curl -s http://esp32-ip-address/pump/ | json_pp
{
   "duty" : 255,
   "name" : "pump",
   "on" : true
}
```

### GET /(fan|pump)/(on|off|\d+)

ファン、水中ポンプの on/off、 duty比の設定を行う。

```shell
curl -s http://esp32-ip-address/pump/100 | json_pp
{
   "duty" : 100,
   "name" : "pump",
   "on" : true
}
```