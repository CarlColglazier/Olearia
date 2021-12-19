# Olearia

A collection of applets for the Daisy Patch.

| Mode   | CV (L)    | CV (R)    | Audio (L)  | Audio (R)  |
|--------|-----------|-----------|------------|------------|
| VCO FM | Pitch     | Mod Index |            |            |
| VCA    | Level (1) | Level (2) | Signal (1) | Signal (2) |
| Noise  | Level (1) | Level (2) |            |            |

All outputs direct through the audio outs.

## Development

To get this set up on a new system, I found I had to build libDaisy separately first:

```
git clone <URL>
git submodule update --init
cd lib/libDaisy
make
cd ../..
make
```

