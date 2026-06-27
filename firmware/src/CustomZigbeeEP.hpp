#pragma once

#include <Zigbee.h>

#define TAG_ZB_CUSTOM "ZB_CUSTOM"

/**
 * @brief Custom endpoint
 * - weather code
 * - temp offset
 * - send action command
 */
class CustomZigbeeEP : public ZigbeeEP
{
private:
    void (*_on_weather)(u8_t);
    void (*_on_temp_offset)(int16_t);

    volatile bool _weather_code_changed = false;
    volatile u8_t _weather_code = 0;

    volatile bool _temp_offset_changed = false;
    volatile int16_t _temp_offset = 0;

public:
    CustomZigbeeEP(u8_t endpoint) : ZigbeeEP(endpoint)
    {
        _device_id = ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID;
        _cluster_list = esp_zb_zcl_cluster_list_create();

        esp_zb_on_off_switch_cfg_t switch_cfg = ESP_ZB_DEFAULT_ON_OFF_SWITCH_CONFIG();
        esp_zb_cluster_list_add_basic_cluster(_cluster_list, esp_zb_basic_cluster_create(&(switch_cfg.basic_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
        esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_identify_cluster_create(&(switch_cfg.identify_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

        esp_zb_attribute_list_t *custom_cluster = esp_zb_zcl_attr_list_create(ZIGBEE_CUSTOM_CLUSTER_ID);
        u8_t no_weather = 0;
        esp_zb_custom_cluster_add_custom_attr(custom_cluster, ZIGBEE_ATTR_WEATHER_ID, ESP_ZB_ZCL_ATTR_TYPE_U8, ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE, &no_weather);
        int16_t offset_zero = 0;
        esp_zb_custom_cluster_add_custom_attr(custom_cluster, ZIGBEE_ATTR_TEMP_OFFSET, ESP_ZB_ZCL_ATTR_TYPE_S16, ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE, &offset_zero);
        esp_zb_cluster_list_add_custom_cluster(_cluster_list, custom_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

        _ep_config = {
            .endpoint = _endpoint,
            .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
            .app_device_id = ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID,
            .app_device_version = 0};
    }

    void onWeather(void (*callback)(u8_t)) { _on_weather = callback; }

    void onTempOffset(void (*callback)(int16_t)) { _on_temp_offset = callback; }

    void loop()
    {
        if (_weather_code_changed)
        {
            _weather_code_changed = false;
            _on_weather(_weather_code);
        }

        if (_temp_offset_changed)
        {
            _temp_offset_changed = false;
            _on_temp_offset(_temp_offset);
        }
    }

    bool setTempOffset(uint16_t offset)
    {
        esp_zb_zcl_status_t ret = ESP_ZB_ZCL_STATUS_SUCCESS;
        ESP_LOGD(TAG_ZB_CUSTOM, "Settings temp offset %d", offset);
        esp_zb_lock_acquire(portMAX_DELAY);
        ret = esp_zb_zcl_set_attribute_val(
            _endpoint, ZIGBEE_CUSTOM_CLUSTER_ID, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ZIGBEE_ATTR_TEMP_OFFSET, &offset, false
        );
        esp_zb_lock_release();
        if (ret != ESP_ZB_ZCL_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG_ZB_CUSTOM, "Failed to set tmep offset: 0x%x: %s", ret, esp_zb_zcl_status_to_name(ret));
            return false;
        }
        return true;
    }

    void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *msg) override
    {
        if (msg->info.cluster == ZIGBEE_CUSTOM_CLUSTER_ID)
        {
            if (msg->attribute.id == ZIGBEE_ATTR_WEATHER_ID)
            {
                u8_t weather = *(u8_t *)msg->attribute.data.value;
                ESP_LOGI(TAG_ZB_CUSTOM, "Received weather code %d", weather);
                _weather_code = weather;
                _weather_code_changed = true;
            }
            else if (msg->attribute.id == ZIGBEE_ATTR_TEMP_OFFSET)
            {
                int16_t offset = *(int16_t *)msg->attribute.data.value;
                ESP_LOGI(TAG_ZB_CUSTOM, "Received temp offset %d", offset);
                _temp_offset = offset;
                _temp_offset_changed = true;
            }
            else
            {
                ESP_LOGW(TAG_ZB_CUSTOM, "Received message ignored. Attribute ID: %d not supported for Custom cluster", msg->attribute.id);
            }
        }
        else
        {
            ESP_LOGW(TAG_ZB_CUSTOM, "Received message ignored. Cluster ID: %d not supported", msg->info.cluster);
        }
    }

    void sendEvent(u8_t button_id, u8_t action)
    {
        ESP_LOGI(TAG_ZB_CUSTOM, "Send event %d %d", button_id, action);
        u8_t payload[2] = {button_id, action};

        esp_zb_zcl_custom_cluster_cmd_req_t req = {0};

        req.zcl_basic_cmd.dst_addr_u.addr_short = 0x0000; // coordinator
        req.zcl_basic_cmd.dst_endpoint = _endpoint;
        req.zcl_basic_cmd.src_endpoint = _endpoint;

        req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
        req.cluster_id = ZIGBEE_CUSTOM_CLUSTER_ID;
        req.profile_id = ESP_ZB_AF_HA_PROFILE_ID;
        req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
        req.custom_cmd_id = ZIGBEE_ATTR_ACTION_ID;

        req.data.type = ESP_ZB_ZCL_ATTR_TYPE_SET;
        req.data.size = sizeof(payload);
        req.data.value = payload;

        esp_zb_lock_acquire(portMAX_DELAY);
        esp_zb_zcl_custom_cluster_cmd_req(&req);
        esp_zb_lock_release();
    }
};
