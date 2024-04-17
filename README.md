# IRC Relay

[![Compile Plugin](https://github.com/d03n3rfr1tz3/bzflag-ircRelay/actions/workflows/build.yml/badge.svg)](https://github.com/d03n3rfr1tz3/bzflag-ircRelay/actions/workflows/build.yml)
[![GitHub release](https://img.shields.io/github/release/d03n3rfr1tz3/bzflag-ircRelay.svg)](https://github.com/d03n3rfr1tz3/bzflag-ircRelay/releases/latest)
![Minimum BZFlag Version](https://img.shields.io/badge/BZFlag-v2.4.0+-blue.svg)
[![License](https://img.shields.io/github/license/d03n3rfr1tz3/bzflag-ircRelay.svg)](LICENSE.md)

This plugin is a revival of the IRC Relay plugin from the BZFlag forum user `wegstar`.
It should work with version 2.4.x and also replaces the usage of a separate config.txt
with the use of BZDB variables and has some minor additional features.

## Requirements

This plug-in follows [my standard instructions for compiling plug-ins](https://github.com/allejo/docs.allejo.io/wiki/BZFlag-Plug-in-Distribution).

## Usage

### Loading the plug-in

You should specify any command line arguments that are needed or lack thereof

```
-loadplugin ircRelay
```

### Custom BZDB Variables

These custom BZDB variables can be configured with `-set` in configuration files and may be changed at any time in-game by using the `/set` command.

```
-set <name> <value>
```

| Name | Type | Default | Description |
| ---- | ---- | ------- | ----------- |
| `_ircAddress` | string |  | Required. The IP address of your IRC server. |
| `_ircChannel` | string |  | Required. The channel your IRC Relay should join. |
| `_ircNick` | string |  | Required. The nickname your IRC Relay should use. |
| `_ircPass` | string |  | Optional. The password for the IRC server. |
| `_ircAuthType` | string |  | Optional. The authentication type of the IRC server. Choose one: `AuthServ`, `NickServ` or `Q`. |
| `_ircAuthPass` | string |  | Optional. The authentication password for the IRC server. |
| `_ircIgnore` | string |  | Optional. Comma separated list of ignored IRC users. Messages from these users will not be passed into the BZFlag chat. |

## License

[LICENSE](LICENSE.md)
