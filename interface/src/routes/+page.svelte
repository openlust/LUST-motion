<script lang="ts">
	import type { PageData } from './$types';
	import Device from './Device.svelte';
	import Control from './Control.svelte';

	interface Props {
		data: PageData;
	}

	let { data }: Props = $props();
</script>

<div
	class="mx-0 my-1 flex flex-col space-y-4
     sm:mx-8 sm:my-8"
>
	<Device />
	<Control />
</div>
