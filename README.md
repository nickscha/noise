# noise
A C89 standard compliant, single header, nostdlib (no C Standard Library) Noise Generation (NOISE).

For more information please look at the "noise.h" file or take a look at the "examples" or "tests" folder.

> [!WARNING]
> THIS PROJECT IS A WORK IN PROGRESS! ANYTHING CAN CHANGE AT ANY MOMENT WITHOUT ANY NOTICE! USE THIS PROJECT AT YOUR OWN RISK!

<p align="center">
  <a href="https://github.com/nickscha/noise/releases">
    <img src="https://img.shields.io/github/v/release/nickscha/noise?style=flat-square&color=blue" alt="Latest Release">
  </a>
  <a href="https://github.com/nickscha/noise/releases">
    <img src="https://img.shields.io/github/downloads/nickscha/noise/total?style=flat-square&color=brightgreen" alt="Downloads">
  </a>
  <a href="https://opensource.org/licenses/MIT">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square" alt="License">
  </a>
  <img src="https://img.shields.io/badge/Standard-C89-orange?style=flat-square" alt="C Standard">
  <img src="https://img.shields.io/badge/nolib-nostdlib-lightgrey?style=flat-square" alt="nostdlib">
</p>

## **Features**
- **C89 compliant** — portable and legacy-friendly  
- **Single-header API** — just include `noise.h`  
- **nostdlib** — no dependency on the C Standard Library  
- **Minimal binary size** — optimized for small executables  
- **Cross-platform** — Windows, Linux, MacOs 
- **Strict compilation** — built with aggressive warnings & safety checks  

## Quick Start

Download or clone noise.h and include it in your project.

```C
#include "noise.h" /* Noise Generation */

int main() {

    float noise_value;
    float rotation_2x2[2][2] = {
          {0.80f, -0.60f},
          {0.60f, 0.80f}
    };

    /* Set a perlin noise and pseudo random number generator seed */
    noise_perlin_seed(42); 
    noise_lcg_state = 21;
    
    /* 2D perlin noise (x, y, frequency) */
    noise_value = noise_perlin_2(1.0f, 2.0f, 0.010f);

    /* 2D fractal perlin noise octaves with no rotation (x, y, frequency, octaves, lacunarity, gain)*/
    noise_value = noise_perlin_2_fbm(1.0f, 2.0f, 0.010f, 4, 2.0f, 0.5f);

    /* 2D fractal perlin noise octaves with rotation */
    noise_value = noise_perlin_2_fbm_rotation(1.0f, 2.0f, 0.010f, 4, 2.0f, 0.5f, rotation_2x2);

    /* 3D perlin noise (x, y, z, frequency) */
    noise_value = noise_perlin_3(1.0f, 2.0f, -5.0f, 0.010f);

    /* All 2D / 3D functions follow the same naming scheme */

    return 0;
}
```

## Run Example: nostdlib, freestsanding

In this repo you will find the "examples/noise_win32_nostdlib.c" with the corresponding "build.bat" file which
creates an executable only linked to "kernel32" and is not using the C standard library and executes the program afterwards.

## "nostdlib" Motivation & Purpose

nostdlib is a lightweight, minimalistic approach to C development that removes dependencies on the standard library. The motivation behind this project is to provide developers with greater control over their code by eliminating unnecessary overhead, reducing binary size, and enabling deployment in resource-constrained environments.

Many modern development environments rely heavily on the standard library, which, while convenient, introduces unnecessary bloat, security risks, and unpredictable dependencies. nostdlib aims to give developers fine-grained control over memory management, execution flow, and system calls by working directly with the underlying platform.

### Benefits

#### Minimal overhead
By removing the standard library, nostdlib significantly reduces runtime overhead, allowing for faster execution and smaller binary sizes.

#### Increased security
Standard libraries often include unnecessary functions that increase the attack surface of an application. nostdlib mitigates security risks by removing unused and potentially vulnerable components.

#### Reduced binary size
Without linking to the standard library, binaries are smaller, making them ideal for embedded systems, bootloaders, and operating systems where storage is limited.

#### Enhanced performance
Direct control over system calls and memory management leads to performance gains by eliminating abstraction layers imposed by standard libraries.

#### Better portability
By relying only on fundamental system interfaces, nostdlib allows for easier porting across different platforms without worrying about standard library availability.
