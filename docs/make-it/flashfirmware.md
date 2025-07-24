# Flash Firmware

Flashing the firmware is easy and doesn't require you to install any development tools. You can do this through a compatible browser which supports webserial like Chrome. The OSSM reference board will enter its bootloader mode automatically.

1. Plug in your OSSM board to an USB port and wait for the USB port to enumerate.
2. Hit "Install" and select the correct COM port (USB JTAG/serial debug unit).
3. After the installation has completed press the `Reset` button once again.

<p class="button-row">
<esp-web-install-button manifest='../firmware/manifest.json'>
  <button slot="activate" class="md-button md-button--primary">INSTALL</button>
  <span slot="unsupported">Use Chrome Desktop</span>
  <span slot="not-allowed">Not allowed to use this without SSL</span>
</esp-web-install-button>
</p>

Powered by [ESP Web Tools](https://esphome.github.io/esp-web-tools/)

<script type="module" src="https://unpkg.com/esp-web-tools@10/dist/web/install-button.js?module"></script>
