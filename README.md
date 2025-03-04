# tstat

Linux utility to get the status of some devices. `tstat` is heavily inspired by [slstatus](https://tools.suckless.org/slstatus/).

Currently, `tstat` can display the following information:

* Date and time
* Battery status and percentage
* Volume status and percentage
* Wifi status
* Bluetooth status

## Configuration

`tstat` is configured via the `config.h` and `config.mk` files. `tstat` must be re-compiled and re-installed when a new configuration is set.

### `config.h`

This file is where customisation of the status text and devices takes place.

### `config.mk`

This file is where the compilation and installation options are specified. By default, `gcc` is used to compile and installation is done under `/usr/local`.

## Requirements

In order to build `tstat`, libX11, alsa-lib and libpulse are required.

## Installation

```
git clone https://github.com/icanalesm/tstat.git
cd tstat
```

Set the configuration in `config.h` and `config.mk`.

Compile and install

```
make
sudo make install
```

## Execution

To get the status, simple execute

```
tstat
```

For window managers that use the `WM_NAME X11` property of the root window to set the status bar (e.g. [dwm](https://dwm.suckless.org/)), `tstat` can be executed with the `set` command

```
tstat set
```

## License

`tstat` is licensed under the MIT License; see [LICENSE](LICENSE).

Some of the code is either modified code or code taken from other projects; see [NOTICE](NOTICE).
