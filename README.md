# Olearia

A collection of applets for the Daisy Patch.

| Mode   | CV (L)    | CV (R)    | Audio (L)  | Audio (R)  |
|--------|-----------|-----------|------------|------------|
| VCO FM | Pitch     | Mod Index |            |            |
| VCA    | Level (1) | Level (2) | Signal (1) | Signal (2) |
| Noise  | Level (1) | Level (2) |            |            |

All outputs direct through the audio outs.

## Development


The repository includes libDaisy and DaisySP as submodules. Keep this in mind when cloning:

```
git clone <URL>
git submodule update --init
make
```

## Development Environment

I personally [use](https://carlcolglazier.com/tools/) an M1 MacBook
and have found it is simplest to use containers to actually build
the binary.

``` sh
docker build --platform linux/amd64 --tag olearia --file Dockerfile .
```
