<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { environment } from '$lib/stores/environment';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Safety from '~icons/tabler/lock-square-rounded';
	import Info from '~icons/tabler/info-circle';
	import Save from '~icons/tabler/device-floppy';
	import Reload from '~icons/tabler/reload';

	type SafetyState = {
		depth_limit: number;
		stroke_limit: number;
		rate_limit: number;
		velocity_limit: number;
		heartbeat_mode: number;
		ease_in_speed: number;
	};

	let safetySettings: SafetyState = $state({
		depth_limit: 0,
		stroke_limit: 0,
		rate_limit: 0,
		velocity_limit: 0,
		heartbeat_mode: 0,
		ease_in_speed: 0
	});

	// $: console.log(safetySettings);

	async function getSafetySettings() {
		try {
			const response = await fetch('/rest/safety', {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			safetySettings = await response.json();
			// console.log(safetySettings);
			environment.update((n) => {
				return {
					...n,
					heartbeat_mode: safetySettings.heartbeat_mode
				};
			});
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	async function postSafetySettings() {
		try {
			const response = await fetch('/rest/safety', {
				method: 'POST',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(safetySettings)
			});
			if (response.status == 200) {
				notifications.success('Safety settings updated.', 3000);
				safetySettings = await response.json();
				environment.update((n) => {
					return {
						...n,
						heartbeat_mode: safetySettings.heartbeat_mode
					};
				});
				//console.log(safetySettings);
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

	onMount(() => {
		getSafetySettings();
	});

	function onChange(event: any) {
		safetySettings.heartbeat_mode = parseInt(event.currentTarget.value);
	}
</script>

<SettingsCard collapsible={false}>
	{#snippet icon()}
		<Safety class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	{/snippet}
	{#snippet title()}
		<span>Safety Settings</span>
	{/snippet}
	<div class="w-full">
		<h1 class="text-xl font-semibold">Range Limit</h1>
		<div class="alert my-2 shadow-lg bg-base-100">
			<Info class="h-6 w-6 shrink-0 stroke-current" />
			<span
				>This limits the range of the input parameters <b>Depth</b>, <b>Stroke</b> and <b>Speed</b>.
				These safety limits can't be overruled and the machine will constrain all inputs to these
				limits. This affects all input methods, including the remote API, M5 Remote, T-Code and the
				Web UI.
			</span>
		</div>

		<div class="mt-4 mx-4 flex-col justify-around">
			<input
				type="range"
				min="0"
				max={$environment.depth}
				bind:value={safetySettings.depth_limit}
				class="range range-primary range-xs w-full"
			/>
			<div class="inline-flex mt-1 justify-between w-full">
				<span class="text-base-content"><b>Depth</b></span>
				<span class="text-base-content">{safetySettings.depth_limit} mm</span>
			</div>
		</div>
		<div class="mt-4 mx-4 flex-col justify-around">
			<input
				type="range"
				min="0"
				max={$environment.depth}
				bind:value={safetySettings.stroke_limit}
				class="range range-primary range-xs w-full"
			/>
			<div class="inline-flex mt-1 justify-between w-full">
				<span class="text-base-content"><b>Stroke</b></span>
				<span class="text-base-content">{safetySettings.stroke_limit} mm</span>
			</div>
		</div>
		<div class="mt-4 mx-4 flex-col justify-around">
			<input
				type="range"
				min="0"
				max={$environment.max_rate}
				bind:value={safetySettings.rate_limit}
				class="range range-primary range-xs w-full"
			/>
			<div class="inline-flex mt-1 justify-between w-full">
				<span class="text-base-content"><b>Speed</b></span>
				<span class="text-base-content">{safetySettings.rate_limit} FPM</span>
			</div>
		</div>
		<div class="divider"></div>
		<h1 class="text-xl font-semibold">Velocity Limit</h1>
		<div class="alert my-2 shadow-lg bg-base-100">
			<Info class="h-6 w-6 shrink-0 stroke-current" />
			<span
				>Limits the maximum velocity of the machine. This is an additional safety feature to prevent
				injuries due to sudden fast and unexpected movements. Especially when remote controlled this
				is an important safety feature to ensure that the remote drivers stays within your
				abilities.
			</span>
		</div>
		<div class="mt-4 mx-4 flex-col justify-around">
			<input
				type="range"
				min="0"
				max={$environment.max_velocity}
				bind:value={safetySettings.velocity_limit}
				class="range range-primary range-xs w-full"
			/>
			<div class="inline-flex mt-1 justify-between w-full">
				<span class="text-base-content"><b>Velocity Limit</b></span>
				<span class="text-base-content">{safetySettings.velocity_limit} mm/s</span>
			</div>
		</div>
		<div class="alert my-2 shadow-lg bg-base-100">
			<Info class="h-6 w-6 shrink-0 stroke-current" />
			<span
				>Changing parameters may result in sudden very fast movements that could come at a surprise.
				To mitigate this effect the <b>Ease In Speed</b> limits how fast a change in <b>Depth</b> or
				<b>Stroke</b> is applied to the individual strokes.
			</span>
		</div>
		<div class="mt-4 mx-4 flex-col justify-around">
			<input
				type="range"
				min="0"
				max="30.0"
				bind:value={safetySettings.ease_in_speed}
				class="range range-primary range-xs w-full"
			/>
			<div class="inline-flex mt-1 justify-between w-full">
				<span class="text-base-content"><b>Ease In Speed</b></span>
				<span class="text-base-content">{safetySettings.ease_in_speed} mm/s</span>
			</div>
		</div>
		<div class="divider"></div>
		<h1 class="text-xl font-semibold">Heartbeat Mode</h1>
		<div class="alert my-2 shadow-lg bg-base-100">
			<Info class="h-6 w-6 shrink-0 stroke-current" />
			<span
				>Heartbeat mode is a safety feature that will stop the machine if the heartbeat signal is
				lost. This is useful if the machine is controlled via a remote API and the connection is
				lost. The heartbeat signal is simply sending the StrokeEngine control message at least once
				every second. If the heartbeat signal is lost the machine will stop and the LED will start
				blinking red. The machine can be restarted by deliberately starting it again. If multiple
				remotes are connected to the machine, <b>Any</b> will stop the machine if any one remote is
				lost. <b>Last</b> will stop the machine once the last remote sending control messages is lost.
			</span>
		</div>
		<div class="flex sm:flex-row flex-col justify-around mt-4 mx-6">
			<label class="inline-flex font-bold m-2 space-x-4">
				<input
					checked={safetySettings.heartbeat_mode === 0}
					onchange={onChange}
					type="radio"
					name="mode"
					value="0"
					class="radio radio-primary"
				/> <span>Disabled</span>
			</label>
			<label class="inline-flex font-bold m-2 space-x-4">
				<input
					checked={safetySettings.heartbeat_mode === 1}
					onchange={onChange}
					type="radio"
					name="mode"
					value="1"
					class="radio radio-primary"
				/> <span>Any</span>
			</label>
			<label class="inline-flex font-bold m-2 space-x-4">
				<input
					checked={safetySettings.heartbeat_mode === 2}
					onchange={onChange}
					type="radio"
					name="mode"
					value="2"
					class="radio radio-primary"
				/> <span>Last</span>
			</label>
		</div>
		<div class="divider mb-2 mt-0"></div>
		<div class="mx-4 flex flex-wrap justify-end gap-2">
			<button class="btn btn-primary" onclick={postSafetySettings}>Apply Settings</button>
		</div>
	</div>
</SettingsCard>
