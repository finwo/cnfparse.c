finwo/cnfparse
==============

> Small library to make config parsing easier

This library makes use of [dep](https://github.com/finwo/dep) to manage its
dependencies and exports.

Installation
------------

```sh
dep add finwo/cnfparse
dep install
```

After that, simply add `include lib/.dep/config.mk` in your makefile and include
the header file by adding `#include "finwo/cnfparse.h`.

API
---

### Structures

<details>
  <summary>struct cnf_directive</summary>

  The main handle of the cnfparse library

```C
struct cnf_directive {
 char   *name;
 size_t argc;
 char   **argv;
};
```

</details>

### Methods

<details>
  <summary>cnf_directive_free(subject)</summary>

  Frees the memory used by a directive

```C
void cnf_directive_free(struct cnf_directive *subject);
```

</details>
<details>
  <summary>cnf_directive_read(fd)</summary>

  Returns a single directive (or NULL) from the file descriptor

```C
struct cnf_directive * cnf_directive_read(FILE *fd);
```

</details>

License
-------

cnfparse.c source code is available under the [FGPL License](LICENSE.md)
