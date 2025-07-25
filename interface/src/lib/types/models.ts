export type WifiStatus = {
	status: number;
	local_ip: string;
	mac_address: string;
	rssi: number;
	ssid: string;
	bssid: string;
	channel: number;
	subnet_mask: string;
	gateway_ip: string;
	dns_ip_1: string;
	dns_ip_2?: string;
};

export type WifiSettings = {
	hostname: string;
	connection_mode: number;
	wifi_networks: KnownNetworkItem[];
};

export type KnownNetworkItem = {
	ssid: string;
	password: string;
	static_ip_config: boolean;
	local_ip?: string;
	subnet_mask?: string;
	gateway_ip?: string;
	dns_ip_1?: string;
	dns_ip_2?: string;
};

export type NetworkItem = {
	rssi: number;
	ssid: string;
	bssid: string;
	channel: number;
	encryption_type: number;
};

export type ApStatus = {
	status: number;
	ip_address: string;
	mac_address: string;
	station_num: number;
};

export type ApSettings = {
	provision_mode: number;
	ssid: string;
	password: string;
	channel: number;
	ssid_hidden: boolean;
	max_clients: number;
	local_ip: string;
	gateway_ip: string;
	subnet_mask: string;
};

export type BrokerSettings = {
	status_topic: string;
	control_topic_pub: string;
	control_topic_sub: string;
	environment_topic_pub: string;
	safestate_topic_pub: string;
	safestate_topic_sub: string;
};

export type NTPStatus = {
	status: number;
	utc_time: string;
	local_time: string;
	server: string;
	uptime: number;
};

export type NTPSettings = {
	enabled: boolean;
	server: string;
	tz_label: string;
	tz_format: string;
};

export type Analytics = {
	max_alloc_heap: number;
	psram_size: number;
	free_psram: number;
	used_psram: number;
	free_heap: number;
	used_heap: number;
	total_heap: number;
	min_free_heap: number;
	core_temp: number;
	fs_total: number;
	fs_used: number;
	uptime: number;
};

export type RSSI = {
	rssi: number;
	ssid: string;
};

export type Battery = {
	soc: number;
	charging: boolean;
};

export type DownloadOTA = {
	status: string;
	progress: number;
	error: string;
};

export type StaticSystemInformation = {
	esp_platform: string;
	firmware_version: string;
	cpu_freq_mhz: number;
	cpu_type: string;
	cpu_rev: number;
	cpu_cores: number;
	sketch_size: number;
	free_sketch_space: number;
	sdk_version: string;
	arduino_version: string;
	flash_chip_size: number;
	flash_chip_speed: number;
	cpu_reset_reason: string;
};

export type SystemInformation = Analytics & StaticSystemInformation;

export type MQTTStatus = {
	enabled: boolean;
	connected: boolean;
	client_id: string;
	last_error: string;
};

export type MQTTSettings = {
	enabled: boolean;
	uri: string;
	username: string;
	password: string;
	client_id: string;
	keep_alive: number;
	clean_session: boolean;
	message_interval_ms: number;
};

export type ControlState = {
	command: string;
	depth: number;
	stroke: number;
	rate: number;
	sensation: number;
	pattern: string;
	go?: boolean;
	oldStroke?: number;
};

export type MotorState = {
	homed: boolean;
	error: boolean;
};

export type MotorConfig = {
	driver: string;
	driver_list: string[];
	steps_per_rev: number;
	max_rpm: number;
	max_acceleration: number;
	pulley_teeth: number;
	invert_direction: boolean;
	measure_travel: boolean;
	home: boolean;
	travel: number;
	keepout: number;
	sensorless_trigger: number;
};

export type SafeState = {
	safestate: boolean;
};

export type HeartbeatMode = {
	heartbeat: number;
};

export type Environment = {
	depth: number;
	max_rate: number;
	max_velocity: number;
	heartbeat_mode: number;
	patterns: string[];
	max_current: number;
	max_voltage: number;
	motor: string;
};
