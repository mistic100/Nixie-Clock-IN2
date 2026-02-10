import * as m from 'zigbee-herdsman-converters/lib/modernExtend';
import { presets as e, access as ea } from 'zigbee-herdsman-converters/lib/exposes';
import { logger } from 'zigbee-herdsman-converters/lib/logger';

const CUSTOM_CLUSTER_ID = 0xFF01; // manuSpecificSinope
const ATTR_ACTION_ID = 0x0000;
const ATTR_WEATHER_ID = 0x0001;

export default {
    zigbeeModel: ['IN-2-Clock'],
    model: 'IN-2-Clock',
    vendor: 'StrangePlanet',
    description: 'IN-2 Clock',
    extend: [
        m.deviceEndpoints({ endpoints: {
            "switch": 10,
            "custom": 11,
            "bme280": 12,
            "default": 11, // for proper routing of getTime cluster
        } }),

        m.onOff({ endpointNames: ["switch"], powerOnBehavior: false }),
        
        m.temperature({ endpointNames: ["bme280"] }),
        m.humidity({ endpointNames: ["bme280"] }),
    ],
    fromZigbee: [
        {
            cluster: "manuSpecificSinope",
            type: ['raw'],
            convert: (model, msg, publish, options, meta) => {
                logger.debug(`[IN-2-Clock] Raw data: ${JSON.stringify(msg.data)}`);
                if (msg.endpoint.ID === 11) {
                    const buttonId = msg.data[3];
                    const action = msg.data[4] ?? -1;
                    const actionStr = ['click', 'double_click', 'long_click'][action] ?? 'unknown';
                    return { action: `${actionStr}_${buttonId}` };
                }
            },
        },
    ],
    toZigbee: [
        {
            key: ['weather'],
            convertSet: async (entity, key, value, meta) => {
                logger.debug(`[IN-2-Clock] Send weather ${value}`);
                const v = {
                  'sunny': 1,
                  'clear-night': 2,
                  'cloudy': 3, 'partlycloudy': 3,
                  'pouring': 4, 'rainy': 4,
                  'lightning': 5, 'lightning-rainy': 5,
                  'hail': 6, 'snowy': 6, 'snowy-rainy': 6,
                  'windy': 7, 'windy-variant': 7,
                  'fog': 8,
                  'exceptional': 9,
                }[value] ?? 0;

                const endpoint = meta.device.getEndpoint(11);
                await endpoint.write(
                    CUSTOM_CLUSTER_ID,
                    { [ATTR_WEATHER_ID]: { value: v, type: 0x20 } }, // 0x20 is uint8
                    { manufacturerCode: null }
                );

                return { state: { weather: value } };
            },
        }
    ],
    exposes: [
        e.action([
          "click_1", "click_2",
          "double_click_1", "double_click_2",
          "long_click_1", "long_click_2",
        ]),
      
        e.text("weather", ea.SET).withCategory("config"),
    ],
};
