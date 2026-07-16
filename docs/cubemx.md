# Prebuilt CubeMX HAL

CI can't run STM32CubeMX. So generated HAL is produced once, archived, and
restored at build time. Archives are keyed by the `.ioc` sha256
(`<name>-<iochash>.tar.gz`), so the HAL always matches its `.ioc`.

One script does it: [`scripts/cubemx.py`](../scripts/cubemx.py) with `ensure`
(consumer, runs in CI/build) and `package` (producer, needs CubeMX). It reads a
per-board manifest and calls the generator (`generate_cubemx_hal.sh`) when it
must generate.

## Manifest

`src/boards/<vendor>/<board>/cubemx.json`:

```json
{
  "name": "salt-pmu-gimbal",
  "ioc": "board/project.ioc",
  "store": { "repo": "<owner>/<repo>", "tag": "<release-tag>" }
}
```

- `name` — archive prefix. `ioc` — path relative to the manifest.
- `store` — GitHub release that hosts archives (supplied by the app; node-core
  has no default). Override at runtime with `CUBEMX_ARCHIVE_REPO` / `CUBEMX_CACHE_TAG`.

### Tag versioning

The archive is addressed by **(tag, `.ioc` hash)**: the asset name encodes the
`.ioc`, the **`store.tag`** (`cubemx-vN`) encodes the **generation environment**
(STM32CubeMX version + `generate_cubemx_hal.sh`) — the part the hash can't see.

- Editing a `.ioc` → new hash, same tag (handled automatically).
- Upgrading CubeMX / changing the generator so HAL output differs → **bump the
  tag** (`cubemx-v1` → `cubemx-v2`), regenerate, re-upload, update `store.tag`.
- Old commits keep their old `store.tag`, so they still restore matching HAL —
  builds stay reproducible. It is **not** the firmware/product version.

## Use

```bash
# Producer (has CubeMX): generate, archive, and upload to the release.
third_party/node-core/scripts/cubemx.py package --manifest src/boards/<v>/<b>/cubemx.json \
    --out build/cubemx/<v>/<b> --upload

# Consumer (CI/build): restore the matching archive (or generate if CubeMX is present).
third_party/node-core/scripts/cubemx.py ensure --manifest src/boards/<v>/<b>/cubemx.json \
    --out build/cubemx/<v>/<b>
```

`ensure` resolves in order: existing project → local cache → local CubeMX →
download from the release.

Pass `--release-only` when a target must always consume the published project.
This ignores an installed local CubeMX, an existing generated project, and the
local archive cache. Mini v3 currently uses this as a deliberate temporary
policy so its optional IMU/DSP generation path cannot slow ordinary builds.

For Mini v3, make the archive self-contained before publishing it: copy the DSP
implementation files used by `cmsisDspSources` from the ARM CMSIS pack selected
by the `.ioc` into the matching directories under the generated
`Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/` tree.

## Wiring into a build

Point the board at the restored project and run `ensure` before building. In the
app Makefile target:

```make
your_target:
	python3 third_party/node-core/scripts/cubemx.py ensure \
		--manifest $(APP_SRC)/boards/<v>/<b>/cubemx.json \
		--out $(BUILD_DIR)/cubemx/<v>/<b>
	@bash "$(BUILD_NODE)" --board "<v>/<b>" --target "<t>" ... \
		-- -DSTM32_CUBEMX_PROJECT_PATH=$(BUILD_DIR)/cubemx/<v>/<b>
```
