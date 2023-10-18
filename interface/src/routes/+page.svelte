<script lang="ts">
	import type { PageData } from './$types';
	import { notifications } from '$lib/components/toasts/notifications';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { analytics } from '$lib/stores/analytics';
	import { Chart, registerables } from 'chart.js';
	import * as LuxonAdapter from 'chartjs-adapter-luxon';
	import ChartStreaming from '@robloche/chartjs-plugin-streaming';
	import { onMount, onDestroy } from 'svelte';
	import Download from '~icons/tabler/download';
	import Start from '~icons/tabler/player-play';
	import Stop from '~icons/tabler/player-stop';
	import { decode } from 'cbor-x/decode';
	import { daisyColor } from '$lib/DaisyUiHelper';

	export let data: PageData;

	Chart.register(...registerables);
	Chart.register(LuxonAdapter);
	Chart.register(ChartStreaming);

	let pressureChartElement: HTMLCanvasElement;
	let pressureChart: Chart;

	let dataSocket: WebSocket;
	let unresponsiveTimeoutData: number;
	let timeSync: number = 0;
	let lastPressure: number = 1024;
	let lastdpdt: number = 0;

	function openDataSocket() {
		dataSocket = new WebSocket('ws://' + $page.url.host + '/ws/rawData');
		dataSocket.binaryType = 'arraybuffer';
		console.log(`trying to connect to: ${dataSocket.url}`);

		dataSocket.onopen = () => {
			console.log(`connection open to: ${dataSocket.url}`);
		};

		dataSocket.onclose = () => {
			console.log(`connection closed to: ${dataSocket.url}`);
		};

		dataSocket.onmessage = (event) => {
			// Reset a timer to detect unresponsiveness
			clearTimeout(unresponsiveTimeoutData);
			unresponsiveTimeoutData = setTimeout(() => {
				console.log('Server is unresponsive');
				dataSocket.close();
			}, 5000); // Detect unresponsiveness after 1 seconds

			const data = decode(new Uint8Array(event.data));

			if (timeSync == 0) {
				timeSync = Date.now() - data[0][0];
			}

			for (let i = 0; i < data.length; i++) {
				let dpdt = (data[i][2] * 0.01 - lastPressure) * 40;
				let d2pdt2 = lastdpdt - dpdt;
				pressureChart.data.datasets[0].data.push({
					x: timeSync + data[i][0],
					y: data[i][1] * 0.01
				});
				pressureChart.data.datasets[1].data.push({
					x: timeSync + data[i][0],
					y: data[i][2] * 0.01
				});
				pressureChart.data.datasets[2].data.push({
					x: timeSync + data[i][0],
					y: dpdt
				});
				pressureChart.data.datasets[3].data.push({
					x: timeSync + data[i][0],
					y: d2pdt2
				});
				lastPressure = data[i][2] * 0.01;
				lastdpdt = dpdt;
			}
		};

		dataSocket.onerror = () => {
			console.log(`connection error with: ${dataSocket.url}`);
		};
	}

	onDestroy(() => {
		dataSocket.close();
	});

	onMount(() => {
		openDataSocket();
		pressureChart = new Chart(pressureChartElement, {
			type: 'line',
			data: {
				datasets: [
					{
						// Position
						borderColor: daisyColor('--p'),
						fill: false,
						pointRadius: 0,
						data: [],
						yAxisID: 'y'
					},
					{
						// Speed
						backgroundColor: daisyColor('--s', 20),
						borderColor: daisyColor('--s'),
						fill: true,
						pointRadius: 0,
						data: [],
						yAxisID: 'y1'
					}
				]
			},
			options: {
				animation: false,
				responsive: true,
				maintainAspectRatio: false,
				interaction: {
					mode: 'index',
					intersect: false
				},
				plugins: {
					// Change options for ALL axes of THIS CHART
					streaming: {
						duration: 20000,
						refresh: 25,
						delay: 100
					},
					tooltip: {
						enabled: false
					},
					legend: {
						display: false
					}
				},
				scales: {
					x: {
						type: 'realtime',
						grid: {
							color: daisyColor('--bc', 10)
						},
						ticks: { color: daisyColor('--bc') }
					},
					y: {
						type: 'linear',
						title: {
							display: true,
							text: 'Position [mm]',
							color: daisyColor('--p'),
							font: {
								size: 16,
								weight: 'bold'
							}
						},
						position: 'left',
						min: 0,
						max: 150,
						grid: { color: daisyColor('--bc', 10) },
						ticks: {
							stepSize: 150 / 6,
							color: daisyColor('--bc')
						},
						border: { color: daisyColor('--bc', 10) }
					},
					y1: {
						type: 'linear',
						title: {
							display: true,
							text: 'Speed [mm/s]',
							color: daisyColor('--s'),
							font: {
								size: 16,
								weight: 'bold'
							}
						},
						position: 'right',
						suggestedMin: -150,
						suggestedMax: 150,
						ticks: {
							stepSize: 150 / 3,
							color: daisyColor('--bc')
						},
						grid: {
							drawOnChartArea: false // only want the grid lines for one axis to show up
						},
						border: { color: daisyColor('--bc', 10) }
					}
				}
			}
		});
	});
</script>

<div class="card bg-base-200 shadow-md shadow-primary/50 mt-3 mb-1.5 mx-auto w-11/12">
	<div class="relative h-72 md:h-96 w-full p-2">
		<canvas bind:this={pressureChartElement} />
	</div>
</div>
<div class="card bg-base-200 shadow-md shadow-primary/50 my-1.5 mx-auto w-11/12">
	<div class="mt-4 mx-4">
		<input
			type="range"
			min="0"
			max="150"
			value="120"
			on:change={() => {
				// nothing
			}}
			class="range range-primary range-xs"
		/>
		<label class="label mt-0 pt-0">
			<span class="label-text">Depth</span>
			<span class="label-text-alt">120 mm</span>
		</label>
	</div>
	<div class="mt-4 mx-4">
		<input
			type="range"
			min="0"
			max="150"
			value="60"
			on:change={() => {
				// nothing
			}}
			class="range range-primary range-xs"
		/>
		<label class="label mt-0 pt-0">
			<span class="label-text">Stroke</span>
			<span class="label-text-alt">60 mm</span>
		</label>
	</div>
	<div class="mt-4 mx-4">
		<input
			type="range"
			min="0"
			max="100"
			value="20"
			on:change={() => {
				// nothing
			}}
			class="range range-primary range-xs"
		/>
		<label class="label mt-0 pt-0">
			<span class="label-text">Speed</span>
			<span class="label-text-alt">20 FPM</span>
		</label>
	</div>
	<div class="mt-4 mx-4">
		<input
			type="range"
			min="-100"
			max="100"
			value="0"
			on:change={() => {
				// nothing
			}}
			class="range range-primary range-xs"
		/>
		<label class="label mt-0 pt-0">
			<span class="label-text">Sensation</span>
			<span class="label-text-alt">0</span>
		</label>
	</div>

	<div class="m-4 flex flex-wrap gap-6 justify-between">
		<div class="flex flex-nowrap justify-start gap-6">
			<button class="btn btn-primary inline-flex items-center w-32">
				<Start class="mr-2 h-5 w-5" /><span>Start</span>
			</button>
			<select class="select select-primary w-52">
				<option>Depth Adjustment</option>
				<option>Streaming</option>
				<option>Pounding or Teasing</option>
				<option>Robo Stroke</option>
				<option>Half'n'Half</option>
				<option>Deeper</option>
				<option>Stop'n'Go</option>
				<option>Insist</option>
			</select>
		</div>
	</div>
</div>
