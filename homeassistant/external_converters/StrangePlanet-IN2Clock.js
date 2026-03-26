import { Zcl } from 'zigbee-herdsman';
import * as m from 'zigbee-herdsman-converters/lib/modernExtend';
import { presets as e } from 'zigbee-herdsman-converters/lib/exposes';
import { logger } from 'zigbee-herdsman-converters/lib/logger';
import { OneJanuary2000 } from 'zigbee-herdsman-converters/lib/constants';

const CUSTOM_CLUSTER_NAME = 'customStrangeplanet';

const CUSTOM_CLUSTER_ID = 0xFF01; // manuSpecificSinope
const ENDPOINT_CUSTOM_ID = 10;
const ATTR_ACTION_ID = 0x0000;
const ATTR_WEATHER_ID = 0x0001;
const ATTR_TEMP_OFFSET = 0x0002;

const ENDPOINT_MAIN = 'main';
const ENDPOINT_BME280 = 'bme280';
const ENDPOINT_LEDS = 'leds';
const ENDPOINT_CUSTOM = 'custom';

export default {
    zigbeeModel: ['IN-2-Clock'],
    model: 'IN-2-Clock',
    vendor: 'StrangePlanet',
    description: 'IN-2 Clock',

    extend: [
        m.deviceEndpoints({ endpoints: {
            [ENDPOINT_MAIN]: 1,
            [ENDPOINT_BME280]: 2,
            [ENDPOINT_LEDS]: 3,
            [ENDPOINT_CUSTOM]: ENDPOINT_CUSTOM_ID,
        } }),

        m.onOff({
            endpointNames: [ENDPOINT_MAIN],
            powerOnBehavior: false,
        }),
        
        m.temperature({
            endpointNames: [ENDPOINT_BME280],
        }),
        m.humidity({
            endpointNames: [ENDPOINT_BME280],
        }),

        m.light({
            endpointNames: [ENDPOINT_LEDS],
            effect: false,
            powerOnBehavior: false,
            color: { modes: ['hs'], enhancedHue: false },
        }),

        m.deviceAddCustomCluster(CUSTOM_CLUSTER_NAME, {
            ID: CUSTOM_CLUSTER_ID,
            name: CUSTOM_CLUSTER_NAME,
            manufacturerCode: null,
            attributes: {
                weatherCode: {
                    name: 'weatherCode',
                    ID: ATTR_WEATHER_ID,
                    type: Zcl.DataType.UINT8,
                    write: true,
                },
                temperatureCalibration: {
                    name: 'temperatureCalibration',
                    ID: ATTR_TEMP_OFFSET,
                    type: Zcl.DataType.INT16,
                    write: true,
                },
            },
        }),

        m.enumLookup({
            name: 'weather_code',
            endpointName: ENDPOINT_CUSTOM,
            cluster: CUSTOM_CLUSTER_NAME,
            attribute: 'weatherCode',
            entityCategory: 'config',
            description: 'Weather code',
            access: 'SET',
            lookup: {
                'sunny': 1,
                'clear-night': 2,
                'cloudy': 3, 'partlycloudy': 3,
                'pouring': 4, 'rainy': 4,
                'lightning': 5, 'lightning-rainy': 5,
                'hail': 6, 'snowy': 6, 'snowy-rainy': 6,
                'windy': 7, 'windy-variant': 7,
                'fog': 8,
                'exceptional': 9,
            }
        }),

        m.numeric({
            name: 'temperature_calibration',
            endpointNames: [ENDPOINT_CUSTOM],
            cluster: CUSTOM_CLUSTER_NAME,
            attribute: 'temperatureCalibration',
            entityCategory: 'config',
            description: 'Offset to add/subtract to the reported temperature',
            valueMin: -10,
            valueMax: 10,
            scale: 100,
            valueStep: 0.1,
            unit: '°C',
        }),
    ],

    fromZigbee: [
        {
            cluster: CUSTOM_CLUSTER_NAME,
            type: ['raw'],
            convert: (model, msg, publish, options, meta) => {
                logger.debug(`[IN-2-Clock] Raw data: ${JSON.stringify(msg.data)}`);
                if (msg.endpoint.ID === ENDPOINT_CUSTOM_ID) {
                    const buttonId = msg.data[3] ?? 0;
                    const action = msg.data[4] ?? -1;
                    const actionStr = ['click', 'double_click', 'long_click'][action];
                    if (actionStr) {
                        return { action: `${actionStr}_${buttonId}` };
                    }
                }
            },
        },
    ],
    exposes: [
        e.action([
          'click_1', 'click_2',
          'long_click_1', 'long_click_2',
          'double_click_1', 'double_click_2',
        ]),
    ],
};
