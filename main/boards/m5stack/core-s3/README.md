# 使用说明

**编译**

```bash
python3 scripts/build.py m5stack/core-s3 --name m5stack-core-s3 --zip
```

如需手动编译，请参考 `config.json` 修改 menuconfig 对应选项。

**设备端 AEC**

默认启用 `CONFIG_USE_DEVICE_AEC` 和 `AUDIO_INPUT_REFERENCE`，沿用 ESP32-S3-BOX-3 的 ES7210 采集方式：以 4 个物理 TDM 槽、16-bit PCM 打开输入，用通道掩码选取槽位 0（MIC1）和槽位 1（MIC3，扬声器回采），向 AFE 提供 `MR` 两路交错数据。

录音与播放共用 BCLK/WS。4 槽录音使用 64fs，播放端会扩展为两个 32-bit 槽，因此双工开启后需同步设置 AW88298 的槽宽，PCM 数据仍为 16-bit。

`--zip` 会生成合并镜像 `build/merged-binary.bin` 和发布压缩包；合并镜像从 `0x0` 烧录。刷机后应验证静音环境下唤醒与识别、扬声器播放，以及播放期间说话打断。

**烧录**

```bash
idf.py flash
```

> 进入下载模式：长按复位按键（约 3 秒），直至内部指示灯亮绿色，松开按键。
