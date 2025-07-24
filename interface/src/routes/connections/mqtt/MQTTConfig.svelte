<script lang="ts">
	import { slide } from 'svelte/transition';
	import { cubicOut } from 'svelte/easing';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import Spinner from '$lib/components/Spinner.svelte';
	import MQTT from '~icons/tabler/topology-star-3';
	import Info from '~icons/tabler/info-circle';
	import type { BrokerSettings } from '$lib/types/models';

	let brokerSettings: BrokerSettings = $state({
		status_topic: '',
		control_topic_pub: '',
		control_topic_sub: '',
		environment_topic_pub: '',
		safestate_topic_pub: '',
		safestate_topic_sub: '',
		identification_topic_pub: ''
	});

	let formField: any = $state();

	async function getBrokerSettings() {
		try {
			const response = await fetch('/rest/brokerSettings', {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			brokerSettings = await response.json();
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	let formErrors = {
		status: false,
		control_pub: false,
		control_sub: false,
		environment: false,
		safestate_pub: false,
		safestate_sub: false,
		identification: false
	};

	async function postBrokerSettings() {
		try {
			const response = await fetch('/rest/brokerSettings', {
				method: 'POST',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(brokerSettings)
			});
			if (response.status == 200) {
				notifications.success('Broker settings updated.', 3000);
				brokerSettings = await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	function handleSubmitBroker() {
		let valid = true;
		// Validate control topic
		if (
			brokerSettings.control_topic_pub.length > 64 ||
			brokerSettings.control_topic_pub.includes('#') ||
			brokerSettings.control_topic_pub.includes('+')
		) {
			valid = false;
			formErrors.control_pub = true;
		} else {
			formErrors.control_pub = false;
		}

		// Validate control topic
		if (brokerSettings.control_topic_sub.length > 64) {
			valid = false;
			formErrors.control_sub = true;
		} else {
			formErrors.control_sub = false;
		}

		// Validate environment topic
		if (
			brokerSettings.environment_topic_pub.length > 64 ||
			brokerSettings.environment_topic_pub.includes('#') ||
			brokerSettings.environment_topic_pub.includes('+')
		) {
			valid = false;
			formErrors.environment = true;
		} else {
			formErrors.environment = false;
		}

		// Validate safestate topic
		if (
			brokerSettings.safestate_topic_pub.length > 64 ||
			brokerSettings.safestate_topic_pub.includes('#') ||
			brokerSettings.safestate_topic_pub.includes('+')
		) {
			valid = false;
			formErrors.safestate_pub = true;
		} else {
			formErrors.safestate_pub = false;
		}

		// Validate safestate topic
		if (brokerSettings.safestate_topic_sub.length > 64) {
			valid = false;
			formErrors.safestate_sub = true;
		} else {
			formErrors.safestate_sub = false;
		}

		// Validate MQTT Status Topic
		if (
			brokerSettings.status_topic.length > 64 ||
			brokerSettings.status_topic.includes('#') ||
			brokerSettings.status_topic.includes('+')
		) {
			valid = false;
			formErrors.status = true;
		} else {
			formErrors.status = false;
		}

		// Validate Identification Topic
		if (
			brokerSettings.identification_topic_pub.length > 64 ||
			brokerSettings.identification_topic_pub.includes('#') ||
			brokerSettings.identification_topic_pub.includes('+')
		) {
			valid = false;
			formErrors.identification = true;
		} else {
			formErrors.identification = false;
		}

		// Submit JSON to REST API
		if (valid) {
			postBrokerSettings();
		}
	}

	function preventDefault(fn) {
		return function (event) {
			event.preventDefault();
			fn.call(this, event);
		};
	}
</script>

<SettingsCard collapsible={true} open={false}>
	{#snippet icon()}
		<MQTT class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	{/snippet}
	{#snippet title()}
		<span>MQTT Broker Settings</span>
	{/snippet}
	<div class="w-full">
		{#await getBrokerSettings()}
			<Spinner />
		{:then nothing}
			<form
				class="fieldset"
				onsubmit={preventDefault(handleSubmitBroker)}
				novalidate
				bind:this={formField}
				transition:slide|local={{ duration: 300, easing: cubicOut }}
			>
				<div class="alert alert-info my-2 shadow-lg">
					<Info class="h-6 w-6 shrink-0 stroke-current" />
					<span
						>Please provide MQTT topics for the different control messages. Wildcards are only
						supported for subscription (set) topics. The use of wildcards for publishing (status)
						topics is not supported.
					</span>
				</div>
				<div class="grid w-full grid-cols-1 content-center gap-x-4 gap-y-2 px-4">
					<div>
						<label class="label" for="status_topic">MQTT Status Topic</label>
						<input
							type="text"
							class="input w-full invalid:border-error invalid:border-2 {formErrors.status
								? 'border-error border-2'
								: ''}"
							bind:value={brokerSettings.status_topic}
							id="status_topic"
							min="0"
							max="64"
							required
						/>
						<label class="label" for="status_topic">
							<span class="text-error {formErrors.status ? '' : 'hidden'}"
								>MQTT status topic is limited to 64 characters</span
							>
						</label>
					</div>
					<div>
						<label class="label" for="control">
							<span class="label-text text-md">Control Status Topic</span>
						</label>
						<input
							type="text"
							class="input w-full invalid:border-error invalid:border-2 {formErrors.control_pub
								? 'border-error border-2'
								: ''}"
							bind:value={brokerSettings.control_topic_pub}
							id="control"
							min="0"
							max="64"
							required
						/>
						<label class="label" for="control">
							<span class="label-text-alt text-error {formErrors.control_pub ? '' : 'hidden'}"
								>MQTT topic is limited to 64 characters</span
							>
						</label>
					</div>
					<div>
						<label class="label" for="control_sub">
							<span class="label-text text-md">Control Set Topic</span>
						</label>
						<input
							type="text"
							class="input w-full invalid:border-error invalid:border-2 {formErrors.control_sub
								? 'border-error border-2'
								: ''}"
							bind:value={brokerSettings.control_topic_sub}
							id="control_sub"
							min="0"
							max="64"
							required
						/>
						<label class="label" for="control_sub">
							<span class="label-text-alt text-error {formErrors.control_sub ? '' : 'hidden'}"
								>MQTT topic is limited to 64 characters</span
							>
						</label>
					</div>
					<div>
						<label class="label" for="environment">
							<span class="label-text text-md">Environment Status Topic</span>
						</label>
						<input
							type="text"
							class="input w-full invalid:border-error invalid:border-2 {formErrors.environment
								? 'border-error border-2'
								: ''}"
							bind:value={brokerSettings.environment_topic_pub}
							id="environment"
							min="0"
							max="64"
							required
						/>
						<label class="label" for="environment">
							<span class="label-text-alt text-error {formErrors.environment ? '' : 'hidden'}"
								>MQTT topic is limited to 64 characters</span
							>
						</label>
					</div>
					<div>
						<label class="label" for="safestate">Safe State Status Topic </label>
						<input
							type="text"
							class="input w-full invalid:border-error invalid:border-2 {formErrors.safestate_pub
								? 'border-error border-2'
								: ''}"
							bind:value={brokerSettings.safestate_topic_pub}
							id="safestate"
							min="0"
							max="64"
							required
						/>
						<label class="label" for="safestate">
							<span class=" text-error {formErrors.safestate_pub ? '' : 'hidden'}"
								>MQTT topic is limited to 64 characters</span
							>
						</label>
					</div>
					<div>
						<label class="label" for="safestate_sub">Safe State Set Topic (Heartbeat) </label>
						<input
							type="text"
							class="input w-full invalid:border-error invalid:border-2 {formErrors.safestate_sub
								? 'border-error border-2'
								: ''}"
							bind:value={brokerSettings.safestate_topic_sub}
							id="safestate_sub"
							min="0"
							max="64"
							required
						/>
						<label class="label" for="safestate_sub">
							<span class=" text-error {formErrors.safestate_sub ? '' : 'hidden'}"
								>MQTT topic is limited to 64 characters</span
							>
						</label>
					</div>
					<div>
						<label class="label" for="identification">Identification Status Topic </label>
						<input
							type="text"
							class="input w-full invalid:border-error invalid:border-2 {formErrors.identification
								? 'border-error border-2'
								: ''}"
							bind:value={brokerSettings.identification_topic_pub}
							id="identification"
							min="0"
							max="64"
							required
						/>
						<label class="label" for="identification">
							<span class=" text-error {formErrors.identification ? '' : 'hidden'}"
								>MQTT topic is limited to 64 characters</span
							>
						</label>
					</div>
				</div>
				<div class="divider mb-2 mt-0"></div>
				<div class="mx-4 flex flex-wrap justify-end gap-2">
					<button class="btn btn-primary" type="submit">Apply Settings</button>
				</div>
			</form>
		{/await}
	</div>
</SettingsCard>
