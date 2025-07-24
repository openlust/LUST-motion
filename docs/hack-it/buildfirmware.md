# Setup Build Environment

NoisyCricket builds on top of [ESP32-Sveltekit](https://theelims.github.io/ESP32-sveltekit/) for its firmware core functions. Please make sure to familiarize yourself with the docs to understand the firmware.

## Prerequisites

This project has quite a complicated build chain to prepare the frontend code for the ESP32. You will need to install some tools to make this all work, starting with a powerful code editor.

### Softwares to Install

Please install the following software, if you haven't already:

- [VSCode](https://code.visualstudio.com/) - IDE for development
- [Node.js](https://nodejs.org) - For building the interface with npm

### VSCode Plugins and Setups

Please install the following mandatory VSCode Plugins:

- [PlatformIO](https://platformio.org/) - Embedded development platform
- [Prettier](https://prettier.io/) - Automated code formatter
- Svelte for VS Code - Makes working with Svelte much easier
- Svelte Intellisense - Another Svelte tool
- Tailwind CSS Intellisense - Makes working with Tailwind CSS much easier
- [Prettier plugin for Tailwind CSS](https://github.com/tailwindlabs/prettier-plugin-tailwindcss) - Automatically sorts the Tailwind classes into their recommended order

Lastly, if you want to make use of Materials for MkDocs as your documentation engine, install [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) by typing the following into the VSCode terminal:

```bash
pip install mkdocs-material
```

!!! tip

    You might need to run this as administrator, if you getting an error message.

### Project Structure

| Resource                                                                                     | Description                                                      |
| -------------------------------------------------------------------------------------------- | ---------------------------------------------------------------- |
| [.github/](https://github.com/openlust/noisycricket/blob/main/.github)                       | Github CI pipeline to deploy MkDocs to gh-pages                  |
| [docs/](https://github.com/openlust/noisycricket/blob/main/docs)                             | MkDocs documentation files                                       |
| [hardware/](https://github.com/openlust/noisycricket/blob/main/hardware)                     | Hardware files for PCB and cases                                 |
| [interface/](https://github.com/openlust/noisycricket/blob/main/interface)                   | SvelteKit based front end                                        |
| [lib/](https://github.com/openlust/noisycricket/blob/main/lib/)                              | Libraries and main part of the code                              |
| [src/](https://github.com/theelims/openlust/noisycricket/blob/main/src)                      | The main.cpp                                                     |
| [scripts/](https://github.com/theelims/openlust/noisycricket/tree/main/scripts)              | Scripts that build the interface as part of the platformio build |
| [platformio.ini](https://github.com/theelims/openlust/noisycricket/blob/main/platformio.ini) | PlatformIO project configuration file                            |
| [mkdocs.yaml](https://github.com/theelims/openlust/noisycricket/blob/main/mkdocs.yaml)       | MkDocs project configuration file                                |

## Setting up PlatformIO

### Build & Upload Process

Select your board's hardware revision as build environment under the PlatformIO tab and hit `Upload and Monitor`. This will download all ESP32 libraries and execute `node install` to install all node packages as well.

The first build process will take a while. After a couple of minutes you can see the ESP32 outputting information on the terminal. Some of the python scripts might need to install additional packages. In that case the first build process will fail. Just run it a second time.

!!! tip "Use several terminals in parallel"

    VSCode allows you to have more then one terminal running at the same time. You can dedicate one terminal to the serial monitor, while having the development server running in an other terminal.

## Setting up SvelteKit

### Setup Proxy for Development

To ease the frontend development you can deploy the back end code on a Noisy Cricket board and pass the websocket and REST API calls through the development server's proxy.
The [vite.config.ts](https://github.com//openlust/noisycricket/blob/main/interface/vite.config.ts) file defines the location of the services which the development server will proxy. This is defined by the "target" property, which will need to be changed to the the IP address or hostname of the device running the firmware. Change this for both, "http://" and "ws://".

```ts
proxy: {
    // Proxying REST: http://localhost:5173/rest/bar -> http://192.168.1.83/rest/bar
    '/rest': {
    target: 'http://192.168.1.83',
    changeOrigin: true,
    },
    // Proxying websockets ws://localhost:5173/ws -> ws://192.168.1.83/ws
    '/ws': {
    target: 'ws://192.168.1.83',
    changeOrigin: true,
    ws: true,
    },
},
```

!!! tip

    You must restart the development server for changes of the proxy location to come into effect.

### Development Server

The interface comes with Vite as a development server. It allows hot module reloading reflecting code changes to the front end instantly in your browser. Open a new terminal session and execute the following commands:

```bash
cd interface
npm run dev
```

Follow the link to access the front end in your browser.

## Setup Material for mkdocs

MkDocs comes with a build-in development server which supports hot reload as well. Open a new terminal session in VSCode and type

```
mkdocs serve
```
