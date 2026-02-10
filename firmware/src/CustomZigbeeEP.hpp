#pragma once

#include <Zigbee.h>

#define CUSTOM_CLUSTER_ID 0xFF01
#define ATTR_ACTION_ID 0x0000
#define ATTR_WEATHER_ID 0x0001

#define TAG_ZB_TBN "ZB_BTN"

/**
 * @brief Custom endpoint
 * - weather code cluster
 * - time cluster
 * - button press command
 */
class CustomZigbeeEP : public ZigbeeEP
{
private:
    time_t _read_time;
    int32_t _read_offset;

    void (*_on_time)(time_t);
    void (*_on_weather)(u8_t);

public:
    CustomZigbeeEP(u8_t endpoint) : ZigbeeEP(endpoint)
    {
        _device_id = ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID;

        _cluster_list = esp_zb_zcl_cluster_list_create();

        esp_zb_on_off_switch_cfg_t switch_cfg = ESP_ZB_DEFAULT_ON_OFF_SWITCH_CONFIG();
        esp_zb_cluster_list_add_basic_cluster(_cluster_list, esp_zb_basic_cluster_create(&(switch_cfg.basic_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
        esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_identify_cluster_create(&(switch_cfg.identify_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

        esp_zb_attribute_list_t *custom_cluster = esp_zb_zcl_attr_list_create(CUSTOM_CLUSTER_ID);
        u8_t no_weather = 0;
        esp_zb_custom_cluster_add_custom_attr(custom_cluster, ATTR_WEATHER_ID, ESP_ZB_ZCL_ATTR_TYPE_U8, ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE, &no_weather);
        esp_zb_cluster_list_add_custom_cluster(_cluster_list, custom_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

        _ep_config = {
            .endpoint = _endpoint,
            .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
            .app_device_id = ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID,
            .app_device_version = 0};

        addTimeCluster();
    }

    void onTime(void (*callback)(time_t)) { _on_time = callback; }

    void onWeather(void (*callback)(u8_t)) { _on_weather = callback; }

    void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *msg) override
    {
        if (msg->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_TIME)
        {
            if (msg->attribute.id == ESP_ZB_ZCL_ATTR_TIME_TIME_ID)
            {
                _read_time = *(uint32_t *)msg->attribute.data.value;
            }
            else if (msg->attribute.id == ESP_ZB_ZCL_ATTR_TIME_TIME_ZONE_ID)
            {
                _read_offset = *(uint32_t *)msg->attribute.data.value;
                _on_time(_read_time + _read_offset);
            }
            else
            {
                log_w("Received message ignored. Attribute ID: %d not supported for Time cluster", msg->attribute.id);
            }
        }
        else if (msg->info.cluster == CUSTOM_CLUSTER_ID)
        {
            if (msg->attribute.id == ATTR_WEATHER_ID)
            {
                u8_t weather = *(u8_t *)msg->attribute.data.value;
                _on_weather(weather);
            }
            else
            {
                log_w("Received message ignored. Attribute ID: %d not supported for custom cluster", msg->attribute.id);
            }
        }
        else
        {
            log_w("Received message ignored. Cluster ID: %d not supported", msg->info.cluster);
        }
    }

    void sendEvent(u8_t button_id, u8_t action)
    {
        ESP_LOGI(TAG_ZB_TBN, "Send event %d %d", button_id, action);
        u8_t payload[2] = {button_id, (u8_t)0};

        esp_zb_zcl_custom_cluster_cmd_req_t req = {0};

        req.zcl_basic_cmd.dst_addr_u.addr_short = 0x0000; // coordinator
        req.zcl_basic_cmd.dst_endpoint = _endpoint;
        req.zcl_basic_cmd.src_endpoint = _endpoint;

        req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
        req.cluster_id = CUSTOM_CLUSTER_ID;
        req.profile_id = ESP_ZB_AF_HA_PROFILE_ID;
        req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
        req.custom_cmd_id = ATTR_ACTION_ID;

        req.data.type = ESP_ZB_ZCL_ATTR_TYPE_SET;
        req.data.size = sizeof(payload);
        req.data.value = payload;

        esp_zb_lock_acquire(portMAX_DELAY);
        esp_zb_zcl_custom_cluster_cmd_req(&req);
        esp_zb_lock_release();
    }
};
