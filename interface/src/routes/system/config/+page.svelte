<script lang="ts">
	import type { PageData } from './$types';
	import Configuration from './Configuration.svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';

	interface Props {
		data: PageData;
	}

	let { data }: Props = $props();
</script>

<div
	class="mx-0 my-1 flex flex-col space-y-4
     sm:mx-8 sm:my-8"
>
	<Configuration />
</div>
