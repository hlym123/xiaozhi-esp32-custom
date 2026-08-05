#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "application.h"
#include "button.h"
#include "config.h"

#include "M5PM1.h"

#include <driver/gpio.h>
#include <driver/i2c_master.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "PaperColor7Inch"

class M5StackPaperColor7InchBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t i2c_bus_ = nullptr;
    M5PM1* pmic_ = nullptr;
    Button key1_;
    Button key2_;

    void InitializeI2c() {
        i2c_master_bus_config_t bus_config = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus_));
    }

    void InitializePower() {
        pmic_ = new M5PM1();
        auto ret = pmic_->begin(i2c_bus_, PMIC_I2C_ADDR, M5PM1_I2C_FREQ_100K);
        ESP_ERROR_CHECK(ret == M5PM1_OK ? ESP_OK : ESP_FAIL);

        ret = pmic_->gpioSet(PMIC_CODEC_POWER_GPIO, M5PM1_GPIO_MODE_OUTPUT,
                             HIGH, M5PM1_GPIO_PULL_NONE, M5PM1_GPIO_DRIVE_PUSHPULL);
        ESP_ERROR_CHECK(ret == M5PM1_OK ? ESP_OK : ESP_FAIL);
        vTaskDelay(pdMS_TO_TICKS(50));
        ESP_LOGI(TAG, "Codec power enabled by M5PM1 GPIO%d", PMIC_CODEC_POWER_GPIO);
    }

    void InitializePa() {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << AUDIO_CODEC_GPIO_PA),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&io_conf));

        gpio_set_level(AUDIO_CODEC_GPIO_PA, 0);
        vTaskDelay(pdMS_TO_TICKS(2));

        for (int i = 0; i < 2; ++i) {
            gpio_set_level(AUDIO_CODEC_GPIO_PA, 1);
            esp_rom_delay_us(5);
            if (i != 1) {
                gpio_set_level(AUDIO_CODEC_GPIO_PA, 0);
                esp_rom_delay_us(5);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(2));
        ESP_LOGI(TAG, "AW8737A mode 2 (1W), GPIO%d held high", AUDIO_CODEC_GPIO_PA);
    }

    void InitializeButtons() {
        key1_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });

        key2_.OnPressDown([]() {
            Application::GetInstance().StartListening();
        });
        key2_.OnPressUp([]() {
            Application::GetInstance().StopListening();
        });
    }

public:
    M5StackPaperColor7InchBoard()
        : key1_(KEY1_GPIO), key2_(KEY2_GPIO) {
        InitializeI2c();
        InitializePower();
        InitializePa();
        InitializeButtons();
    }

    AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec audio_codec(
            i2c_bus_,
            I2C_NUM_0,
            AUDIO_INPUT_SAMPLE_RATE,
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK,
            AUDIO_I2S_GPIO_BCLK,
            AUDIO_I2S_GPIO_WS,
            AUDIO_I2S_GPIO_DOUT,
            AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_GPIO_PA,
            AUDIO_CODEC_ES8311_ADDR,
            true);
        return &audio_codec;
    }
};

DECLARE_BOARD(M5StackPaperColor7InchBoard);
