# HarmonyLink Unreal Engine Client Plugin

This repository contains the Unreal Engine client plugin for the HarmonyLink API, providing real-time device metrics for handheld devices to enhance gameplay and immersion.

## Features

- **Device Identification**: The plugin allows Unreal Engine games to identify the specific handheld device they're operating on.
- **Real-Time Metrics**: Unreal Engine games can access real-time data about the handheld device's power usage and docking status via this plugin.
- **Adaptive Game Settings**: Use the power metrics and docking status to adapt the game’s quality settings for a more customized gaming experience.

## Requirements

- Unreal Engine 4.x/5.x
- HarmonyLink Server (native Linux or Windows)

## Getting Started

1. Clone this repository to your local machine.
2. Copy the `HarmonyLinkPlugin` folder into your Unreal Engine project's `Plugins` folder.
3. Open your Unreal Engine project. The plugin should be automatically detected. If not, enable it manually in the Plugins window.

## Usage

1. Use the HarmonyLink API functions provided by this plugin in your game's code to access device metrics and docking status.
2. Add the included `DataRefreshActor` to your levels for automatic api updates or request data manually from the functions (does not update on tick)
3. Adjust your game's settings based on these metrics to optimize the gaming experience on handheld devices.

If you have not yet made the jump to Unreal Engine 5 you will need to edit the `HarmonyLinkUE.uplugin` and change the engine version to the desired engine.

⚠️ HarmonyLinkUE will only work if you run [HarmonyLinkServer](https://github.com/Jordonbc/HarmonyLinkServer)

## Contributing

We appreciate your contributions! Please fork this repository and make your changes in a separate branch. Then submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).

## Contact

For more information, feel free to reach out or open an issue.

## Acknowledgements

- The HarmonyLink Server application can be found [here](https://github.com/Jordonbc/HarmonyLinkServer).
