import { writable } from 'svelte/store';
import type { MotorState, RSSI } from '../types/models';
import type { Battery } from '../types/models';
import type { DownloadOTA } from '../types/models';

let telemetry_data = {
	rssi: {
		rssi: 0,
		ssid: '',
		disconnected: true
	},
	battery: {
		soc: 100,
		charging: false
	},
	download_ota: {
		status: 'none',
		progress: 0,
		error: ''
	},
	homed: false,
	error: true
};

function createTelemetry() {
	const { subscribe, set, update } = writable(telemetry_data);

	return {
		subscribe,
		setRSSI: (data: RSSI) => {
			if (!isNaN(Number(data.rssi))) {
				update((telemetry_data) => ({
					...telemetry_data,
					rssi: { rssi: Number(data.rssi), ssid: data.ssid, disconnected: false }
				}));
			} else {
				update((telemetry_data) => ({
					...telemetry_data,
					rssi: { rssi: 0, ssid: data.ssid, disconnected: true }
				}));
			}
		},
		setBattery: (data: Battery) => {
			update((telemetry_data) => ({
				...telemetry_data,
				battery: { soc: data.soc, charging: data.charging }
			}));
		},
		setDownloadOTA: (data: DownloadOTA) => {
			update((telemetry_data) => ({
				...telemetry_data,
				download_ota: { status: data.status, progress: data.progress, error: data.error }
			}));
		},
		setMotorStatus: (data: MotorState) => {
			update((telemerty_data) => ({ ...telemerty_data, homed: data.homed, error: data.error }));
		},
		setMotorError: (data: string) => {
			const content = JSON.parse(data);
			update((telemerty_data) => ({ ...telemerty_data, error: content.error }));
		}
	};
}

export const telemetry = createTelemetry();
