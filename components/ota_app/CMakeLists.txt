set(srcs "src/ota_app.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES app_update esp_http_client esp_https_ota protocol_examples_common nvs_flash)
