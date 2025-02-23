# Bazel build

## Using the Pico SDK in a Bazel project.

### Add pico-sdk as a dependency
First, in your `MODULE.bazel` file, add a dependency on the Pico SDK:
```python
bazel_dep(
  name = "pico-sdk",
  version = "2.0.0",
)
```

Second, in the same file you'll need to add an explicit dependency on
`rules_cc`, as it's a special-cased Bazel module:
```python
# Note: rules_cc is special-cased repository; a dependency on rules_cc in a
# module will not ensure that the root Bazel module has that same version of
# rules_cc. For that reason, this primarily acts as a FYI. You'll still need
# to explicitly list this dependency in your own project's MODULE.bazel file.
bazel_dep(name = "rules_cc", version = "0.0.9")

# rules_cc v0.0.10 is not yet cut, so manually pull in the desired version.
# This does not apply to dependent projects, so it needs to be copied to your
# project's MODULE.bazel too.
archive_override(
    module_name = "rules_cc",
    urls = "https://github.com/bazelbuild/rules_cc/archive/1acf5213b6170f1f0133e273cb85ede0e732048f.zip",
    strip_prefix = "rules_cc-1acf5213b6170f1f0133e273cb85ede0e732048f",
    integrity = "sha256-NddP6xi6LzsIHT8bMSVJ2NtoURbN+l3xpjvmIgB6aSg=",
)
```

### Register toolchains
These toolchains tell Bazel how to compile for ARM cores. Add the following
to the `MODULE.bazel` for your project:
```python
register_toolchains(
    "@pico-sdk//bazel/toolchain:linux-x86_64-rp2040",
    "@pico-sdk//bazel/toolchain:linux-x86_64-rp2350",
    "@pico-sdk//bazel/toolchain:win-x86_64-rp2040",
    "@pico-sdk//bazel/toolchain:win-x86_64-rp2350",
    "@pico-sdk//bazel/toolchain:mac-x86_64-rp2040",
    "@pico-sdk//bazel/toolchain:mac-x86_64-rp2350",
    "@pico-sdk//bazel/toolchain:mac-aarch64-rp2040",
    "@pico-sdk//bazel/toolchain:mac-aarch64-rp2350",
)
```

### Enable required .bazelrc flags
To use the toolchains provided by the Pico SDK, you'll need to enable a few
new features. In your project's `.bazelrc`, add the following
```
# Required for new toolchain resolution API.
build --incompatible_enable_cc_toolchain_resolution
build --@rules_cc//cc/toolchains:experimental_enable_rule_based_toolchains
```

### Ready to build!
You're now ready to start building Pico Projects in Bazel! When building,
don't forget to specify `--platforms` so Bazel knows you're targeting the
Raspberry Pi Pico:
```console
$ bazelisk build --platforms=@pico-sdk//bazel/platform:rp2040 //...
```

## SDK configuration
An exhaustive list of build system configuration options is available in
`//bazel/config:BUILD.bazel`.

### Selecting a different board
A different board can be selected specifying `--@pico-sdk//bazel/config:PICO_BOARD`:
```console
$ bazelisk build --platforms=//bazel/platform:rp2040 --@pico-sdk//bazel/config:PICO_BOARD=pico_w //...
```

If you have a bespoke board definition, you can configure the Pico SDK to use it
by pointing `--@pico-sdk//bazel/config:PICO_CONFIG_HEADER` to a `cc_library`
that defines `PICO_BOARD` and either a `PICO_CONFIG_HEADER` define or a
`pico/config_autogen.h` header. Make sure any required `includes`, `hdrs`, and
`deps` are also provided.

## Generating UF2 firmware images
Creation of UF2 images can be done as explicit build steps on a per-binary
rule basis, or through an aspect. Running a wildcard build with the
`pico_uf2_aspect` enabled is the easiest way to create a UF2 for every ELF
firmware image.

```console
$ bazelisk build --platforms=@pico-sdk//bazel/platform:rp2040 \
    --aspects @pico-sdk//tools:uf2_aspect.bzl%pico_uf2_aspect \
    --output_groups=+pico_uf2_files \
    //...
```

## Building the Pico SDK itself

### First time setup
You'll need Bazel (v7.0.0 or higher) or Bazelisk (a self-updating Bazel
launcher) to build the Pico SDK.

We strongly recommend you set up
[Bazelisk](https://bazel.build/install/bazelisk).

You will also need a working compiler configured if you wish to build Picotool
or pioasm.

* Linux: `sudo apt-get install build-essential` or similar.
* macOS: `xcode-select --install`
* Windows: [Install MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/)

### Building
To build all of the Pico SDK, run the following command:
```console
$ bazelisk build --platforms=//bazel/platform:rp2040 //...
```

## Known issues and limitations
The Bazel build for the Pico SDK is relatively new, but most features and
configuration options available in the CMake build are also available in Bazel.
You are welcome and encouraged to file issues for any problems and limitations
you encounter along the way.

Currently, the following features are not supported:

* The pioasm parser cannot be built from source via Bazel.
* Windows MSVC wildcard build (`bazel build //...`) does not work when targeting
  host.
* Bazel does not yet provide RISC-V support for Pico 2/RP2350.
* Pico W wireless libraries have link issues.

## Contributing
When making changes to the Bazel build, please run the Bazel validation script
to ensure all supported configurations build properly:

```console
$ ./tools/run_all_bazel_checks.py
```

If you need to check against a local version of Picotool, you can run the script
with `--picotool-dir`:

```console
$ ./tools/run_all_bazel_checks.py --picotool-dir=/path/to/picotool
```
