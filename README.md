# IRC Relay

[![Compile Plugin](https://github.com/d03n3rfr1tz3/bzflag-ircRelay/actions/workflows/build.yml/badge.svg)](https://github.com/d03n3rfr1tz3/bzflag-ircRelay/actions/workflows/build.yml)
[![GitHub release](https://img.shields.io/github/release/d03n3rfr1tz3/bzflag-ircRelay.svg)](https://github.com/d03n3rfr1tz3/bzflag-ircRelay/releases/latest)
![Minimum BZFlag Version](https://img.shields.io/badge/BZFlag-v2.4.0+-blue.svg)
[![License](https://img.shields.io/github/license/d03n3rfr1tz3/bzflag-ircRelay.svg)](LICENSE.md)

This plugin is a revival of the IRC Relay plugin from the BZFlag forum user `wegstar`.
It should work with version 2.4.x and also replaces the usage of a separate config.txt
with the use of the plug-in configuration and BZDB variables and has some minor additional features.

## Requirements

This plug-in follows [my standard instructions for compiling plug-ins](https://github.com/allejo/docs.allejo.io/wiki/BZFlag-Plug-in-Distribution).

## Usage

### Loading the plug-in

The plug-in is configured by the plug-in configuration, which is given after the comma of the `-loadplugin` option. It can either be the path to a configuration file (recommended) or an inline list of `key=value` pairs.

```
-loadplugin ircRelay,/path/to/ircRelay.conf
-loadplugin ircRelay,address=irc.example.org,port=6667,pass=secret
```

The configuration file contains one `key=value` pair per line. Empty lines and lines starting with `#` are ignored.

```
# connection settings, only available in the plug-in configuration
address=irc.example.org
port=6667
pass=secret
authType=NickServ
authPass=secret

# channel settings, alternatively available as BZDB variables
channel=bzflag
nick=MyBZFlagServer
ignore=SomeBot,AnotherBot
prefix=[BZFlag]-
```

Inline values cannot contain a comma and are visible in the process list (e.g. `ps` or `docker inspect`), so using a configuration file is recommended.

| Key | Type | Default | Description |
| --- | ---- | ------- | ----------- |
| `address` | string |  | Required. The IP address or hostname of your IRC server. |
| `port` | int | 6667 | Optional. The port of your IRC server. |
| `pass` | string |  | Optional. The password for the IRC server. |
| `authType` | string |  | Optional. The authentication type of the IRC server. Choose one: `AuthServ`, `NickServ` or `Q`. |
| `authPass` | string |  | Optional. The authentication password for the IRC server. |
| `channel` | string |  | Required. The channel your IRC Relay should join. Alternatively `_ircChannel`. |
| `nick` | string |  | Required. The nickname your IRC Relay should use. Alternatively `_ircNick`. |
| `ignore` | string |  | Optional. Comma separated list of ignored IRC users. Alternatively `_ircIgnore`. |
| `prefix` | string |  | Optional. Prefix for all messages sent into the IRC channel. Alternatively `_ircPrefix`. |

### Custom BZDB Variables

BZDB variables are sent to every connected client and are visible in public server lists. That's why the connection settings
(`address`, `port`, `pass`, `authType` and `authPass`) are only available in the plug-in configuration.

The remaining settings can also be configured as custom BZDB variables with `-set` in configuration files and may be changed at any time in-game by using the `/set` command.
If a setting is given in the plug-in configuration, its BZDB variable does not exist at all, so it stays private and a `-set` on it will be rejected by the server.

```
-set <name> <value>
```

| Name | Type | Default | Description |
| ---- | ---- | ------- | ----------- |
| `_ircChannel` | string |  | Required. The channel your IRC Relay should join. Changing it reconnects the IRC Relay. |
| `_ircNick` | string |  | Required. The nickname your IRC Relay should use. |
| `_ircIgnore` | string |  | Optional. Comma separated list of ignored IRC users. Messages from these users will not be passed into the BZFlag chat. |
| `_ircPrefix` | string |  | Optional. Prefix for all messages sent into the IRC channel. |

### Messages for a specific game

If you run several game servers with an IRC Relay in the same channel, an IRC message can be sent to only one of them
by starting it with the tag of that game in brackets, followed by a space, e.g. `[BZFlag] Hello`.
The tag is taken from the brackets at the start of `prefix`. A message with the own tag is passed into the BZFlag chat
without the tag, a message with any other tag is not passed at all. Messages without a tag are passed as usual.
German umlauts are replaced by their ASCII spelling (e.g. `ä` by `ae`), because BZFlag cannot show them.

### Upgrading from older versions

Older versions used the BZDB variables `_ircAddress`, `_ircPort`, `_ircPass`, `_ircAuthType` and `_ircAuthPass`.
These do not exist anymore, so remove their `-set` lines and move the values into the plug-in configuration. Otherwise the server will refuse to start.

## License

[LICENSE](LICENSE.md)
